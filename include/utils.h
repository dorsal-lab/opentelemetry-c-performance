#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stddef.h>

struct timespec timespec_diff(struct timespec start, struct timespec end);

struct array_stats_t {
  long min;
  long max;
  double median;
  double mean;
  double std;
};

void compute_array_stats(const long *array, const size_t len,
                         struct array_stats_t *stats);

void print_array_stats(const struct array_stats_t *stats, const char *unit);

#endif /* MATH_UTILS_H */
