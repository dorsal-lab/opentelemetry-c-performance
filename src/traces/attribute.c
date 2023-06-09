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
  otelc_init_tracer_provider("opentelemetry-c-performance", "0.0.1", "",
                       "machine-0.0.1");

  void *tracer = otelc_get_tracer();

  long long *nano_durations = malloc(N_SPANS_TO_CREATE * sizeof(long long));
  struct timespec start, end;
  for (int i = 0; i < N_SPANS_TO_CREATE; i++) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    void *span = otelc_start_span(tracer, "span", OTELC_SPAN_KIND_INTERNAL, "");
    void *map = otelc_create_attr_map();
    otelc_set_int64_t_attr(map, "key", 0);
    otelc_set_span_attrs(span, map);
    otelc_destroy_attr_map(map);
    otelc_end_span(span);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    struct timespec duration = timespec_diff(start, end);
    nano_durations[i] = (1000000000LL * duration.tv_sec + duration.tv_nsec);
    usleep(10000);
  }

  struct array_stats_t stats;
  compute_array_stats(nano_durations, N_SPANS_TO_CREATE, &stats);
  print_array_stats(&stats, "ns");
  free(nano_durations);
  otelc_destroy_tracer(tracer);

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
      "\"\\027\\000\\270\\372\\254\\307\\201JT\\371\\017\\003"
      "\\307\\234\\351\\302\"\n"
      "    span_id: \"\\326-\\250#\\033\\003\\n\\225\"\n"
      "    name: \"span\"\n"
      "    kind: SPAN_KIND_INTERNAL\n"
      "    start_time_unix_nano: 1674508829463496695\n"
      "    end_time_unix_nano: 1674508829463505481\n"
      "    attributes {\n"
      "      key: \"key\"\n"
      "      value {\n"
      "        int_value: 0\n"
      "      }\n"
      "    }\n"
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
    usleep(10000);
  }
  struct array_stats_t stats;
  compute_array_stats(nano_durations, N_SPANS_TO_CREATE, &stats);
  print_array_stats(&stats, "ns");
  free(nano_durations);
  return 0;
}

#endif
