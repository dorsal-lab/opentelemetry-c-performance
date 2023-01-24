#undef LTTNG_UST_TRACEPOINT_PROVIDER
#define LTTNG_UST_TRACEPOINT_PROVIDER opentelemetry_c_performance

#undef LTTNG_UST_TRACEPOINT_INCLUDE
#define LTTNG_UST_TRACEPOINT_INCLUDE "./common/telemetry_data_tracepoint.h"

#if !defined(_TELEMETRY_DATA_TRACEPOINT_H) ||                                  \
    defined(LTTNG_UST_TRACEPOINT_HEADER_MULTI_READ)
#define _TELEMETRY_DATA_TRACEPOINT_H

#include <lttng/tracepoint.h>

/*
 * Use LTTNG_UST_TRACEPOINT_EVENT(), LTTNG_UST_TRACEPOINT_EVENT_CLASS(),
 * LTTNG_UST_TRACEPOINT_EVENT_INSTANCE(), and
 * LTTNG_UST_TRACEPOINT_LOGLEVEL() here.
 */
LTTNG_UST_TRACEPOINT_EVENT(
    /* Tracepoint provider name */
    opentelemetry_c_performance,
    /* Tracepoint class name */
    telemetry_data,
    /* Input arguments */
    LTTNG_UST_TP_ARGS(const char *, telemetry_data),
    /* Output event fields */
    LTTNG_UST_TP_FIELDS(lttng_ust_field_string(telemetry_data, telemetry_data)))

#endif /* _TELEMETRY_DATA_TRACEPOINT_H */

#include <lttng/tracepoint-event.h>
