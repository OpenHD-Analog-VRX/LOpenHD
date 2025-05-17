#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include "lvgl.h"
#include "../external/lvgl/src/drivers/display/drm/lv_linux_drm.h"
#include "../gui/lvgl_base.h"
#include "../gui/settings_overlay.h"
#include "../gui/lib/driver_backends.h"
#include "../video/drm_video.h"
#include "../telemetry/mavlink_udp.h"
#include "../telemetry/telemetry_state.h"
#include "osd_loader/osd_loader.h"

#include "../gui/osd/osd_manager.h"
#include "../gui/osd/soc_temp_widget.h"
#include <iostream>

#include <thread>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>

#define BUFFER_CNT 3

typedef struct {
    uint32_t handle;
    uint32_t pitch;
    uint32_t offset;
    unsigned long int size;
    uint8_t * map;
    uint32_t fb_handle;
} drm_buffer_t;

typedef struct {
    int fd;
    uint32_t conn_id, enc_id, crtc_id, plane_id, crtc_idx;
    uint32_t width, height;
    uint32_t mmWidth, mmHeight;
    uint32_t fourcc;
    drmModeModeInfo mode;
    uint32_t blob_id;
    drmModeCrtc * saved_crtc;
    drmModeAtomicReq * req;
    drmEventContext drm_event_ctx;
    drmModePlane * plane;
    drmModeCrtc * crtc;
    drmModeConnector * conn;
    uint32_t count_plane_props;
    uint32_t count_crtc_props;
    uint32_t count_conn_props;
    drmModePropertyPtr plane_props[128];
    drmModePropertyPtr crtc_props[128];
    drmModePropertyPtr conn_props[128];
    drm_buffer_t drm_bufs[BUFFER_CNT];
    drm_buffer_t * act_buf;
} drm_dev_t;

std::atomic<bool> osd_thread_running{true};

void osd_update_cb(lv_timer_t* timer) {
    OSDManager* osd = static_cast<OSDManager*>(lv_timer_get_user_data(timer));
    osd->update_all();
}


int main() {

    std::unique_ptr<TelemetryState> telemetry_state = std::make_unique<TelemetryState>();
    std::unique_ptr<Mavlinkudp> telemetry = std::make_unique<Mavlinkudp>(*telemetry_state);
    telemetry->start();

    lvgl_init();

    // For now we only support video on drm
    if(!is_wayland_available()) {
        drm_dev_t * drm_dev = (drm_dev_t *)lv_display_get_driver_data(lv_obj_get_disp(lv_scr_act()));
        std::thread gst_thread([drm_dev]() {
            sched_param sch_params;
            sch_params.sched_priority = 20;
            if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch_params)) {
                std::cerr << "Failed to set RT priority inside thread\n";
            } else {
                std::cout << "GStreamer thread RT priority set\n";
            }
            drm_video_main(drm_dev->fd);
        });

        gst_thread.detach();
        lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_TRANSP, LV_PART_MAIN);
    }

    OSDManager osd_manager;

  /*  auto air_temp = std::make_shared<SocTempWidget>(lv_scr_act(), true, telemetry_state.get());
    air_temp->set_position(10, 10);
    osd_manager.add_widget(air_temp);

    auto ground_temp = std::make_shared<SocTempWidget>(lv_scr_act(), false, telemetry_state.get());
    ground_temp->set_position(10, 30);
    osd_manager.add_widget(ground_temp);*/

    OSDLoader loader(osd_manager, telemetry_state.get(), lv_scr_act());
    loader.load_from_file("/etc/lopenhd/osd_config.json");

    lv_timer_t* timer = lv_timer_create(osd_update_cb, 500, &osd_manager);

  //  settings_overlay_create();
        driver_backends_run_loop();

    return 0;
}
