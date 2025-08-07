#pragma once

#include "mavlink_handler.h"
#include "telemetry_state.h"

class FCTelemetry : public MavlinkHandler {
public:
    FCTelemetry(TelemetryState& state);
    ~FCTelemetry() override = default;

    bool handle_message(const mavlink_message_t& msg) override;
};
