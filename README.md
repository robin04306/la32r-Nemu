# LA32R NEMU — Custom Instruction Support for Quantized CNN Research

> **Research-modified NEMU implementation.** This repository contains a LA32R NEMU codebase with modifications to instruction decoding, execution, and architectural state for research on accelerating quantized convolutional neural network (CNN) workloads. It is based on the upstream LA32R NEMU project and is **not** an original implementation of NEMU from scratch.

## Overview

NEMU is an instruction-set emulator used for architecture research and software experimentation. This repository preserves the underlying LA32R NEMU implementation and includes research changes intended to support custom instructions for quantized CNN inference.

The broader research project studies two distinct approaches:

1. **Software / ISA evaluation:** extend LA32R NEMU and the associated assembly/toolchain workflow to execute custom CNN-oriented instructions and compare their behavior with a baseline.
2. **FPGA datapath evaluation:** implement and evaluate dedicated RTL vector datapaths on an FPGA. **Those FPGA artifacts are not included in this NEMU-only repository unless explicitly added later.** FPGA measurements must not be presented as NEMU simulation measurements.

## Research objectives

- Investigate instruction-level acceleration of quantized CNN operations on LA32R.
- Extend instruction decode and execution support for custom operations.
- Preserve functional correctness when comparing baseline and custom instruction sequences.
- Collect reproducible instruction counts and emulator timing under equivalent workloads and measurement conditions.
- Keep software-emulator and FPGA hardware evaluations clearly separated.

## What is included

The initial upload contains the NEMU source tree and local research modifications. The following paths are especially relevant:

| Path | Purpose |
| --- | --- |
| `src/isa/la32r/instr/decode.c` | LA32R instruction decoding |
| `src/isa/la32r/instr/compute.h` | Arithmetic / compute instruction logic |
| `src/isa/la32r/instr/ldst.h` | Load/store instruction logic |
| `src/isa/la32r/include/isa-all-instr.h` | Instruction-related declarations |
| `src/isa/la32r/include/isa-def.h` | ISA-specific architectural definitions |
| `src/isa/la32r/local-include/reg.h` | Register-related definitions and helpers |
| `src/cpu/cpu-exec.c` | CPU execution loop |
| `include/` | Common interfaces and headers |
| `configs/` | Configuration presets |
| `scripts/` | Build and configuration scripts |
| `tools/` | Supporting build and development tools |
| `Makefile`, `Kconfig` | Build and configuration entry points |

**Scope note:** The table identifies source locations, not a complete verified inventory of custom opcodes. Refer to the actual source and experiment artifacts for the precise instruction set and encodings.

## Relationship to upstream

**Upstream LA32R NEMU:** https://gitee.com/wwt_panache/la32r-nemu

This repository was prepared from a local copy of that project. The research working tree included modifications to these seven files:

```text
NEMU/src/cpu/cpu-exec.c
NEMU/src/isa/la32r/include/isa-all-instr.h
NEMU/src/isa/la32r/include/isa-def.h
NEMU/src/isa/la32r/instr/compute.h
NEMU/src/isa/la32r/instr/decode.c
NEMU/src/isa/la32r/instr/ldst.h
NEMU/src/isa/la32r/local-include/reg.h
```

Because this GitHub repository was initialized with a new root commit, its Git history does **not** preserve the upstream commit history. To inspect the research changes precisely, compare the above files against the corresponding upstream revision. The upstream revision/commit hash should be recorded here once confirmed.

## Requirements

The project was prepared in a Linux environment using Ubuntu under Windows Subsystem for Linux (WSL). A typical development setup includes:

- Ubuntu / WSL2 or another supported Linux environment
- Git
- GNU Make
- A C compiler and related build tools
- Any additional packages required by the original NEMU build configuration

The exact dependency list and build steps depend on the chosen NEMU configuration. Consult the upstream documentation and the included `Makefile`, `Kconfig`, and `configs/` files before building.

## Getting started

Clone this repository:

```bash
git clone https://github.com/robin04306/la32r-Nemu.git
cd la32r-Nemu
```

Inspect available build targets and configurations:

```bash
make help
ls configs/
```

> **Build verification:** The commands above are entry points for inspecting the project; a full clean-clone build and execution sequence has not yet been documented or independently verified for this GitHub snapshot. Do not assume that `make help` is available on every upstream revision. Record a tested configuration, compiler version, build command, and sample invocation here after verification.

## Suggested reproducibility workflow

A complete experiment should record:

1. The exact Git commit and NEMU configuration.
2. The baseline and custom assembly source files.
3. Toolchain versions and assembly/link commands.
4. Input tensors, quantization parameters, and workload dimensions.
5. The emulator invocation and measurement boundaries.
6. Correctness checks comparing expected and actual outputs.
7. Dynamic instruction counts and emulator execution times for each implementation.

A results table can be added after measurements have been reproduced:

| Metric | Baseline | Custom instructions | Notes |
| --- | --- | --- | --- |
| Functional output | To verify | To verify | Use identical inputs |
| Dynamic instruction count | To measure | To measure | Same workload |
| NEMU execution time | To measure | To measure | Same environment and boundaries |
| FPGA cycle count | Not part of this repository | Not part of this repository | Report separately |

**Measurement caution:** A reduction in dynamic instruction count is not automatically an equivalent speedup in execution time. Likewise, an FPGA cycle-count ratio is not necessarily an end-to-end CNN speedup. Avoid comparing NEMU wall-clock measurements directly with FPGA hardware timing.

## Current repository scope and planned documentation

The initial GitHub upload contains the `NEMU/` source directory only. Related research assets from the broader local project—such as CNN assembly workloads, baseline/custom test scripts, FPGA RTL, waveform captures, and results—are not implied to be present here.

Useful future additions include:

- `docs/instruction-set.md` — custom instruction names, encodings, operands, and semantics
- `docs/build-and-run.md` — tested build and execution instructions
- `experiments/` — baseline and custom workloads, plus reproducibility scripts
- `results/` — validated logs, tables, and plots
- `docs/validation.md` — correctness tests and comparison criteria

## Attribution and license

This project builds on the LA32R NEMU implementation distributed from:

https://gitee.com/wwt_panache/la32r-nemu

The supplied upstream licensing material identifies the **Mulan Permissive Software License, Version 2 (Mulan PSL v2)** and lists the following copyright holders:

- Copyright (c) 2014–2021 Zihao Yu, Nanjing University
- Copyright (c) 2020–2021 Institute of Computing Technology, Chinese Academy of Sciences
- Copyright (c) 2021–2022 Weitong Wang, University of Chinese Academy of Sciences

Mulan PSL v2 permits redistribution and modification subject to its conditions, including providing recipients with the license and retaining applicable copyright, patent, trademark, and disclaimer notices. **Include the complete original license text in a top-level `LICENSE` file before treating this repository as distribution-ready.** Keep any third-party component licenses and notices as well. Research modifications do not transfer ownership of the upstream code.

License reference: http://license.coscl.org.cn/MulanPSL2

## Citation

If you use this research implementation, cite the relevant upstream NEMU project and the associated research publication where appropriate. Add a verified paper citation or BibTeX entry here when its final bibliographic details are available.

## Contact and contributions

For questions about this repository, use the GitHub Issues page:

https://github.com/robin04306/la32r-Nemu/issues

Contributions and corrections are welcome, subject to the project's license and the need to preserve attribution.
