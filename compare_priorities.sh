#!/bin/bash
# Script to compare PROPER vs IMPROPER priority assignments

echo "========================================================================="
echo "FreeRTOS Priority Quadrant Demo - Comparison Script"
echo "========================================================================="
echo ""

# Function to set priority mode
set_priority_mode() {
    local mode=$1
    sed -i "s/#define USE_PROPER_PRIORITIES.*/#define USE_PROPER_PRIORITIES  $mode/" src/freertos_quadrant_demo_linux.c
}

# Function to run demo for limited time
run_demo() {
    timeout 10s ./build/freertos_demo 2>&1 || true
}

echo "Step 1: Building and running with IMPROPER priorities..."
echo "---------------------------------------------------------------------"
set_priority_mode 0
make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed!"
    exit 1
fi

echo ""
echo "=== SCENARIO 1: IMPROPER PRIORITIES (Ad-hoc) ==="
echo ""
run_demo

echo ""
echo ""
echo "Step 2: Building and running with PROPER priorities..."
echo "---------------------------------------------------------------------"
set_priority_mode 1
make clean > /dev/null 2>&1
make > /dev/null 2>&1

echo ""
echo "=== SCENARIO 2: PROPER PRIORITIES (Quadrant Model) ==="
echo ""
run_demo

echo ""
echo "========================================================================="
echo "Comparison complete!"
echo "========================================================================="
echo ""
echo "Key differences to observe:"
echo "  - IMPROPER: More deadline misses and warnings"
echo "  - PROPER: Fewer deadline misses, better task scheduling"
echo ""
