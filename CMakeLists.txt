cmake_minimum_required(VERSION 3.15)

project(opentelemetry-c-performance)

set(CMAKE_C_STANDARD 99)

option(TRACING_ENABLED "Whether to enable tracing or not" ON)
if (TRACING_ENABLED)
    option(LTTNG_EXPORTER_ENABLED "Whether to use LTTng exporter or not. If set to off, OTLP GRPC default exporter will be used" ON)
    add_subdirectory(third-party/opentelemetry-c)
endif ()

# benchmark-traces-simple
add_executable(benchmark-traces-simple)
target_include_directories(benchmark-traces-simple PRIVATE include)
target_link_libraries(benchmark-traces-simple PRIVATE m)
if (TRACING_ENABLED)
    target_link_libraries(benchmark-traces-simple PRIVATE opentelemetry-c)
    target_compile_definitions(benchmark-traces-simple PRIVATE TRACING_ENABLED)
endif ()
target_sources(benchmark-traces-simple PRIVATE
        src/traces/simple.c
        src/utils.c
        )

# benchmark-traces-context-extraction
add_executable(benchmark-traces-context-extraction)
target_include_directories(benchmark-traces-context-extraction PRIVATE include)
target_link_libraries(benchmark-traces-context-extraction PRIVATE m)
if (TRACING_ENABLED)
    target_link_libraries(benchmark-traces-context-extraction PRIVATE opentelemetry-c)
    target_compile_definitions(benchmark-traces-context-extraction PRIVATE TRACING_ENABLED)
endif ()
target_sources(benchmark-traces-context-extraction PRIVATE
        src/traces/context_extraction.c
        src/utils.c
        )

# benchmark-traces-event
add_executable(benchmark-traces-event)
target_include_directories(benchmark-traces-event PRIVATE include)
target_link_libraries(benchmark-traces-event PRIVATE m)
if (TRACING_ENABLED)
    target_link_libraries(benchmark-traces-event PRIVATE opentelemetry-c)
    target_compile_definitions(benchmark-traces-event PRIVATE TRACING_ENABLED)
endif ()
target_sources(benchmark-traces-event PRIVATE
        src/traces/event.c
        src/utils.c
        )

# benchmark-traces-attribute
add_executable(benchmark-traces-attribute)
target_include_directories(benchmark-traces-attribute PRIVATE include)
target_link_libraries(benchmark-traces-attribute PRIVATE m)
if (TRACING_ENABLED)
    target_link_libraries(benchmark-traces-attribute PRIVATE opentelemetry-c)
    target_compile_definitions(benchmark-traces-attribute PRIVATE TRACING_ENABLED)
endif ()
target_sources(benchmark-traces-attribute PRIVATE
        src/traces/attribute.c
        src/utils.c
        )

# benchmark-traces-span-context
add_executable(benchmark-traces-span-context)
target_include_directories(benchmark-traces-span-context PRIVATE include)
target_link_libraries(benchmark-traces-span-context PRIVATE m)
if (TRACING_ENABLED)
    target_link_libraries(benchmark-traces-span-context PRIVATE opentelemetry-c)
    target_compile_definitions(benchmark-traces-span-context PRIVATE TRACING_ENABLED)
endif ()
target_sources(benchmark-traces-span-context PRIVATE
        src/traces/span_context.c
        src/utils.c
        )

# benchmark-traces-nested-span
add_executable(benchmark-traces-nested-span)
target_include_directories(benchmark-traces-nested-span PRIVATE include)
target_link_libraries(benchmark-traces-nested-span PRIVATE m)
if (TRACING_ENABLED)
    target_link_libraries(benchmark-traces-nested-span PRIVATE opentelemetry-c)
    target_compile_definitions(benchmark-traces-nested-span PRIVATE TRACING_ENABLED)
endif ()
target_sources(benchmark-traces-nested-span PRIVATE
        src/traces/nested_span.c
        src/utils.c
        )

# benchmark-metrics-observable-up-down-counter-500
add_executable(benchmark-metrics-observable-up-down-counter-500)
target_include_directories(benchmark-metrics-observable-up-down-counter-500 PRIVATE include)
target_link_libraries(benchmark-metrics-observable-up-down-counter-500 PRIVATE m)
if (TRACING_ENABLED)
    target_link_libraries(benchmark-metrics-observable-up-down-counter-500 PRIVATE opentelemetry-c)
    target_compile_definitions(benchmark-metrics-observable-up-down-counter-500 PRIVATE TRACING_ENABLED)
endif ()
target_sources(benchmark-metrics-observable-up-down-counter-500 PRIVATE
        src/metrics/observable_up_down_counter_500.c
        src/utils.c
        )

# benchmark-metrics-observable-up-down-counter-1000
add_executable(benchmark-metrics-observable-up-down-counter-1000)
target_include_directories(benchmark-metrics-observable-up-down-counter-1000 PRIVATE include)
target_link_libraries(benchmark-metrics-observable-up-down-counter-1000 PRIVATE m)
if (TRACING_ENABLED)
    target_link_libraries(benchmark-metrics-observable-up-down-counter-1000 PRIVATE opentelemetry-c)
    target_compile_definitions(benchmark-metrics-observable-up-down-counter-1000 PRIVATE TRACING_ENABLED)
endif ()
target_sources(benchmark-metrics-observable-up-down-counter-1000 PRIVATE
        src/metrics/observable_up_down_counter_1000.c
        src/utils.c
        )
