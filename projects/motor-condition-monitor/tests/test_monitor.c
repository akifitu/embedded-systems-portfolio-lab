#include <stdio.h>

#include "condition_monitor.h"
#include "fault_classifier.h"
#include "feature_extractor.h"

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assertion failed: %s (%s:%d)\n", #condition,      \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static void fill_window(mcm_sample_t *samples, const int16_t *pattern,
                        uint16_t current_ma, int16_t temp_deci_c) {
    for (size_t i = 0; i < MCM_WINDOW_SIZE; ++i) {
        samples[i].vibration_mg = pattern[i];
        samples[i].motor_current_ma = current_ma;
        samples[i].board_temp_deci_c = temp_deci_c;
        samples[i].timestamp_ms = (uint32_t)(i * 10u);
    }
}

static int test_feature_metrics(void) {
    static const int16_t pattern[MCM_WINDOW_SIZE] = {
        0,   96,  177, 231, 250, 231, 177, 96,
        0,  -96, -177, -231, -250, -231, -177, -96,
    };
    mcm_sample_t samples[MCM_WINDOW_SIZE];
    mcm_features_t features = {0};

    fill_window(samples, pattern, 4200u, 360);
    mcm_features_compute(samples, MCM_WINDOW_SIZE, &features);

    ASSERT_TRUE(features.rms_mg >= 170u && features.rms_mg <= 190u);
    ASSERT_TRUE(features.peak_to_peak_mg == 500u);
    ASSERT_TRUE(features.mean_abs_jerk_mg >= 60u && features.mean_abs_jerk_mg <= 80u);
    ASSERT_TRUE(features.avg_current_ma == 4200u);
    ASSERT_TRUE(features.max_temp_deci_c == 360);
    return 0;
}

static int test_imbalance_classification(void) {
    static const int16_t pattern[MCM_WINDOW_SIZE] = {
        0,   344, 636, 831, 900, 831, 636, 344,
        0,  -344, -636, -831, -900, -831, -636, -344,
    };
    mcm_sample_t samples[MCM_WINDOW_SIZE];
    mcm_features_t features = {0};

    fill_window(samples, pattern, 5200u, 380);
    mcm_features_compute(samples, MCM_WINDOW_SIZE, &features);

    ASSERT_TRUE(mcm_classify_fault(&features) == MCM_FAULT_IMBALANCE);
    return 0;
}

static int test_bearing_classification(void) {
    static const int16_t pattern[MCM_WINDOW_SIZE] = {
        0,    320,  -260, 820,  -910, 270,  1110, -1000,
        330, -380, 960,  -870, 220,  1200, -1100, 410,
    };
    mcm_sample_t samples[MCM_WINDOW_SIZE];
    mcm_features_t features = {0};

    fill_window(samples, pattern, 5600u, 410);
    mcm_features_compute(samples, MCM_WINDOW_SIZE, &features);

    ASSERT_TRUE(mcm_classify_fault(&features) == MCM_FAULT_BEARING_WEAR);
    return 0;
}

static int test_stall_and_overtemp_priority(void) {
    mcm_features_t stall = {
        .rms_mg = 45u,
        .peak_to_peak_mg = 120u,
        .mean_abs_jerk_mg = 42u,
        .avg_current_ma = 9800u,
        .max_temp_deci_c = 440,
    };
    mcm_features_t overtemp = stall;

    ASSERT_TRUE(mcm_classify_fault(&stall) == MCM_FAULT_STALL);
    overtemp.max_temp_deci_c = 950;
    ASSERT_TRUE(mcm_classify_fault(&overtemp) == MCM_FAULT_OVERTEMP);
    return 0;
}

static int test_event_log_transitions(void) {
    static const int16_t healthy[MCM_WINDOW_SIZE] = {
        0,   96,  177, 231, 250, 231, 177, 96,
        0,  -96, -177, -231, -250, -231, -177, -96,
    };
    static const int16_t imbalance[MCM_WINDOW_SIZE] = {
        0,   344, 636, 831, 900, 831, 636, 344,
        0,  -344, -636, -831, -900, -831, -636, -344,
    };
    mcm_monitor_t monitor;
    mcm_analysis_t analysis = {0};

    mcm_monitor_init(&monitor);

    for (size_t i = 0; i < MCM_WINDOW_SIZE; ++i) {
        mcm_sample_t sample = {
            .vibration_mg = healthy[i],
            .motor_current_ma = 4200u,
            .board_temp_deci_c = 360,
            .timestamp_ms = (uint32_t)(i * 10u),
        };
        analysis = mcm_monitor_push_sample(&monitor, &sample);
    }
    ASSERT_TRUE(analysis.active_fault == MCM_FAULT_HEALTHY);
    ASSERT_TRUE(analysis.logged_events == 0u);

    for (size_t i = 0; i < MCM_WINDOW_SIZE; ++i) {
        mcm_sample_t sample = {
            .vibration_mg = imbalance[i],
            .motor_current_ma = 5200u,
            .board_temp_deci_c = 380,
            .timestamp_ms = (uint32_t)(160u + i * 10u),
        };
        analysis = mcm_monitor_push_sample(&monitor, &sample);
    }

    ASSERT_TRUE(analysis.active_fault == MCM_FAULT_IMBALANCE);
    ASSERT_TRUE(analysis.logged_events >= 1u);
    ASSERT_TRUE(mcm_monitor_event_log(&monitor)->count >= 1u);
    ASSERT_TRUE(mcm_event_log_get(mcm_monitor_event_log(&monitor), 0u)->fault ==
                MCM_FAULT_IMBALANCE);
    return 0;
}

int main(void) {
    ASSERT_TRUE(test_feature_metrics() == 0);
    ASSERT_TRUE(test_imbalance_classification() == 0);
    ASSERT_TRUE(test_bearing_classification() == 0);
    ASSERT_TRUE(test_stall_and_overtemp_priority() == 0);
    ASSERT_TRUE(test_event_log_transitions() == 0);
    printf("motor-condition-monitor tests passed\n");
    return 0;
}
