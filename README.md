# Reaction–diffusion with grain growth

This repository contains the code accompanying the paper submitted to
*Physical Chemistry Chemical Physics* (PCCP):

**Sub-parabolic growth of reaction layers: a numerical study of grain coarsening and diffusion**  
Ya. A. Nikiforov, S. A. Chizhik, and N. I. Baklanova.

The preprint is available on arXiv: [arXiv:2609.01002](https://arxiv.org/abs/2609.01002).

The code implements a moving-boundary diffusion model for product-layer growth
in a solid-state reaction, accounting for grain growth and contributions from
bulk and grain-boundary diffusion. See the paper for the model and its physical
interpretation.

## Build and run

Requirements: a C++17-compatible compiler and CMake 3.16 or newer.

From the repository root, run:

```bash
cmake -S . -B build
cmake --build build
./build/FDsubparabolic
```

The executable reads simulation cases from `parameters.txt` in the current
working directory. Edit this file to configure the simulations.

## Repository contents

- `src/`: C++ implementation.
- `include/`: C++ headers.
- `parameters.txt`: simulation parameters.
- `CMakeLists.txt`: build configuration.

The generated `build/` and `output/` directories are excluded from version
control by `.gitignore`.

## Citation

If you use this code in your research, please cite the accompanying preprint:

> Ya. A. Nikiforov, S. A. Chizhik, and N. I. Baklanova,
> “Numerical study of the grain-growth-induced non-parabolic kinetics of a solid-state reaction,”
> arXiv:2609.01002 (2026). https://doi.org/10.48550/arXiv.2609.01002
