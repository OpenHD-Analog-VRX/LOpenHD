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

        void set_cpu(int val)       { cpu_usage_percent = val; }
        void set_ram(int val)       { ram_usage_percent = val; }
        void set_total_ram(int val) { total_ram_mb = val; }
        void set_soc_temp(int val)  { soc_temp_c = val; }
    };

    struct FCStats {
        float roll_deg = 0.6f;
        float pitch_deg = 0.3f;
        float yaw_deg = 0.0f;

        float battery_voltage = 0.0f;
        float battery_current = 0.0f;
        int battery_remaining_pct = -1;

        void set_roll(float val)     { roll_deg = val; }
        void set_pitch(float val)    { pitch_deg = val; }
        void set_yaw(float val)      { yaw_deg = val; }

        void set_voltage(float val)  { battery_voltage = val; }
        void set_current(float val)  { battery_current = val; }
        void set_battery_remaining(int val) { battery_remaining_pct = val; }
    };

    // Setters
    void set_ground_stats(const SystemStats& stats);
    void set_air_stats(const SystemStats& stats);
    void set_fc_stats(const FCStats& stats);

    void set_fc_sys_id(uint8_t sysid);
    void set_air_sys_id(uint8_t sysid);
    void set_ground_sys_id(uint8_t sysid);

    void set_fc_roll(float roll);
    void set_fc_pitch(float pitch);
    void set_fc_yaw(float yaw);

    void set_fc_voltage(float voltage);
    void set_fc_current(float current);
    void set_fc_battery_remaining(int pct);

    // Getters
    SystemStats get_ground_stats();
    SystemStats get_air_stats();
    FCStats get_fc_stats();

    uint8_t get_fc_sys_id();
    uint8_t get_air_sys_id();
    uint8_t get_ground_sys_id();

private:
    std::mutex mtx;

    SystemStats ground_stats;
    SystemStats air_stats;
    FCStats fc_stats;

    uint8_t fc_sys_id = 0;
    uint8_t air_sys_id = 0;
    uint8_t ground_sys_id = 0;
};
