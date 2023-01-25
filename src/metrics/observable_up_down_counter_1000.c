#include "common/utils.h"

#include <stdio.h>
#include <time.h>

static inline void work() {
  // Set i as volatile to prevent compiler to do any optimisation
  for (volatile long i = 0; i < 100000000000; i++) {
  }
}

#ifndef TRACING_ENABLED

int main() {
  struct timespec start, end;
  printf("Starting program execution ...\n");
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  work();
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  printf("Program execution ends\n");
  struct timespec duration = timespec_diff(start, end);
  long long ns_duration = 1000000000LL * duration.tv_sec + duration.tv_nsec;
  printf("duration = %lld ns\n", ns_duration);
  return 0;
}

#elif OPENTELEMETRY_C_TRACING_ENABLED

#include <opentelemetry_c.h>

int64_t counter_callback() { return 0; }

int main() {
  struct timespec start, end;
  init_metrics_provider("opentelemetry-c-performance", "0.0.1", "",
                        "machine-0.0.1", 1000, 500);
  void *counter = create_int64_observable_up_down_counter("example-counter",
                                                          "description...");
  void *registration = int64_observable_up_down_counter_register_callback(
      counter, &counter_callback);

  printf("Starting program execution...\n");
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  work();
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  printf("Program execution ends\n");
  struct timespec duration = timespec_diff(start, end);
  long long counter_execution_duration = 1000000000LL * duration.tv_sec + duration.tv_nsec;
  printf("duration = %lld ns\n", counter_execution_duration);

  int64_observable_up_down_counter_cancel_registration(counter, registration);
  destroy_observable_up_down_counter(counter);
}

#else

#include <pthread.h>

#include "common/telemetry_data_tracepoint.h"

void log_telemetry_data() {
  char telemetry_data[] =
      "resource {\n"
      "  attributes {\n"
      "  key: \"telemetry.sdk.version\"\n"
      "        value {\n"
      "    string_value: \"1.8.1\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "  key: \"telemetry.sdk.language\"\n"
      "        value {\n"
      "    string_value: \"cpp\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "  key: \"telemetry.sdk.name\"\n"
      "        value {\n"
      "    string_value: \"opentelemetry\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "  key: \"service.instance.id\"\n"
      "        value {\n"
      "    string_value: \"machine-0.0.1\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "  key: \"service.namespace\"\n"
      "        value {\n"
      "    string_value: \"\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "  key: \"service.version\"\n"
      "        value {\n"
      "    string_value: \"0.0.1\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "  key: \"service.name\"\n"
      "        value {\n"
      "    string_value: \"opentelemetry-c-performance\"\n"
      "    }\n"
      "  }\n"
      "}\n"
      "scope_metrics {\n"
      "  scope {\n"
      "    name: \"example-counter\"\n"
      "    version: \"1.2.0\"\n"
      "  }\n"
      "  metrics {\n"
      "  name: \"example-counter\"\n"
      "        description: \"description...\"\n"
      "                      sum {\n"
      "      data_points {\n"
      "        start_time_unix_nano: 1674506754236827164\n"
      "        time_unix_nano: 1674506754736967576\n"
      "        as_int: 0\n"
      "      }\n"
      "      aggregation_temporality: AGGREGATION_TEMPORALITY_CUMULATIVE\n"
      "                                    is_monotonic: true\n"
      "    }\n"
      "  }\n"
      "}";
  lttng_ust_tracepoint(opentelemetry_c_performance, telemetry_data,
                       telemetry_data);
}

void *counter_routine(void *arg) {
  int *thread_running = (int *)arg;
  struct timespec ts;
  ts.tv_sec = 1;
  ts.tv_nsec = 0;
  while (*thread_running) {
    log_telemetry_data();
    nanosleep(&ts, NULL);
  }
  log_telemetry_data();
  return NULL;
}

int main() {
  struct timespec start, end;

  int thread_running = 1;
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, counter_routine, &thread_running);

  printf("Starting program execution...\n");
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  work();
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  printf("Program execution ends\n");
  struct timespec duration = timespec_diff(start, end);
  long long counter_execution_duration = 1000000000LL * duration.tv_sec + duration.tv_nsec;
  printf("duration = %lld ns\n", counter_execution_duration);

  thread_running = 0;
  pthread_join(thread_id, NULL);
  return 0;
}

#endif
