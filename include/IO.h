#pragma once

#include <vector>
#include <string>

void write_profile(
    const std::string& filename,
    const std::vector<double>& xi,
    const std::vector<double>& C,
    double ell,
    double time
);

void write_interface_history(
    const std::string& filename,
    const std::vector<double>& time_history,
    const std::vector<double>& ell_history
);