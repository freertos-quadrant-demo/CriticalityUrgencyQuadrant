#!/bin/bash
# Quick test of the enhanced demo

echo "Testing Enhanced FreeRTOS Quadrant Demo..."
echo "Running for 8 seconds..."
echo ""

timeout 8s ./build/freertos_demo_enhanced 2>&1

echo ""
echo "Test complete!"
