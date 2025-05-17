#pragma once
#include "telemetry_state.h"
#include "mavlink-headers/mavlink/v2.0/common/mavlink.h"

class MavlinkHandler {
public:
    explicit MavlinkHandler(TelemetryState& state);
    virtual bool handle_message(const mavlink_message_t& msg) = 0;
    virtual ~MavlinkHandler() = default;

protected:
    TelemetryState& telemetry_state;  // Protected so derived classes can access
};
