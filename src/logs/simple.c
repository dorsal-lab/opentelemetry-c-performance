#ifndef TRACING_ENABLED

#include <stdio.h>
int main() {
  printf("Nothing to do\n");
  return 0;
}

#elif OPENTELEMETRY_C_TRACING_ENABLED

#include "common/utils.h"

#include <opentelemetry_c/opentelemetry_c.h>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main() {
  otelc_init_logger_provider("opentelemetry-c-performance", "0.0.1", "",
                             "machine-0.0.1");

  void *logger = otelc_get_logger();

  long long *nano_durations = malloc(N_LOGS_TO_CREATE * sizeof(long long));
  struct timespec start, end;
  for (int i = 0; i < N_LOGS_TO_CREATE; i++) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    otelc_log(logger, OTEL_C_LOG_SEVERITY_KINFO, "Hello");
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    struct timespec duration = timespec_diff(start, end);
    nano_durations[i] = (1000000000LL * duration.tv_sec + duration.tv_nsec);
    usleep(10000);
  }

  struct array_stats_t stats;
  compute_array_stats(nano_durations, N_LOGS_TO_CREATE, &stats);
  print_array_stats(&stats, "ns");
  free(nano_durations);
  otelc_destroy_tracer(logger);

  return 0;
}

#else
#include "common/telemetry_data_tracepoint.h"
#include "common/utils.h"

#include <time.h>
#include <unistd.h>

void log_telemetry_data() {
  char telemetry_data[] =
      "resource {\n"
      "  attributes {\n"
      "    key: \"service.name\"\n"
      "    value {\n"
      "      string_value: \"unknown_service\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "    key: \"telemetry.sdk.version\"\n"
      "    value {\n"
      "      string_value: \"1.8.1\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "    key: \"telemetry.sdk.name\"\n"
      "    value {\n"
      "      string_value: \"opentelemetry\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "    key: \"telemetry.sdk.language\"\n"
      "    value {\n"
      "      string_value: \"cpp\"\n"
      "    }\n"
      "  }\n"
      "}\n"
      "scope_logs {\n"
      "  scope {\n"
      "    name: \"default\"\n"
      "    version: \"1.0.0\"\n"
      "  }\n"
      "  log_records {\n"
      "    time_unix_nano: 1698797332192121648\n"
      "    severity_number: SEVERITY_NUMBER_INFO\n"
      "    severity_text: \"INFO\"\n"
      "    body {\n"
      "      string_value: \"Hello\"\n"
      "    }\n"
      "  }\n"
      "  schema_url: \"https://opentelemetry.io/schemas/1.11.0\"\n"
      "}\n";
  lttng_ust_tracepoint(opentelemetry_c_performance, telemetry_data,
                       telemetry_data);
}

int main() {
  long long *nano_durations = malloc(N_LOGS_TO_CREATE * sizeof(long long));
  struct timespec start, end;
  for (int i = 0; i < N_LOGS_TO_CREATE; i++) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    log_telemetry_data();
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    struct timespec duration = timespec_diff(start, end);
    nano_durations[i] = (1000000000LL * duration.tv_sec + duration.tv_nsec);
    usleep(10000);
  }
  struct array_stats_t stats;
  compute_array_stats(nano_durations, N_LOGS_TO_CREATE, &stats);
  print_array_stats(&stats, "ns");
  free(nano_durations);

  return 0;
}

#endif
