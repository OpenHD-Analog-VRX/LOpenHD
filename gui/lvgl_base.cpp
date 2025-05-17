#include "lvgl_base.h"
#include "lv_conf.h"
#include "lvgl.h"

#include "lib/gui_settings.h"


#include "lib/driver_backends.h"

#include <chrono>
#include <thread>
#include <iostream>

extern simulator_settings_t settings;

bool is_wayland_available() {
    const char* wayland_display = getenv("WAYLAND_DISPLAY");
    return wayland_display != nullptr;
}

void lvgl_init() {
    lv_init();
    std::cout << "LVGL initialized\n";
    driver_backends_register();
    settings.window_width=800;
    settings.window_height=600;
    std::cout << "Backends registered\n";
    if (is_wayland_available()) {
        std::cout << "Using Wayland backend\n";
        driver_backends_init_backend("WAYLAND");
    } else {
        std::cout << "Using Drm backend\n";
        driver_backends_init_backend("DRM");
        driver_backends_init_backend("EVDEV");
    }
    std::cout << "Backend init result: " << "\n";

    std::cout << "Backend loop exited?\n";
}
