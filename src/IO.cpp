#include "IO.h"

#include <fstream>

void write_profile(
    const std::string& filename,
    const std::vector<double>& xi,
    const std::vector<double>& C,
    double ell,
    double time
)
{
    std::ofstream file(filename);

    file << "# x C\n";

    for(size_t i = 0; i < xi.size(); ++i)
    {
        double x = xi[i] * ell;

        file << x << " " << C[i] << "\n";
    }
}

void write_interface_history(
    const std::string& filename,
    const std::vector<double>& time_history,
    const std::vector<double>& ell_history
)
{
    std::ofstream file(filename);

    file << "# time ell\n";

    for(size_t i = 0; i < time_history.size(); ++i)
    {
        file << time_history[i]
             << " "
             << ell_history[i]
             << "\n";
    }
}