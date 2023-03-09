#!/usr/bin/env bash
#set -e

echo "Building all targets ..."

build_dir_tracing_off=/tmp/build_dir_tracing_off
mkdir -p "$build_dir_tracing_off"
cmake -B "$build_dir_tracing_off" -S . \
  -D CMAKE_BUILD_TYPE=Release \
  -D TRACING_ENABLED=OFF
cmake --build "$build_dir_tracing_off" --target all --

build_dir_opentelemetry_off=/tmp/build_dir_opentelemetry_off
mkdir -p "$build_dir_opentelemetry_off"
cmake -B "$build_dir_opentelemetry_off" -S . \
  -D CMAKE_BUILD_TYPE=Release \
  -D TRACING_ENABLED=ON \
  -D OPENTELEMETRY_C_TRACING_ENABLED=OFF
cmake --build "$build_dir_opentelemetry_off" --target all --

build_dir_lttng_exporter_on_simple=/tmp/build_dir_lttng_exporter_on_simple
mkdir -p "$build_dir_lttng_exporter_on_simple"
cmake -B "$build_dir_lttng_exporter_on_simple" -S . \
  -D CMAKE_BUILD_TYPE=Release \
  -D TRACING_ENABLED=ON \
  -D OPENTELEMETRY_C_TRACING_ENABLED=ON \
  -D LTTNG_EXPORTER_ENABLED=ON \
  -D BATCH_SPAN_PROCESSOR_ENABLED=OFF
cmake --build "$build_dir_lttng_exporter_on_simple" --target all --

build_dir_lttng_exporter_on_batch=/tmp/build_dir_lttng_exporter_on_batch
mkdir -p "$build_dir_lttng_exporter_on_batch"
cmake -B "$build_dir_lttng_exporter_on_batch" -S . \
  -D CMAKE_BUILD_TYPE=Release \
  -D TRACING_ENABLED=ON \
  -D OPENTELEMETRY_C_TRACING_ENABLED=ON \
  -D LTTNG_EXPORTER_ENABLED=ON \
  -D BATCH_SPAN_PROCESSOR_ENABLED=ON
cmake --build "$build_dir_lttng_exporter_on_batch" --target all --

build_dir_lttng_exporter_off_simple=/tmp/build_dir_lttng_exporter_off_simple
mkdir -p "$build_dir_lttng_exporter_off_simple"
cmake -B "$build_dir_lttng_exporter_off_simple" -S . \
  -D CMAKE_BUILD_TYPE=Release \
  -D TRACING_ENABLED=ON \
  -D OPENTELEMETRY_C_TRACING_ENABLED=ON \
  -D LTTNG_EXPORTER_ENABLED=OFF \
  -D BATCH_SPAN_PROCESSOR_ENABLED=OFF \
  -D GENERATE_HIGH_NUMBER_OF_TRACES=OFF
cmake --build "$build_dir_lttng_exporter_off_simple" --target all --

build_dir_lttng_exporter_off_batch=/tmp/build_dir_lttng_exporter_off_batch
mkdir -p "$build_dir_lttng_exporter_off_batch"
cmake -B "$build_dir_lttng_exporter_off_batch" -S . \
  -D CMAKE_BUILD_TYPE=Release \
  -D TRACING_ENABLED=ON \
  -D OPENTELEMETRY_C_TRACING_ENABLED=ON \
  -D LTTNG_EXPORTER_ENABLED=OFF \
  -D BATCH_SPAN_PROCESSOR_ENABLED=ON
cmake --build "$build_dir_lttng_exporter_off_batch" --target all --

n=1 # n = number of run per executable

