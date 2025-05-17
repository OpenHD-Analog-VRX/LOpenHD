#include "telemetry_state.h"

// Setters
void TelemetryState::set_ground_stats(const SystemStats& stats) {
    std::lock_guard<std::mutex> lock(mtx);
    ground_stats = stats;
}

void TelemetryState::set_air_stats(const SystemStats& stats) {
    std::lock_guard<std::mutex> lock(mtx);
    air_stats = stats;
}

void TelemetryState::set_fc_sys_id(uint8_t sysid) {
    std::lock_guard<std::mutex> lock(mtx);
    fc_sys_id = sysid;
}

void TelemetryState::set_air_sys_id(uint8_t sysid) {
    std::lock_guard<std::mutex> lock(mtx);
    air_sys_id = sysid;
}

void TelemetryState::set_ground_sys_id(uint8_t sysid) {
    std::lock_guard<std::mutex> lock(mtx);
    ground_sys_id = sysid;
}

// Getters
TelemetryState::SystemStats TelemetryState::get_ground_stats() {
    std::lock_guard<std::mutex> lock(mtx);
    return ground_stats;
}

TelemetryState::SystemStats TelemetryState::get_air_stats() {
    std::lock_guard<std::mutex> lock(mtx);
    return air_stats;
}

uint8_t TelemetryState::get_fc_sys_id() {
    std::lock_guard<std::mutex> lock(mtx);
    return fc_sys_id;
}

uint8_t TelemetryState::get_air_sys_id() {
    std::lock_guard<std::mutex> lock(mtx);
    return air_sys_id;
}

uint8_t TelemetryState::get_ground_sys_id() {
    std::lock_guard<std::mutex> lock(mtx);
    return ground_sys_id;
}
