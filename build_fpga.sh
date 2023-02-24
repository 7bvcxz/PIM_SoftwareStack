#!/bin/sh

echo "Assume that ../fpga_runtime.c exists (if not, error causes)"

make clean && make fpga && make
