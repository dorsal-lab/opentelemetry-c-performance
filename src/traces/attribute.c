#include "utils.h"

#include <opentelemetry_c.h>

#include <stdlib.h>
#include <time.h>

#define N_SPANS_TO_CREATE 100000

int main() {
  init_tracer_provider("opentelemetry-c-performance", "0.0.1", "",
                       "machine-0.0.1");

  void *tracer = get_tracer();

  long nano_durations[N_SPANS_TO_CREATE];
  struct timespec start, end;
  for (int i = 0; i < N_SPANS_TO_CREATE; i++) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    void *span = start_span(tracer, "span", SPAN_KIND_INTERNAL, "");
    void *map = create_attr_map();
    set_int64_t_attr(map, "key", 0);
    set_span_attrs(span, map);
    destroy_attr_map(map);
    end_span(span);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    nano_durations[i] = timespec_diff(start, end).tv_nsec / 1000;
  }

  struct array_stats_t stats;
  compute_array_stats(nano_durations, N_SPANS_TO_CREATE, &stats);
  print_array_stats(&stats, "us");

  return 0;
}
