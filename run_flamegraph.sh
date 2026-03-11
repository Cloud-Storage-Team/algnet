#!/usr/bin/env bash
set -exu
sudo perf record -F 99 -g -- ./build/nons -c configs/fat-tree/huge-scenario.yml --use-new-simulator > perf.data
perf report -i perf.data --header-only
perf script -i perf.data -v > out.perf 2>&1

FLAMEGRAPH_OUTPUT_DIR=flamegraph

mkdir -p ${FLAMEGRAPH_OUTPUT_DIR}

stackcollapse-perf out.perf > out.folded
flamegraph
out.folded > ${FLAMEGRAPH_OUTPUT_DIR}/flamegraph.svg
python3 scripts/generate-html.py ./${FLAMEGRAPH_ARTIFACT_DIR}
