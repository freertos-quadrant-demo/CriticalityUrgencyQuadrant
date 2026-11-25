#!/bin/bash
# Script to compare PROPER vs IMPROPER priority assignments
# ENHANCED VERSION with CPU stress

echo "========================================================================="
echo "FreeRTOS Priority Quadrant Demo - ENHANCED Comparison Script"
echo "========================================================================="
echo "This version uses CPU-intensive work to demonstrate priority issues"
echo ""

# Function to set priority mode
set_priority_mode() {
    local mode=$1
    sed -i "s/#define USE_PROPER_PRIORITIES.*/#define USE_PROPER_PRIORITIES  $mode/" src/freertos_quadrant_demo_enhanced.c
}

# Function to run demo for limited time
run_demo() {
    timeout 15s ./build/freertos_demo_enhanced 2>&1 || true
}

echo "Step 1: Building and running with IMPROPER priorities..."
echo "---------------------------------------------------------------------"
set_priority_mode 0
make clean > /dev/null 2>&1
make enhanced > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed!"
    exit 1
fi

echo ""
echo "=== SCENARIO 1: IMPROPER PRIORITIES (Ad-hoc) ==="
echo "Watch for [CRITICAL] and [WARNING] messages indicating deadline misses!"
echo ""
run_demo

echo ""
echo ""
echo "Step 2: Building and running with PROPER priorities..."
echo "---------------------------------------------------------------------"
set_priority_mode 1
make clean > /dev/null 2>&1
make enhanced > /dev/null 2>&1

echo ""
echo "=== SCENARIO 2: PROPER PRIORITIES (Quadrant Model) ==="
echo "Fewer deadline misses expected due to correct priority assignment"
echo ""
run_demo

echo ""
echo "========================================================================="
echo "Comparison complete!"
echo "========================================================================="
echo ""
echo "Key differences to observe:"
echo "  - IMPROPER: More [CRITICAL] and [WARNING] messages (deadline misses)"
echo "  - PROPER: Significantly fewer deadline misses, better scheduling"
echo "  - Check 'Missed' column in statistics tables"
echo "  - Check 'Max(ms)' column showing worst-case execution times"
echo ""
echo "The Priority-Urgency Quadrant Model ensures:"
echo "  Q1 (High Urgency + High Criticality): Highest priorities"
echo "  Q2 (Low Urgency + High Criticality): Medium-high priorities"
echo "  Q3 (High Urgency + Low Criticality): Medium priorities"
echo "  Q4 (Low Urgency + Low Criticality): Lowest priorities"
echo ""
