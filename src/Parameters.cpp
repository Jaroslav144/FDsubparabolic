#include "Parameters.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

namespace fs = std::filesystem;

namespace
{

std::string trim(std::string s)
{
    auto not_space =
        [](unsigned char c)
        {
            return !std::isspace(c);
        };

    s.erase(
        s.begin(),
        std::find_if(s.begin(), s.end(), not_space));

    s.erase(
        std::find_if(s.rbegin(), s.rend(), not_space).base(),
        s.end());

    return s;
}

InitialProfile parse_profile(const std::string& s)
{
    if (s == "linear")
        return InitialProfile::Linear;

    if (s == "error_function")
        return InitialProfile::ErrorFunction;

    throw std::runtime_error(
        "Unknown initial profile: " + s);
}

}

std::vector<Parameters> Parameters::read(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file)
        throw std::runtime_error(
            "Cannot open " + filename);

    std::vector<Parameters> cases;

    Parameters current;

    bool has_data = false;

    std::string line;

    while (std::getline(file, line))
    {
        line = trim(line);

        if (line.empty())
            continue;

        if (line[0] == '#')
            continue;

        if (line == "---")
        {
            if (has_data)
            {
                cases.push_back(current);

                current = Parameters();

                has_data = false;
            }

            continue;
        }

        auto pos = line.find('=');

        if (pos == std::string::npos)
            continue;

        std::string key =
            trim(line.substr(0, pos));

        std::string value =
            trim(line.substr(pos + 1));

        has_data = true;

        //---------------------------------

        if (key == "case")
            current.case_name = value;

        else if (key == "Nx")
            current.Nx = std::stoi(value);

        else if (key == "alpha")
            current.alpha = std::stod(value);

        else if (key == "dt")
            current.dt = std::stod(value);

        else if (key == "t_end")
            current.t_end = std::stod(value);

        else if (key == "output_interval")
            current.output_interval =
                std::stoi(value);

        else if (key == "m")
            current.m = std::stod(value);

        else if (key == "k_grain")
            current.k_grain = std::stod(value);

        else if (key == "C_eq")
            current.C_eq = std::stod(value);

        else if (key == "C_max")
            current.C_max = std::stod(value);

        else if (key == "D_gb")
            current.D_gb = std::stod(value);

        else if (key == "ell0")
            current.ell0 = std::stod(value);

        else if (key == "initial_profile")
            current.initial_profile =
                parse_profile(value);

        else
        {
            throw std::runtime_error(
                "Unknown parameter: " + key);
        }
    }

    if (has_data)
        cases.push_back(current);

    return cases;
}

void Parameters::write_readme(
    const std::string& directory) const
{
    fs::create_directories(directory);

    std::ofstream file(
        directory + "/README.txt");

    file << "Case: " << case_name << "\n\n";

	file << "[Diffusion]\n";
file << "D_gb = " << D_gb << "\n\n";

file << "[Grain growth]\n";
file << "k_grain = " << k_grain << '\n';
file << "m = " << m << "\n\n";

file << "[Thermodynamics]\n";
file << "C_max = " << C_max << '\n';
file << "C_eq = " << C_eq << "\n\n";

file << "[Numerics]\n";
file << "Nx = " << Nx << '\n';
file << "dt = " << dt << '\n';
file << "t_end = " << t_end << '\n';
file << "output_interval = "
     << output_interval << "\n\n";

file << "[Initial state]\n";
file << "ell0 = " << ell0 << '\n';

    file << "[Initial concentration]\n";

    file << "Profile = ";

    switch (initial_profile)
    {
        case InitialProfile::Linear:
            file << "linear";
            break;

        case InitialProfile::ErrorFunction:
            file << "error_function";
            break;
    }

    file << '\n';
}
