#include "osd_loader.h"
#include <fstream>
#include <cstdio>

#include "gui/osd/soc_temp_widget.h"
#include "gui/osd/text_widget.h"
#include "gui/osd/attitude_widget.h"
#include "gui/osd/battery_widget.h"

OSDLoader::OSDLoader(OSDManager& manager, TelemetryState* state, lv_obj_t* parent)
    : osd_manager(manager), telemetry_state(state), parent(parent) {}

bool OSDLoader::load_from_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        printf("Failed to open OSD config file: %s\n", path.c_str());
        return false;
    }

    nlohmann::json config;
    try {
        file >> config;
    } catch (const std::exception& e) {
        printf("Failed to parse OSD config: %s\n", e.what());
        return false;
    }

    for (const auto& j : config["widgets"]) {
        auto widget = create_widget(j);
        if (widget) {
            set_position(*widget, j);
            osd_manager.add_widget(widget);
        }
    }

    return true;
}

std::shared_ptr<OSDWidget> OSDLoader::create_widget(const nlohmann::json& j) {
    std::string type = j.value("type", "");

    if (type == "soc_temp") {
        auto widget = std::make_shared<SocTempWidget>(
            parent,
            j.value("is_air", false),
            telemetry_state
        );

        widget->set_text_prefix(j.value("prefix", ""));
        widget->set_show_unit(j.value("show_unit", true));

        if (j.contains("icon")) widget->set_icon_src(j["icon"]);
        if (j.contains("color")) widget->set_color(hex_to_color(j["color"]));
        return widget;
    } else if (type == "text") {
        auto widget = std::make_shared<TextWidget>(
            parent,
            j.value("text", "TEXT")
        );

        if (j.contains("color")) widget->set_color(hex_to_color(j["color"]));

        return widget;
    } else if (type == "attitude") {
        int size = j.value("size", 120);
        auto widget = std::make_shared<AttitudeWidget>(parent, telemetry_state, size);

        if (j.contains("color"))
            widget->set_color(hex_to_color(j["color"]));

        return widget;
    } else if (type == "battery") {
        auto widget = std::make_shared<BatteryWidget>(
            parent,
            j.value("is_air", false),
            telemetry_state
        );

        if (j.contains("icon")) widget->set_icon_src(j["icon"]);
        if (j.contains("color")) widget->set_color(hex_to_color(j["color"]));
        return widget;
    }

    printf("Unknown widget type: %s\n", type.c_str());
    return nullptr;
}

void OSDLoader::set_position(OSDWidget& widget, const nlohmann::json& j) {
    int norm_x = j.value("x", 0);
    int norm_y = j.value("y", 0);

    widget.set_position(norm_x, norm_y);
}

lv_color_t OSDLoader::hex_to_color(const std::string& hex) {
    unsigned int r, g, b;
    sscanf(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
    return lv_color_make(r, g, b);
}
