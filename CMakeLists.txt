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

# benchmark-traces-attribute

# benchmark-traces-span-context

# benchmark-traces-nested-span
