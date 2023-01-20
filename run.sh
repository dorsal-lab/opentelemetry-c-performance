#!/usr/bin/env bash
set -e

echo "Building all targets ..."
build_dir_tracing_off=/tmp/build_dir_tracing_off
mkdir -p "$build_dir_tracing_off"
cmake -B "$build_dir_tracing_off" -S . -D CMAKE_BUILD_TYPE=Release -D TRACING_ENABLED=OFF
cmake --build "$build_dir_tracing_off" --target all --
build_dir_lttng_exporter_on=/tmp/build_dir_lttng_exporter_on
mkdir -p "$build_dir_lttng_exporter_on"
cmake -B "$build_dir_lttng_exporter_on" -S . -D CMAKE_BUILD_TYPE=Release -D TRACING_ENABLED=ON -D LTTNG_EXPORTER_ENABLED=ON
cmake --build "$build_dir_lttng_exporter_on" --target all --
build_dir_lttng_exporter_off=/tmp/build_dir_lttng_exporter_off
mkdir -p "$build_dir_lttng_exporter_off"
cmake -B "$build_dir_lttng_exporter_off" -S . -D CMAKE_BUILD_TYPE=Release -D TRACING_ENABLED=ON -D LTTNG_EXPORTER_ENABLED=OFF
cmake --build "$build_dir_lttng_exporter_off" --target all --

all_executables=(
  "benchmark-traces-simple" 1
  "benchmark-traces-context-extraction" 1
  "benchmark-traces-event" 1
  "benchmark-traces-attribute" 1
  "benchmark-traces-span-context" 1
  "benchmark-traces-nested-span" 1
  "benchmark-metrics-observable-up-down-counter-500" 10
  "benchmark-metrics-observable-up-down-counter-1000" 10
)
for ((i = 0; i < ${#all_executables[@]}; i += 2)); do
  executable="${all_executables[i]}"
  n="${all_executables[i + 1]}"

  for j in $(seq 1 "$n") ; do
    echo "########## $j executable=$executable TRACING_ENABLED=OFF with local collector without lttng all events disabled ##########"
    "$build_dir_tracing_off/$executable"
  done

  # echo "########## executable=$executable LTTNG_EXPORTER_ENABLED=OFF with local collector without lttng all events disabled ##########"
  # "$build_dir_lttng_exporter_off/$executable"

  # Run benchmark with remote collector if available
  # if curl http://otel-collector-vm.aka.fyty.app:13133/ | grep -q "available"; then
  #     echo "########## executable=$executable LTTNG_EXPORTER_ENABLED=OFF with remote collector without lttng all events disabled ##########"
  #     "$build_dir_lttng_exporter_off/$executable"
  # fi

  # echo "########## executable=$executable LTTNG_EXPORTER_ENABLED=ON without lttng ##########"
  # "$build_dir_lttng_exporter_on/$executable"

  # echo "########## executable=$executable LTTNG_EXPORTER_ENABLED=ON with lttng and all events disabled ##########"
  # echo "Starting a LTTng session ..."
  # lttng create "--output=ctf-traces/$executable/lttng-export-disabled"
  # lttng enable-channel --userspace default-channel
  # lttng start
  # echo "Starting $executable ..."
  # "$build_dir_lttng_exporter_on/$executable"
  # echo "Stop LTTng session ..."
  # lttng stop
  # # echo "View traces ..."
  # # lttng view | sed 's/\(.\{400\}\).*/\1.../'
  # echo "Destroying LTTng session ..."
  # lttng destroy

  #  echo "########## executable=$executable LTTNG_EXPORTER_ENABLED=ON with lttng and all events enabled ##########"
  #  echo "Starting a LTTng session ..."
  #  lttng create "--output=ctf-traces/$executable/lttng-export-enabled"
  #  lttng enable-event -u 'opentelemetry:*'
  #  lttng start
  #  echo "Starting $executable ..."
  #  "$build_dir_lttng_exporter_on/$executable"
  #  echo "Stop LTTng session ..."
  #  lttng stop
  #  # echo "View traces ..."
  #  # lttng view | sed 's/\(.\{400\}\).*/\1.../'
  #  echo "Destroying LTTng session ..."
  #  lttng destroy
done

echo "Done!"
