#!/usr/bin/env bash

set -e

echo "#######################################################################"
echo "# Traces benchmarks"
echo "#######################################################################"

./run-traces-benchmarks.sh

echo "#######################################################################"
echo "# Metrics benchmarks"
echo "#######################################################################"

./run-metrics-benchmarks.sh

echo "#######################################################################"
echo "# Logs benchmarks"
echo "#######################################################################"

./run-logs-benchmarks.sh