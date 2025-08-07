#include <string>
#include <cstdio>
#include <cstring>

#include "mavlink_udp.h"

#include "telemetry.h"

#include "mavlink-headers/mavlink/v2.0/common/mavlink.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


Mavlinkudp::Mavlinkudp(TelemetryState& state)
    : telemetry_state(state) {
}

void Mavlinkudp::start() {
    struct sockaddr_in local_addr{};
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        perror("Failed to create socket");
        return;
    }

    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(UDP_PORT);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udp_socket, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("Bind failed");
        close(udp_socket);
        return;
    }

    running = true;
    receive_thread = std::thread(&Mavlinkudp::receive_loop, this);

    // Create handlers
    gnd_telem = std::make_unique<GroundTelemetry>(telemetry_state);
    fc_telem = std::make_unique<FCTelemetry>(telemetry_state);

    printf("MavlinkTelemetry: Started\n");
}

void Mavlinkudp::stop() {
    running = false;
    if (receive_thread.joinable()) receive_thread.join();
    std::lock_guard<std::mutex> lock(socket_mutex);
    if (udp_socket >= 0) {
        close(udp_socket);
        udp_socket = -1;
    }
    printf("MavlinkTelemetry: Stopped\n");
}

void Mavlinkudp::receive_loop() {
    uint8_t buf[2048];
    struct sockaddr_in sender{};
    socklen_t addrlen = sizeof(sender);

    while (running) {
        ssize_t len = recvfrom(udp_socket, buf, sizeof(buf), 0,
                               (struct sockaddr*)&sender, &addrlen);
        if (len > 0) {
            mavlink_message_t msg;
            mavlink_status_t status;

            for (ssize_t i = 0; i < len; ++i) {
                if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status)) {

                    if (msg.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
                        mavlink_heartbeat_t hb;
                        mavlink_msg_heartbeat_decode(&msg, &hb);
                    //    printf("Heartbeat: type=%d autopilot=%d\n", hb.type, hb.autopilot);
                    }

                    else if(msg.sysid == OHD_SYS_ID_GROUND || msg.sysid == OHD_SYS_ID_AIR) {
                        if(!gnd_telem->handle_message(msg)) {
                            printf("Mavlink msg: ID=%d sysid=%d compid=%d len=%d\n",
                                    msg.msgid, msg.sysid, msg.compid, msg.len);
                        }
                    } else if (msg.sysid == OHD_SYS_ID_BETAFLIGHT || msg.sysid == OHD_SYS_ID_INAV) {
                        if(!fc_telem->handle_message(msg)) {
                            printf("Mavlink msg: ID=%d sysid=%d compid=%d len=%d\n",
                                    msg.msgid, msg.sysid, msg.compid, msg.len);
                        }
                    } else {
                        printf("Mavlink msg: ID=%d sysid=%d compid=%d len=%d\n",
                                    msg.msgid, msg.sysid, msg.compid, msg.len);
                    }
                    // tbd: actually handle stuff
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
