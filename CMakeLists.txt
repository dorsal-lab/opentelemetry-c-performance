cmake_minimum_required(VERSION 3.15)

project(opentelemetry-c-performance)

set(CMAKE_C_STANDARD 99)

option(TRACING_ENABLED "Whether to enable tracing or not" ON)
if (TRACING_ENABLED)
    option(OPENTELEMETRY_C_TRACING_ENABLED "Whether to use OpenTelemetry C tracing or not. If set to off, some traces data will be logged to LTTng" ON)
    option(GENERATE_HIGH_NUMBER_OF_TRACES "If set off, 5000 traces will be generated. Otherwise 20000 will be generated" ON)
    if (OPENTELEMETRY_C_TRACING_ENABLED)
        option(LTTNG_EXPORTER_ENABLED "Whether to use LTTng exporter or not. If set to off, OTLP GRPC default exporter will be used" ON)
        option(BATCH_SPAN_PROCESSOR_ENABLED "Whether to use Batching Span Processor or Simple Span Processor" OFF)
        add_subdirectory(third-party/opentelemetry-c)
        if (BATCH_SPAN_PROCESSOR_ENABLED)
            # Set a big number for span processor queue size to avoid spans to be dropped
            target_compile_definitions(opentelemetry-c PRIVATE BATCH_SPAN_PROCESSOR_MAX_QUEUE_SIZE=20000)
        endif ()
    endif ()
endif ()

function(add_traces_benchmark_executable executable_name executable_src)
    add_executable("${executable_name}")
    target_include_directories("${executable_name}" PRIVATE include)
    target_link_libraries("${executable_name}" PRIVATE m)
    if (TRACING_ENABLED)
        target_compile_definitions("${executable_name}" PRIVATE TRACING_ENABLED)
        if (OPENTELEMETRY_C_TRACING_ENABLED)
            target_link_libraries("${executable_name}" PRIVATE opentelemetry-c)
            target_compile_definitions("${executable_name}" PRIVATE OPENTELEMETRY_C_TRACING_ENABLED)
        else ()
            target_link_libraries("${executable_name}" PRIVATE lttng-ust dl)
            target_sources("${executable_name}" PRIVATE src/common/telemetry_data_tracepoint.c)
        endif ()
        if (GENERATE_HIGH_NUMBER_OF_TRACES)
            target_compile_definitions("${executable_name}" PRIVATE N_SPANS_TO_CREATE=20000)
        else()
            target_compile_definitions("${executable_name}" PRIVATE N_SPANS_TO_CREATE=5000)
        endif()
    endif ()
    target_sources("${executable_name}" PRIVATE
            "${executable_src}"
            src/common/utils.c
            )
endfunction()

function(add_metrics_benchmark_executable executable_name executable_src)
    add_executable("${executable_name}")
    target_include_directories("${executable_name}" PRIVATE include)
    target_link_libraries("${executable_name}" PRIVATE m)
    if (TRACING_ENABLED)
        target_compile_definitions("${executable_name}" PRIVATE TRACING_ENABLED)
        if (OPENTELEMETRY_C_TRACING_ENABLED)
            target_link_libraries("${executable_name}" PRIVATE opentelemetry-c)
            target_compile_definitions("${executable_name}" PRIVATE OPENTELEMETRY_C_TRACING_ENABLED)
        else ()
            target_link_libraries("${executable_name}" PRIVATE lttng-ust dl)
            target_sources("${executable_name}" PRIVATE src/common/telemetry_data_tracepoint.c)
        endif ()
    endif ()
    target_sources("${executable_name}" PRIVATE
            "${executable_src}"
            src/common/utils.c
            )
endfunction()

function(add_logs_benchmark_executable executable_name executable_src)
    add_executable("${executable_name}")
    target_include_directories("${executable_name}" PRIVATE include)
    target_link_libraries("${executable_name}" PRIVATE m)
    if (TRACING_ENABLED)
        target_compile_definitions("${executable_name}" PRIVATE TRACING_ENABLED)
        if (OPENTELEMETRY_C_TRACING_ENABLED)
            target_link_libraries("${executable_name}" PRIVATE opentelemetry-c)
            target_compile_definitions("${executable_name}" PRIVATE OPENTELEMETRY_C_TRACING_ENABLED)
        else ()
            target_link_libraries("${executable_name}" PRIVATE lttng-ust dl)
            target_sources("${executable_name}" PRIVATE src/common/telemetry_data_tracepoint.c)
        endif ()
        target_compile_definitions("${executable_name}" PRIVATE N_LOGS_TO_CREATE=5000)
    endif ()
    target_sources("${executable_name}" PRIVATE
            "${executable_src}"
            src/common/utils.c
    )
endfunction()

# benchmark-traces-simple
add_traces_benchmark_executable(benchmark-traces-simple src/traces/simple.c)

# benchmark-traces-context-extraction
add_traces_benchmark_executable(benchmark-traces-context-extraction src/traces/context_extraction.c)

# benchmark-traces-event
add_traces_benchmark_executable(benchmark-traces-event src/traces/event.c)

# benchmark-traces-attribute
add_traces_benchmark_executable(benchmark-traces-attribute src/traces/attribute.c)

# benchmark-traces-span-context
add_traces_benchmark_executable(benchmark-traces-span-context src/traces/span_context.c)

# benchmark-traces-nested-span
add_traces_benchmark_executable(benchmark-traces-nested-span src/traces/nested_span.c)

# benchmark-metrics-observable-up-down-counter-500
add_metrics_benchmark_executable(benchmark-metrics-observable-up-down-counter-500 src/metrics/observable_up_down_counter_500.c)

# benchmark-metrics-observable-up-down-counter-1000
add_metrics_benchmark_executable(benchmark-metrics-observable-up-down-counter-1000 src/metrics/observable_up_down_counter_1000.c)

# benchmark-logs-simple
add_logs_benchmark_executable(benchmark-logs-simple src/logs/simple.c)