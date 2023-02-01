#ifndef TRACING_ENABLED

#include <stdio.h>
int main() {
  printf("Nothing to do\n");
  return 0;
}

#elif OPENTELEMETRY_C_TRACING_ENABLED

#include "common/utils.h"

#include <opentelemetry_c.h>

#include <stdlib.h>
#include <time.h>

int main() {
  init_tracer_provider("opentelemetry-c-performance", "0.0.1", "",
                       "machine-0.0.1");

  void *tracer = get_tracer();

  long long *nano_durations = malloc(N_SPANS_TO_CREATE * sizeof(long long));
  struct timespec start, end;
  for (int i = 0; i < N_SPANS_TO_CREATE; i++) {
    void *outer_span = start_span(tracer, "outer-span", SPAN_KIND_INTERNAL, "");
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    void *span = start_span(tracer, "span", SPAN_KIND_INTERNAL, "");
    end_span(span);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    struct timespec duration = timespec_diff(start, end);
    nano_durations[i] = (1000000000LL * duration.tv_sec + duration.tv_nsec);
    end_span(outer_span);
  }

  struct array_stats_t stats;
  compute_array_stats(nano_durations, N_SPANS_TO_CREATE, &stats);
  print_array_stats(&stats, "ns");
  free(nano_durations);

  return 0;
}

#else
#include "common/telemetry_data_tracepoint.h"
#include "common/utils.h"

#include <time.h>

void log_telemetry_data() {
  char telemetry_data[] =
      "resource {\n"
      "  attributes {\n"
      "    key: \"telemetry.sdk.version\"\n"
      "    value {\n"
      "      string_value: \"1.8.1\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "    key: \"telemetry.sdk.language\"\n"
      "    value {\n"
      "      string_value: \"cpp\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "    key: \"telemetry.sdk.name\"\n"
      "    value {\n"
      "      string_value: \"opentelemetry\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "    key: \"service.instance.id\"\n"
      "    value {\n"
      "      string_value: \"machine-0.0.1\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "    key: \"service.namespace\"\n"
      "    value {\n"
      "      string_value: \"\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "    key: \"service.version\"\n"
      "    value {\n"
      "      string_value: \"0.0.1\"\n"
      "    }\n"
      "  }\n"
      "  attributes {\n"
      "    key: \"service.name\"\n"
      "    value {\n"
      "      string_value: \"opentelemetry-c-performance\"\n"
      "    }\n"
      "  }\n"
      "}\n"
      "scope_spans {\n"
      "  scope {\n"
      "    name: \"opentelemetry-c\"\n"
      "    version: \"1.8.1\"\n"
      "  }\n"
      "  spans {\n"
      "    trace_id: "
      "\"\\341\\352\\235\\000\\311\\203\\314A\\355\\337\\372\\333j\\027\\214\\0"
      "26"
      "\"\n"
      "    span_id: \"J\\232\\017V\\3567?1\"\n"
      "    parent_span_id: \"\\304\\253Z\\204\\010\\257~\\257\"\n"
      "    name: \"span\"\n"
      "    kind: SPAN_KIND_INTERNAL\n"
      "    start_time_unix_nano: 1674509109471406947\n"
      "    end_time_unix_nano: 1674509109471410827\n"
      "  }\n"
      "}";
  lttng_ust_tracepoint(opentelemetry_c_performance, telemetry_data,
                       telemetry_data);
}

int main() {
  long long *nano_durations = malloc(N_SPANS_TO_CREATE * sizeof(long long));
  struct timespec start, end;
  for (int i = 0; i < N_SPANS_TO_CREATE; i++) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    log_telemetry_data();
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    struct timespec duration = timespec_diff(start, end);
    nano_durations[i] = (1000000000LL * duration.tv_sec + duration.tv_nsec);
  }
  struct array_stats_t stats;
  compute_array_stats(nano_durations, N_SPANS_TO_CREATE, &stats);
  print_array_stats(&stats, "ns");
  free(nano_durations);
  return 0;
}

#endif
