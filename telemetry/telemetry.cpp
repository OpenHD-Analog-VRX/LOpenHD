#include "telemetry.h"
#include "mavlink_udp.h"
#include <cstdio>
#include <cstring>
#include "mavlink-headers/mavlink/v2.0/common/mavlink.h"

GroundTelemetry::GroundTelemetry(TelemetryState& state)
    : MavlinkHandler(state)
{
}

bool GroundTelemetry::handle_message(const mavlink_message_t& msg) {
    bool handled = false;
    bool ground = false;
    if (msg.sysid == OHD_SYS_ID_GROUND)
        ground = true;

    switch (msg.msgid) {
        case MAVLINK_MSG_ID_ONBOARD_COMPUTER_STATUS: {
            mavlink_onboard_computer_status_t parsedMsg;
            mavlink_msg_onboard_computer_status_decode(&msg, &parsedMsg);
            TelemetryState::SystemStats stats = telemetry_state.get_ground_stats();
            stats.soc_temp_c = parsedMsg.temperature_core[0];
            if(ground)
                telemetry_state.set_ground_stats(stats);
            else
                telemetry_state.set_air_stats(stats);
            handled = true;
            break;
        }
        case MAVLINK_MSG_ID_STATUSTEXT: {
            mavlink_statustext_t stat;
            mavlink_msg_statustext_decode(&msg, &stat);
            stat.text[sizeof(stat.text) - 1] = '\0'; // Ensure null-termination
            printf("[GCS] StatusText: severity=%d, text=%s\n", stat.severity, stat.text);
            handled = true;
            break;
        }
        case MAVLINK_MSG_ID_COMMAND_ACK: {
            mavlink_command_ack_t ack;
            mavlink_msg_command_ack_decode(&msg, &ack);
            printf("[GCS] Command ACK: command=%d, result=%d\n", ack.command, ack.result);
            handled = true;
            break;
        }
        default:
            // Uncomment this if you want to log everything
            printf("[ground] Unhandled message ID: %d\n", msg.msgid);
            break;
    }
    return handled;
}
