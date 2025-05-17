#pragma once

#include "mavlink_handler.h"
#include "telemetry_state.h"

class GroundTelemetry : public MavlinkHandler {
public:
    GroundTelemetry(TelemetryState& state);
    ~GroundTelemetry() override = default;

    bool handle_message(const mavlink_message_t& msg) override;
};
