#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>

#include "gui/osd/osd_manager.h"
#include "telemetry/telemetry_state.h"

class OSDLoader {
public:
    OSDLoader(OSDManager& manager, TelemetryState* state, lv_obj_t* parent);
    bool load_from_file(const std::string& path);

private:
    std::shared_ptr<OSDWidget> create_widget(const nlohmann::json& j);
    void set_position(OSDWidget& widget, const nlohmann::json& j);
    lv_color_t hex_to_color(const std::string& hex);

    OSDManager& osd_manager;
    TelemetryState* telemetry_state;
    lv_obj_t* parent;
};
