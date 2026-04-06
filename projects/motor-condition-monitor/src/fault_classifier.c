#include "fault_classifier.h"

mcm_fault_t mcm_classify_fault(const mcm_features_t *features) {
    if (features->max_temp_deci_c >= 900) {
        return MCM_FAULT_OVERTEMP;
    }

    if (features->avg_current_ma >= 9000u && features->rms_mg <= 220u &&
        features->peak_to_peak_mg <= 600u) {
        return MCM_FAULT_STALL;
    }

    if (features->peak_to_peak_mg >= 2200u &&
        features->mean_abs_jerk_mg >= 450u) {
        return MCM_FAULT_BEARING_WEAR;
    }

    if (features->rms_mg >= 600u && features->mean_abs_jerk_mg <= 300u) {
        return MCM_FAULT_IMBALANCE;
    }

    return MCM_FAULT_HEALTHY;
}

mcm_severity_t mcm_fault_severity(mcm_fault_t fault) {
    switch (fault) {
        case MCM_FAULT_HEALTHY:
            return MCM_SEVERITY_INFO;
        case MCM_FAULT_IMBALANCE:
        case MCM_FAULT_BEARING_WEAR:
            return MCM_SEVERITY_WARNING;
        case MCM_FAULT_STALL:
        case MCM_FAULT_OVERTEMP:
            return MCM_SEVERITY_CRITICAL;
        default:
            return MCM_SEVERITY_WARNING;
    }
}

const char *mcm_fault_name(mcm_fault_t fault) {
    switch (fault) {
        case MCM_FAULT_HEALTHY:
            return "HEALTHY";
        case MCM_FAULT_IMBALANCE:
            return "IMBALANCE";
        case MCM_FAULT_BEARING_WEAR:
            return "BEARING_WEAR";
        case MCM_FAULT_STALL:
            return "STALL";
        case MCM_FAULT_OVERTEMP:
            return "OVERTEMP";
        default:
            return "UNKNOWN";
    }
}

const char *mcm_severity_name(mcm_severity_t severity) {
    switch (severity) {
        case MCM_SEVERITY_INFO:
            return "INFO";
        case MCM_SEVERITY_WARNING:
            return "WARNING";
        case MCM_SEVERITY_CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

