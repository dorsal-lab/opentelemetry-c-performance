#include "utils.h"

#ifdef TRACING_ENABLED
#include <opentelemetry_c.h>
#endif

#include <stdio.h>
#include <time.h>

#ifdef TRACING_ENABLED
int64_t counter_callback() { return 0; }
#endif

static inline void work() {
  // Set i as volatile to prevent compiler to do any optimisation
  // TODO : Increase j maximum value
  for (volatile long i = 0; i < 10000000000; i++) {
  }
}

int main() {
  struct timespec start, end;
  printf("Starting normal program execution ...\n");
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  work();
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  printf("Normal program execution ends\n");
  long normal_execution_duration = timespec_diff(start, end).tv_nsec;
  printf("normal = %ld ns\n", normal_execution_duration);

#ifdef TRACING_ENABLED
  init_metrics_provider("opentelemetry-c-performance", "0.0.1", "",
                        "machine-0.0.1", 500, 250);
  void *counter = create_int64_observable_up_down_counter("example-counter",
                                                          "description...");
  void *registration = int64_observable_up_down_counter_register_callback(
      counter, &counter_callback);

  printf("Starting program execution with counter...\n");
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  work();
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  printf("Program execution with counter ends\n");
  long counter_execution_duration = timespec_diff(start, end).tv_nsec;
  printf("with counter = %ld ns\n", counter_execution_duration);

  int64_observable_up_down_counter_cancel_registration(counter, registration);
  destroy_observable_up_down_counter(counter);

  long difference = counter_execution_duration - normal_execution_duration;
  printf("difference = %ld ns %ld us %ld ms\n", difference, difference / 1000,
         difference / 1000000);
#endif

  return 0;
}
