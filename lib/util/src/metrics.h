#ifndef _METRICS_H_
#define _METRICS_H_

#define EXPONENTIAL_SMOOTHING_ALPHA 0.3

template <typename T>
static inline void exponentialSmoothing(
    T& metric,
    T current,
    double alpha = EXPONENTIAL_SMOOTHING_ALPHA) {
  metric = current * alpha + metric * (1 - alpha);
}

#endif
