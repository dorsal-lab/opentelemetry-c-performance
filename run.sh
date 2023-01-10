#!/usr/bin/env bash

set -ex

echo "Building all targets ..."

mkdir -p build
cmake -B build -S . -D CMAKE_BUILD_TYPE=Release
cmake --build build/ --target all --

# echo "Starting a LTTng session ..."
# lttng create --output=ctf-traces/
# lttng enable-event -u 'opentelemetry:*'
# lttng add-context -u -t vtid
# lttng start

echo "Starting benchmark-traces-simple ..."
./build/benchmark-traces-simple

echo "Starting benchmark-traces-context-extraction ..."
./build/benchmark-traces-context-extraction

echo "Starting benchmark-traces-event ..."
./build/benchmark-traces-event

echo "Starting benchmark-traces-attribute ..."
./build/benchmark-traces-attribute

echo "Starting benchmark-traces-span-context ..."
./build/benchmark-traces-span-context

echo "Starting benchmark-traces-nested-span ..."
./build/benchmark-traces-nested-span

# echo "Stop LTTng session ..."
# lttng stop

# echo "View traces ..."
# lttng view | sed 's/\(.\{400\}\).*/\1.../'

# echo "Destroting LTTng session ..."
# lttng destroy

echo "Done!"
