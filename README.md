# FreeRTOS Criticality-Urgency Quadrant Model Demo

Reference implementation of the **Criticality-Urgency Quadrant Model (CUQM)** for systematic fixed-priority task assignment in Real-Time Operating Systems (RTOS), running on the **FreeRTOS POSIX (Linux) port**.

## Associated Publication

> **Azad Mohammed Shaik**, *"The Criticality-Urgency Quadrant Model: A Systematic Approach to Task Priority Assignment in Real-Time Operating Systems,"* SSRG International Journal of Computer Science and Engineering (IJCSE), Vol. 13, Issue 3, pp. 1–19, March 2026.  
> DOI: [10.14445/23488387/IJCSE-V13I3P101](https://doi.org/10.14445/23488387/IJCSE-V13I3P101)

## Overview

The Criticality-Urgency Quadrant Model classifies real-time tasks along two orthogonal dimensions—**Criticality** (safety impact) and **Urgency** (deadline tightness)—into four ordered priority bands:

| Quadrant | Classification | Priority Band |
|---|---|---|
| **Q1** | High Criticality, High Urgency | Highest |
| **Q2** | High Criticality, Low Urgency | Medium-High |
| **Q3** | Low Criticality, High Urgency | Medium |
| **Q4** | Low Criticality, Low Urgency | Lowest |

## Repository Structure

- `src/freertos_quadrant_demo_linux.c` — Baseline FreeRTOS POSIX demonstration comparing ad-hoc vs. Quadrant Model priorities
- `src/freertos_quadrant_demo_enhanced.c` — Enhanced CPU-stress benchmark highlighting deadline misses and response-time margins
- `src/FreeRTOSConfig_linux.h` & `src/FreeRTOSConfig.h` — FreeRTOS POSIX configuration (1 ms tick, 10 priority levels)
- `compare_priorities.sh` / `compare_enhanced.sh` — Automated comparison scripts (Improper vs. Quadrant priorities)
- `Makefile` — Build automation including FreeRTOS-Kernel fetch

## Quick Start (Linux / WSL)

### Prerequisites
- `gcc`, `make`, `git`, and POSIX `pthread` / `librt`

### Build & Run
```bash
# 1. Fetch FreeRTOS-Kernel V11.1.0
make fetch-freertos

# 2. Build and run the baseline demo
make run

# 3. Build and run the enhanced CPU-stress demo
make run-enhanced

# 4. Run automated Improper vs. Proper priority comparison
chmod +x compare_priorities.sh compare_enhanced.sh
./compare_enhanced.sh
```

## Citation

```bibtex
@article{shaik2026cuqm,
  author  = {Shaik, Azad Mohammed},
  title   = {The Criticality-Urgency Quadrant Model: A Systematic Approach to Task Priority Assignment in Real-Time Operating Systems},
  journal = {SSRG International Journal of Computer Science and Engineering},
  volume  = {13},
  number  = {3},
  pages   = {1--19},
  year    = {2026},
  month   = {March},
  doi     = {10.14445/23488387/IJCSE-V13I3P101}
}
```

## License

This project is released under the [MIT License](LICENSE). FreeRTOS-Kernel is fetched separately under its upstream MIT license.
