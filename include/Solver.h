#pragma once

#include "Grid.h"
#include "Output.h"
#include "Parameters.h"
#include "DiffusivityModel.h"

#include <vector>


class Solver
{
public:

    Solver(const Parameters& params_);

    void initialize();

    void run();

private:

    //--------------------------------------------------
    // Core objects
    //--------------------------------------------------

    Parameters params;

    Grid grid;

    DiffusivityModel diffusivity_model;

    Output output;

    // Time-step index
    int step;
    double time;
    double dt;

    //--------------------------------------------------
    // Primary fields
    //--------------------------------------------------

    std::vector<double> C;

    std::vector<double> tau;

    std::vector<double> D;

    std::vector<double> D_old;
    
    // Innitialization
    void initialize_linear();
    void initialize_erfc();

    //--------------------------------------------------
    // Moving interface
    //--------------------------------------------------

    double ell;

    double ell_dot;

    //--------------------------------------------------
    // History
    //--------------------------------------------------

    std::vector<double> time_history;

    std::vector<double> ell_history;

    //--------------------------------------------------
    // Core stepping
    //--------------------------------------------------

    void time_step();

    //--------------------------------------------------
    // Physics updates
    //--------------------------------------------------

    void update_diffusivity();

    void update_concentration();

    void update_interface();

    void update_age();

    //--------------------------------------------------
    // Utilities
    //--------------------------------------------------

    double compute_timestep();

    double harmonic_mean(
        double a,
        double b
    ) const;

    //--------------------------------------------------
    // Output
    //--------------------------------------------------

    void save_results();
};
