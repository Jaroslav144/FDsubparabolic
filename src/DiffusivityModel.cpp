#include "DiffusivityModel.h"

#include <cmath>

DiffusivityModel::DiffusivityModel(
    const Parameters& params_
)
:
params(params_)
{}

double DiffusivityModel::compute_D(
    double tau
) const
{
    double term = 1.0 + params.k_grain * tau;
    // double term = params.k_grain * (tau + 1e-14);

    double grain_factor = std::pow(term, -1.0 / params.m);

    return
       1.0 + params.D_gb * grain_factor;
        // params.D_gb * grain_factor;
}