all_executables=(
  "benchmark-traces-simple"
  "benchmark-traces-context-extraction"
  "benchmark-traces-event"
  "benchmark-traces-attribute"
  "benchmark-traces-span-context"
  "benchmark-traces-nested-span"
)
for executable in "${all_executables[@]}"; do

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=OFF ##########"
    time "$build_dir_tracing_off/$executable"
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=OFF without lttng session ##########"
    time "$build_dir_opentelemetry_off/$executable"
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=OFF in lttng session but all events disabled ##########"
    lttng create
    lttng enable-channel --userspace default-channel
    lttng start
    time "$build_dir_opentelemetry_off/$executable"
    lttng stop
    lttng destroy
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=OFF in lttng session ust event enabled ##########"
    lttng create "--output=ctf-traces/$executable/open-telemetry-off"
    lttng enable-channel --userspace userspace_channel --subbuf-size=2M
    lttng enable-event --channel=userspace_channel -u 'opentelemetry:*'
    lttng start
    time "$build_dir_opentelemetry_off/$executable"
    lttng stop
    lttng destroy
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=OFF in remote lttng session ust event enabled ##########"
    if ping -c 5 "132.207.72.28"; then
      lttng create --set-url=net://132.207.72.28
      lttng enable-channel --userspace userspace_channel --subbuf-size=2M
      lttng enable-event --channel=userspace_channel -u 'opentelemetry:*'
      lttng start
      time "$build_dir_opentelemetry_off/$executable"
      lttng stop
      lttng destroy
    else
      echo "Remote lttng not responding"
    fi
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=OFF LTTNG_EXPORTER_ENABLED=OFF with local collector ##########"
    if curl "http://localhost:13133/" | grep -q "Server available"; then
      time OTEL_EXPORTER_OTLP_TRACES_ENDPOINT="http://localhost:4317/" "$build_dir_lttng_exporter_off_simple/$executable"
    else
      echo "Local collector not responding"
    fi
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=ON LTTNG_EXPORTER_ENABLED=OFF with local collector ##########"
    if curl "http://localhost:13133/" | grep -q "Server available"; then
      time OTEL_EXPORTER_OTLP_TRACES_ENDPOINT="http://localhost:4317/" "$build_dir_lttng_exporter_off_batch/$executable"
    else
      echo "Local collector not responding"
    fi
  done

  # Run benchmark with remote collector only if available
  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=OFF LTTNG_EXPORTER_ENABLED=OFF with remote collector ##########"
    if curl "http://132.207.72.28:13133/" | grep -q "Server available"; then
      time OTEL_EXPORTER_OTLP_TRACES_ENDPOINT="http://132.207.72.28:4317" "$build_dir_lttng_exporter_off_simple/$executable"
    else
      echo "Remote collector not responding"
    fi
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=ON LTTNG_EXPORTER_ENABLED=OFF with remote collector ##########"
    if curl "http://132.207.72.28:13133/" | grep -q "Server available"; then
      time OTEL_EXPORTER_OTLP_TRACES_ENDPOINT="http://132.207.72.28:4317" "$build_dir_lttng_exporter_off_batch/$executable"
    else
      echo "Remote collector not responding"
    fi
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=OFF LTTNG_EXPORTER_ENABLED=ON without lttng session ##########"
    time "$build_dir_lttng_exporter_on_simple/$executable"
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=ON LTTNG_EXPORTER_ENABLED=ON without lttng session ##########"
    time "$build_dir_lttng_exporter_on_batch/$executable"
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=OFF LTTNG_EXPORTER_ENABLED=ON in lttng session but all events disabled ##########"
    lttng create
    lttng enable-channel --userspace default-channel
    lttng start
    time "$build_dir_lttng_exporter_on_simple/$executable"
    lttng stop
    lttng destroy
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=ON LTTNG_EXPORTER_ENABLED=ON in lttng session but all events disabled ##########"
    lttng create
    lttng enable-channel --userspace default-channel
    lttng start
    time "$build_dir_lttng_exporter_on_batch/$executable"
    lttng stop
    lttng destroy
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=OFF LTTNG_EXPORTER_ENABLED=ON in lttng session ust event enabled ##########"
    lttng create
    lttng enable-channel --userspace userspace_channel --subbuf-size=2M
    lttng enable-event --channel=userspace_channel -u 'opentelemetry:*'
    lttng start
    time "$build_dir_lttng_exporter_on_simple/$executable"
    lttng stop
    lttng destroy
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=OFF LTTNG_EXPORTER_ENABLED=ON in remote lttng session ust event enabled ##########"
    if ping -c 5 "132.207.72.28"; then
      lttng create --set-url=net://132.207.72.28
      lttng enable-channel --userspace userspace_channel --subbuf-size=2M
      lttng enable-event --channel=userspace_channel -u 'opentelemetry:*'
      lttng start
      time "$build_dir_lttng_exporter_on_simple/$executable"
      lttng stop
      lttng destroy
    else
      echo "Remote lttng not responding"
    fi
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=ON LTTNG_EXPORTER_ENABLED=ON in lttng session ust event enabled ##########"
    lttng create
    lttng enable-channel --userspace userspace_channel --subbuf-size=2M
    lttng enable-event --channel=userspace_channel -u 'opentelemetry:*'
    lttng start
    time "$build_dir_lttng_exporter_on_batch/$executable"
    lttng stop
    lttng destroy
  done

  for i in $(seq 1 $n); do
    echo "########## Run no $i executable=$executable TRACING_ENABLED=ON OPENTELEMETRY_C_TRACING_ENABLED=ON BATCH_SPAN_PROCESSOR_ENABLED=ON LTTNG_EXPORTER_ENABLED=ON in remote lttng session ust event enabled ##########"
    if ping -c 5 "132.207.72.28"; then
      lttng create --set-url=net://132.207.72.28
      lttng enable-channel --userspace userspace_channel --subbuf-size=2M
      lttng enable-event --channel=userspace_channel -u 'opentelemetry:*'
      lttng start
      time "$build_dir_lttng_exporter_on_batch/$executable"
      lttng stop
      lttng destroy
    else
      echo "Remote lttng not responding"
    fi
  done

done

echo "Done!"
