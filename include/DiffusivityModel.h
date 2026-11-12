#pragma once

#include "Parameters.h"

class DiffusivityModel
{
public:

    DiffusivityModel(const Parameters& params_);

    double compute_D(double tau) const;

private:

    Parameters params;

};
