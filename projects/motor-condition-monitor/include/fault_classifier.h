#ifndef FAULT_CLASSIFIER_H
#define FAULT_CLASSIFIER_H

#include "monitor_types.h"

mcm_fault_t mcm_classify_fault(const mcm_features_t *features);
mcm_severity_t mcm_fault_severity(mcm_fault_t fault);
const char *mcm_fault_name(mcm_fault_t fault);
const char *mcm_severity_name(mcm_severity_t severity);

#endif

