#!/usr/bin/env bash
set -e

echo "Building all targets ..."
buildir_lttng_expoter_on=/tmp/buildir_lttng_expoter_on
mkdir -p "$buildir_lttng_expoter_on"
cmake -B "$buildir_lttng_expoter_on" -S . -D CMAKE_BUILD_TYPE=Release -D LTTNG_EXPORTER_ENABLED=ON
cmake --build "$buildir_lttng_expoter_on" --target all --
buildir_lttng_expoter_off=/tmp/buildir_lttng_expoter_off
mkdir -p "$buildir_lttng_expoter_off"
cmake -B "$buildir_lttng_expoter_off" -S . -D CMAKE_BUILD_TYPE=Release -D LTTNG_EXPORTER_ENABLED=OFF
cmake --build "$buildir_lttng_expoter_off" --target all --

all_executables=(
    "benchmark-traces-simple"
    "benchmark-traces-context-extraction"
    "benchmark-traces-event"
    "benchmark-traces-attribute"
    "benchmark-traces-span-context"
    "benchmark-traces-nested-span"
)
for executable in "${all_executables[@]}"; do
    # echo "########## executable=$executable LTTNG_EXPORTER_ENABLED=OFF without lttng all events disabled ##########"
    # "$buildir_lttng_expoter_off/$executable"

    echo "########## executable=$executable LTTNG_EXPORTER_ENABLED=ON without lttng ##########"
    "$buildir_lttng_expoter_on/$executable"

    echo "########## executable=$executable LTTNG_EXPORTER_ENABLED=ON with lttng and all events disabled ##########"
    echo "Starting a LTTng session ..."
    lttng create "--output=ctf-traces/$executable/lttng-export-disabled"
    lttng enable-channel --userspace default-channel
    lttng start
    echo "Starting $executable ..."
    "$buildir_lttng_expoter_on/$executable"
    echo "Stop LTTng session ..."
    lttng stop
    # echo "View traces ..."
    # lttng view | sed 's/\(.\{400\}\).*/\1.../'
    echo "Destroying LTTng session ..."
    lttng destroy

    echo "########## executable=$executable LTTNG_EXPORTER_ENABLED=ON with lttng and all events enabled ##########"
    echo "Starting a LTTng session ..."
    lttng create "--output=ctf-traces/$executable/lttng-export-enabled"
    lttng enable-event -u 'opentelemetry:*'
    lttng start
    echo "Starting $executable ..."
    "$buildir_lttng_expoter_on/$executable"
    echo "Stop LTTng session ..."
    lttng stop
    # echo "View traces ..."
    # lttng view | sed 's/\(.\{400\}\).*/\1.../'
    echo "Destroying LTTng session ..."
    lttng destroy
done

echo "Done!"
