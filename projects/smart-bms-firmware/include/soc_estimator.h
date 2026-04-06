#ifndef SOC_ESTIMATOR_H
#define SOC_ESTIMATOR_H

typedef struct {
    float capacity_ah;
    float soc_percent;
} soc_estimator_t;

void soc_estimator_init(soc_estimator_t *estimator, float capacity_ah,
                        float initial_soc_percent);
float soc_estimator_update(soc_estimator_t *estimator, float pack_current_a,
                           float dt_seconds);

#endif

