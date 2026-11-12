#pragma once

#include <string>
#include <vector>

enum class InitialProfile
{
	Linear,
	ErrorFunction
};

struct Parameters
{
    //--------------------------------------------------
    // Identification
    //--------------------------------------------------

    std::string case_name = "case";

    //--------------------------------------------------
    // Diffusion parameters
    //--------------------------------------------------

    double D_gb = 1.0e3;

    //--------------------------------------------------
    // Grain growth parameters
    //--------------------------------------------------

    double k_grain = 1.0e10;

    double m = 2.0;

    //--------------------------------------------------
    // Thermodynamic parameters
    //--------------------------------------------------

    double C_max = 1.0;

    double C_eq = 0.0;

    //--------------------------------------------------
    // Numerical parameters
    //--------------------------------------------------

    int Nx = 1000;

    double dt = 1.0e3;

    double alpha = 1.0;

    double t_end = 10.0;

    int output_interval = 100;

    // Initial layer thickness

    double ell0 = 1e-2;

    // Initial concentration
    InitialProfile initial_profile = InitialProfile::Linear;

    //---------------------------------

    static std::vector<Parameters>
    read(const std::string& filename);

    //---------------------------------

    void write_readme(
        const std::string& directory) const;
};
