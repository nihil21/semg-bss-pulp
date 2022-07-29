#!/bin/bash

# Set working directory
cd ../

# Iterate over strategies
for c in {1..8}; do
    echo "Cores $c" | tee -a report/report_cores.txt;
    make clean all NUM_CORES=$c USE_SVM=1
    make run | tee -a report/report_cores.txt
done
