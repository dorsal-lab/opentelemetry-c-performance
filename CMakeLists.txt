cmake_minimum_required(VERSION 3.15)

project(opentelemetry-c-performance)

set(CMAKE_C_STANDARD 99)

set(BUILD_EXAMPLES OFF)
add_subdirectory(third-party/opentelemetry-c)

# benchmark-traces-simple
add_executable(benchmark-traces-simple)
target_include_directories(benchmark-traces-simple PRIVATE include)
target_link_libraries(benchmark-traces-simple PRIVATE opentelemetry-c)
target_sources(benchmark-traces-simple PRIVATE
    src/traces/simple.c
    src/utils.c
)

# benchmark-traces-context-extraction
add_executable(benchmark-traces-context-extraction)
target_include_directories(benchmark-traces-context-extraction PRIVATE include)
target_link_libraries(benchmark-traces-context-extraction PRIVATE opentelemetry-c)
target_sources(benchmark-traces-context-extraction PRIVATE
    src/traces/context-extraction.c
    src/utils.c
)

# benchmark-traces-event
add_executable(benchmark-traces-event)
target_include_directories(benchmark-traces-event PRIVATE include)
target_link_libraries(benchmark-traces-event PRIVATE opentelemetry-c)
target_sources(benchmark-traces-event PRIVATE
    src/traces/event.c
    src/utils.c
)

# benchmark-traces-attribute
add_executable(benchmark-traces-attribute)
target_include_directories(benchmark-traces-attribute PRIVATE include)
target_link_libraries(benchmark-traces-attribute PRIVATE opentelemetry-c)
target_sources(benchmark-traces-attribute PRIVATE
    src/traces/attribute.c
    src/utils.c
)

# benchmark-traces-span-context
add_executable(benchmark-traces-span-context)
target_include_directories(benchmark-traces-span-context PRIVATE include)
target_link_libraries(benchmark-traces-span-context PRIVATE opentelemetry-c)
target_sources(benchmark-traces-span-context PRIVATE
    src/traces/span-context.c
    src/utils.c
)

# benchmark-traces-nested-span
add_executable(benchmark-traces-nested-span)
target_include_directories(benchmark-traces-nested-span PRIVATE include)
target_link_libraries(benchmark-traces-nested-span PRIVATE opentelemetry-c)
target_sources(benchmark-traces-nested-span PRIVATE
    src/traces/nested-span.c
    src/utils.c
)
