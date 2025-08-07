#pragma once

#include "telemetry.h"
#include "fc_telemetry.h"
#include "telemetry_state.h"

#include <thread>
#include <mutex>
#include <atomic>

#define OHD_SYS_ID_BETAFLIGHT 0
#define OHD_SYS_ID_GROUND 100
#define OHD_SYS_ID_AIR 101

class Mavlinkudp {
public:
    Mavlinkudp(TelemetryState& state);
    void start();
    void stop();
    void receive_loop();

private:
    int udp_socket = -1;
    std::thread receive_thread;
    std::atomic<bool> running = false;
    std::mutex socket_mutex;

    static constexpr int UDP_PORT = 14550;
    TelemetryState& telemetry_state;
    std::unique_ptr<GroundTelemetry> gnd_telem;
    std::unique_ptr<FCTelemetry> fc_telem;
};
