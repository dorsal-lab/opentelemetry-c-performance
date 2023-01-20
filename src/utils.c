#include "utils.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct timespec timespec_diff(struct timespec start, struct timespec end) {
  struct timespec temp;
  if ((end.tv_nsec - start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }
  return temp;
}

int cmp(void const *lhs, void const *rhs) {
  const long diff = *((const long *)lhs) - *((const long *)rhs);
  return diff == 0 ? 0 : (diff < 0 ? -1 : 1);
}

long *long_dup(const long *source, const size_t len) {
  long *p = malloc(len * sizeof(long));

  if (p == NULL)
    exit(1);

  memcpy(p, source, len * sizeof(long));
  return p;
}

long find_smallest(const long *array, const size_t len) {
  long smallest = array[0];

  size_t i;
  for (i = 1; i < len; ++i) {
    if (array[i] < smallest)
      smallest = array[i];
  }

  return smallest;
}

long find_largest(const long *array, const size_t len) {
  long largest = array[0];

  size_t i;
  for (i = 1; i < len; ++i) {
    if (array[i] > largest)
      largest = array[i];
  }

  return largest;
}

double compute_median(const long *array, const size_t len) {
  long *calc_array = long_dup(array, len);

  qsort(calc_array, len, sizeof(long), cmp);

  if (len % 2 == 0) { // is even
    // return the arithmetic middle of the two middle values
    return (array[(len - 1) / 2] + array[len / 2]) / 2.0;
  } else { // is odd
    // return the middle
    return array[len / 2];
  }
}

double compute_mean(const long *array, const size_t len) {
  double mean = 0;

  size_t i;
  for (i = 0; i < len; ++i)
    mean += array[i];

  return mean / len;
}

double compute_variance(const long *array, const size_t len,
                        const double mean) {
  if (len < 1)
    return 0.0;
  double variance = 0.0;
  int i;
  for (i = 0; i < len; i++) {
    variance += (array[i] - mean) * (array[i] - mean);
  }
  return variance / len;
}

double compute_std(const long *array, const size_t len, const double mean) {
  return sqrt(compute_variance(array, len, mean));
}

void compute_array_stats(const long *array, const size_t len,
                         struct array_stats_t *stats) {
  assert(array);
  assert(len >= 3);
  stats->first = array[0];
  stats->second = array[1];
  stats->third = array[2];
  stats->min = find_smallest(array, len);
  stats->max = find_largest(array, len);
  stats->median = compute_median(array, len);
  stats->mean = compute_mean(array, len);
  stats->std = compute_std(array, len, stats->mean);
}

void print_array_stats(const struct array_stats_t *stats, const char *unit) {
  printf("first = %ld %s\n", stats->first, unit);
  printf("second = %ld %s\n", stats->second, unit);
  printf("third = %ld %s\n", stats->third, unit);
  printf("min = %ld %s\n", stats->min, unit);
  printf("max = %ld %s\n", stats->max, unit);
  printf("median = %lf %s\n", stats->median, unit);
  printf("mean = %lf %s\n", stats->mean, unit);
  printf("std = %lf %s\n", stats->std, unit);
}
