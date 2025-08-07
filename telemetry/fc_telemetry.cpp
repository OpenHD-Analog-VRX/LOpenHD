#include "fc_telemetry.h"
#include "mavlink_udp.h"
#include <cstdio>
#include <cstring>
#include "mavlink-headers/mavlink/v2.0/common/mavlink.h"

FCTelemetry::FCTelemetry(TelemetryState& state)
    : MavlinkHandler(state)
{
}

bool FCTelemetry::handle_message(const mavlink_message_t& msg) {
    bool handled = false;

    switch (msg.msgid) {
        case MAVLINK_MSG_ID_SYS_STATUS: {
            mavlink_sys_status_t parsedMsg;
            mavlink_msg_sys_status_decode(&msg, &parsedMsg);
            telemetry_state.set_fc_battery_remaining(parsedMsg.battery_remaining);
            telemetry_state.set_fc_current(parsedMsg.current_battery / 100.0);
            telemetry_state.set_fc_voltage(parsedMsg.voltage_battery / 1000.0);
            handled = true;
            break;
        }
        case MAVLINK_MSG_ID_ATTITUDE: {
            mavlink_attitude_t parsedMsg;
            mavlink_msg_attitude_decode(&msg, &parsedMsg);
            telemetry_state.set_fc_roll(parsedMsg.roll);
            telemetry_state.set_fc_pitch(parsedMsg.pitch);
            telemetry_state.set_fc_yaw(parsedMsg.yaw);
            handled = true;
            break;
        }
        case MAVLINK_MSG_ID_RC_CHANNELS_RAW: {
            mavlink_rc_channels_raw_t parsedMsg;
            mavlink_msg_rc_channels_raw_decode(&msg, &parsedMsg);
            handled = true;
            break;
        }
        default:
            // Uncomment this if you want to log everything
            printf("[fc] Unhandled message ID: %d\n", msg.msgid);
            break;
    }
    return handled;
}
