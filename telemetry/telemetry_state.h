#pragma once

#include <mutex>
#include <cstdint>

class TelemetryState {
public:
    struct SystemStats {
        int cpu_usage_percent = 0;
        int ram_usage_percent = 0;
        int total_ram_mb = 0;
        int soc_temp_c = 0;

        void set_cpu(int val) { cpu_usage_percent = val; }
        void set_ram(int val) { ram_usage_percent = val; }
        void set_total_ram(int val) { total_ram_mb = val; }
        void set_soc_temp(int val) { soc_temp_c = val; }
    };

    // Setters
    void set_ground_stats(const SystemStats& stats);
    void set_air_stats(const SystemStats& stats);
    void set_fc_sys_id(uint8_t sysid);
    void set_air_sys_id(uint8_t sysid);
    void set_ground_sys_id(uint8_t sysid);

    // Getters
    SystemStats get_ground_stats();
    SystemStats get_air_stats();
    uint8_t get_fc_sys_id();
    uint8_t get_air_sys_id();
    uint8_t get_ground_sys_id();

private:
    std::mutex mtx;

    SystemStats ground_stats;
    SystemStats air_stats;

    uint8_t fc_sys_id = 0;
    uint8_t air_sys_id = 0;
    uint8_t ground_sys_id = 0;
};
