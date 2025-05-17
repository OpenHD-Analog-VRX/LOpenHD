#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define WIDTH  1024
#define HEIGHT 600

#define DRM_DEVICE "/dev/dri/card0"
#define PLANE_ID 32
#define CRTC_ID 42

#define CHECK(x) do { if ((x) < 0) { perror(#x); exit(1); } } while (0)

int drm_fd;
uint32_t fb_id;
uint8_t* fb_data;
size_t fb_size;

int create_drm_fb() {
    struct drm_mode_create_dumb create = {0};
    create.width = WIDTH;
    create.height = HEIGHT;
    create.bpp = 32; // ARGB8888

    CHECK(drmIoctl(drm_fd, DRM_IOCTL_MODE_CREATE_DUMB, &create));

    struct drm_mode_map_dumb map = { .handle = create.handle };
    CHECK(drmIoctl(drm_fd, DRM_IOCTL_MODE_MAP_DUMB, &map));

    fb_size = create.size;
    fb_data = (uint8_t*)mmap(0, fb_size, PROT_WRITE, MAP_SHARED, drm_fd, map.offset);
    assert(fb_data != MAP_FAILED);

struct drm_mode_fb_cmd2 fb = {};
fb.width = WIDTH;
fb.height = HEIGHT;
fb.pixel_format = DRM_FORMAT_XRGB8888;
fb.handles[0] = create.handle;
fb.pitches[0] = create.pitch;
fb.offsets[0] = 0;

CHECK(drmIoctl(drm_fd, DRM_IOCTL_MODE_ADDFB2, &fb));

    fb_id = fb.fb_id;
    return 0;
}

GstElement *pipeline, *appsink;

static GstFlowReturn on_new_sample(GstAppSink *sink, gpointer user_data) {
    GstSample *sample = gst_app_sink_pull_sample(sink);
    if (!sample) return GST_FLOW_ERROR;

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    gst_buffer_map(buffer, &map, GST_MAP_READ);

    // Simple memcopy (format must match)
    memcpy(fb_data, map.data, WIDTH * HEIGHT * 4); // ARGB

    // Flip to screen
    drmModeSetPlane(drm_fd, PLANE_ID, CRTC_ID, fb_id,
                    0, 0, 0, WIDTH, HEIGHT,
                    0 << 16, 0 << 16, WIDTH << 16, HEIGHT << 16);

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

void on_pad_added(GstElement* src, GstPad* new_pad, gpointer user_data) {
    GstElement* sink_element = (GstElement*)user_data;
    GstPad* sink_pad = gst_element_get_static_pad(sink_element, "sink");

    if (gst_pad_is_linked(sink_pad)) {
        gst_object_unref(sink_pad);
        return;
    }

    if (gst_pad_link(new_pad, sink_pad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link demux pad to next element\n");
    }

    gst_object_unref(sink_pad);
}

void setup_gstreamer(int drm_fd) {
    gst_init(NULL, NULL);

    pipeline = gst_pipeline_new("video-pipeline");

    GstElement* src      = gst_element_factory_make("udpsrc", "src");
    GstElement* depay    = gst_element_factory_make("rtph264depay", "depay");
    GstElement* parser   = gst_element_factory_make("h264parse", "parser");
    GstElement* decoder  = gst_element_factory_make("v4l2slh264dec", "decoder");
    GstElement* convert  = gst_element_factory_make("videoconvert", "convert");
    GstElement* sink     = gst_element_factory_make("kmssink", "sink");

    assert(src && depay && parser && decoder && convert && sink);

    // UDP source settings
    g_object_set(src, "port", 5600, NULL);

    // Set caps for RTP H264 stream
    GstCaps* caps = gst_caps_new_simple("application/x-rtp",
                                        "media", G_TYPE_STRING, "video",
                                        "clock-rate", G_TYPE_INT, 90000,
                                        "encoding-name", G_TYPE_STRING, "H264",
                                        NULL);
    g_object_set(src, "caps", caps, NULL);
    gst_caps_unref(caps);

    // Configure KMS sink
    g_object_set(sink,
        "fd", drm_fd,
        "plane-id", 32,
        "plane-properties", "s,zpos=1",
        "force-modesetting", FALSE,
        "skip-vsync", TRUE,
        NULL);

    // Add elements to pipeline
    gst_bin_add_many(GST_BIN(pipeline), src, depay, parser, decoder, convert, sink, NULL);

    // Link elements
    CHECK(gst_element_link_many(src, depay, parser, decoder, convert, sink, NULL) == TRUE);
}


GMainLoop* loop = NULL;

int drm_video_main(int drm_fd) {
    g_setenv("GST_DEBUG", "*:3", TRUE);
    setup_gstreamer(drm_fd);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    return 0;
}
