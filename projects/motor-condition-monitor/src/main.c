#include <stdio.h>

#include "condition_monitor.h"
#include "fault_classifier.h"

typedef struct {
    const char *label;
    const int16_t *pattern;
    uint16_t current_ma;
    int16_t temp_deci_c;
} phase_profile_t;

static const int16_t k_healthy_pattern[MCM_WINDOW_SIZE] = {
    0,   96,  177, 231, 250, 231, 177, 96,
    0,  -96, -177, -231, -250, -231, -177, -96,
};

static const int16_t k_imbalance_pattern[MCM_WINDOW_SIZE] = {
    0,   344, 636, 831, 900, 831, 636, 344,
    0,  -344, -636, -831, -900, -831, -636, -344,
};

static const int16_t k_bearing_pattern[MCM_WINDOW_SIZE] = {
    0,    320,  -260, 820,  -910, 270,  1110, -1000,
    330, -380, 960,  -870, 220,  1200, -1100, 410,
};

static const int16_t k_stall_pattern[MCM_WINDOW_SIZE] = {
    20,  40,  55,  60,  30,  10,  -20, -40,
    -60, -45, -25, 0,   15,  35,  50,  60,
};

static const int16_t k_overtemp_pattern[MCM_WINDOW_SIZE] = {
    0,   110, 190, 235, 260, 235, 190, 110,
    0,  -110, -190, -235, -260, -235, -190, -110,
};

static void print_phase_summary(const char *label, const mcm_analysis_t *analysis) {
    float temp_c = (float)analysis->features.max_temp_deci_c / 10.0f;

    printf("phase=%s fault=%s sev=%s rms=%u p2p=%u jerk=%u current=%u temp=%.1fC "
           "events=%zu\n",
           label, mcm_fault_name(analysis->active_fault),
           mcm_severity_name(mcm_fault_severity(analysis->active_fault)),
           analysis->features.rms_mg, analysis->features.peak_to_peak_mg,
           analysis->features.mean_abs_jerk_mg, analysis->features.avg_current_ma,
           temp_c, analysis->logged_events);
}

static void run_phase(mcm_monitor_t *monitor, const phase_profile_t *phase,
                      uint32_t *timestamp_ms) {
    mcm_analysis_t analysis = {0};

    for (size_t i = 0; i < MCM_WINDOW_SIZE; ++i) {
        mcm_sample_t sample = {
            .vibration_mg = phase->pattern[i],
            .motor_current_ma = phase->current_ma,
            .board_temp_deci_c = phase->temp_deci_c,
            .timestamp_ms = *timestamp_ms,
        };

        analysis = mcm_monitor_push_sample(monitor, &sample);
        *timestamp_ms += 10u;
    }

    if (analysis.window_ready) {
        print_phase_summary(phase->label, &analysis);
    }
}

static void print_events(const mcm_monitor_t *monitor) {
    const mcm_event_log_t *log = mcm_monitor_event_log(monitor);

    for (size_t i = 0; i < log->count; ++i) {
        const mcm_event_t *event = mcm_event_log_get(log, i);
        printf("event[%zu] t=%u fault=%s sev=%s rms=%u current=%u temp=%.1fC\n", i,
               event->timestamp_ms, mcm_fault_name(event->fault),
               mcm_severity_name(event->severity), event->snapshot.rms_mg,
               event->snapshot.avg_current_ma,
               (float)event->snapshot.max_temp_deci_c / 10.0f);
    }
}

int main(void) {
    mcm_monitor_t monitor;
    uint32_t timestamp_ms = 0u;
    const phase_profile_t phases[] = {
        {"healthy", k_healthy_pattern, 4200u, 360},
        {"imbalance", k_imbalance_pattern, 5200u, 380},
        {"bearing", k_bearing_pattern, 5600u, 410},
        {"stall", k_stall_pattern, 9800u, 440},
        {"overtemp", k_overtemp_pattern, 4600u, 960},
    };

    mcm_monitor_init(&monitor);

    for (size_t i = 0; i < (sizeof(phases) / sizeof(phases[0])); ++i) {
        run_phase(&monitor, &phases[i], &timestamp_ms);
    }

    print_events(&monitor);
    return 0;
}

