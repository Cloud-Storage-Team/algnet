#!/usr/bin/env bash
set -exu
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PROJECT=ON -DBUILD_TESTS=OFF -DLOG_LEVEL=LOG_LEVEL_ERROR -DCMAKE_CXX_FLAGS="-g -fno-omit-frame-pointer"
cmake --build build -j $(nproc)
sudo perf record -F 99 -g -- ./build/nons -c configs/fat-tree/huge-scenario.yml --use-new-simulator > perf.data
perf report -i perf.data --header-only
perf script -i perf.data -v > out.perf 2>&1

FLAMEGRAPH_OUTPUT_DIR=flamegraph

mkdir -p ${FLAMEGRAPH_OUTPUT_DIR}

stackcollapse-perf out.perf > out.folded
flamegraph out.folded > ${FLAMEGRAPH_OUTPUT_DIR}/flamegraph.svg
python3 scripts/generate-html.py ./${FLAMEGRAPH_OUTPUT_DIR}
