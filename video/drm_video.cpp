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

#define DRM_DEVICE "/dev/dri/card0"
#define PLANE_ID 32
#define CRTC_ID 42

#define CHECK(x) do { if ((x) < 0) { perror(#x); exit(1); } } while (0)

int drm_fd;
uint32_t fb_id;
uint8_t* fb_data;
size_t fb_size;


GstElement *pipeline, *appsink;

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
    GstElement* sink     = gst_element_factory_make("kmssink", "sink");

    assert(src && depay && parser && decoder && sink);

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
    g_object_set(parser, "config-interval", 1, NULL);

    g_object_set(decoder, "qos", FALSE, NULL);

    // Configure KMS sink
    g_object_set(sink,
        "fd", drm_fd,
        "plane-id", 32,
        "plane-properties", "s,zpos=1",
        "force-modesetting", FALSE,
        "skip-vsync", TRUE,
	"sync", FALSE,
        NULL);

    // Add elements to pipeline
    gst_bin_add_many(GST_BIN(pipeline), src, depay, parser, decoder, sink, NULL);

    // Link elements
    CHECK(gst_element_link_many(src, depay, parser, decoder, sink, NULL) == TRUE);
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
