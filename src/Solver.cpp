#include "Solver.h"

#include "IO.h"
#include "Output.h"
#include "TridiagonalSolver.h"


#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>

Solver::Solver(const Parameters& params_)
    :
    params(params_),
    grid(params_.Nx),
    step(0),
    diffusivity_model(params_),
    output("output/" + params_.case_name)
    {
        C.resize(grid.Nx);
    
        tau.resize(grid.Nx);
    
        D.resize(grid.Nx);
        D_old.resize(grid.Nx);
	params.write_readme("output/" + params_.case_name);
    }

void Solver::initialize_linear()
{
    for(int i = 0; i < grid.Nx; ++i)
    {
        C[i] = params.C_max - (params.C_max - params.C_eq) * i / (grid.Nx - 1);
    }
}

void Solver::initialize_erfc()
{
    for(int i = 0; i < grid.Nx; ++i)
    {
		C[i] = params.C_max - (params.C_max - params.C_eq) * std::erf(i * 0.62/ grid.Nx) / std::erf(0.62);
    }
}

void Solver::initialize()
{
    ell = params.ell0;

    ell_dot = 0.0;

    time = 0.0;

    dt = params.dt;


    //--------------------------------------------------
    // Initial concentration
    //--------------------------------------------------

	switch(params.initial_profile)
{
case InitialProfile::Linear:

    initialize_linear();

    break;

case InitialProfile::ErrorFunction:

    initialize_erfc();

    break;
}

    //--------------------------------------------------
    // Initial age field
    //--------------------------------------------------

    for(int i = 0; i < grid.Nx; ++i)
    {
        tau[i] = 0.0;
    }

    //--------------------------------------------------
    // Initial diffusivity
    //--------------------------------------------------

    update_diffusivity();

    //--------------------------------------------------
    // History
    //--------------------------------------------------

    time_history.push_back(time);

    ell_history.push_back(ell);
}

double Solver::harmonic_mean(
    double a,
    double b
) const
{
    return 2.0 * a * b / (a + b);
}

double Solver::compute_timestep()
{
	// Maximum geometrix velocity
	double u_max = std::abs(ell_dot / ell);

	// Maximum diffusivity
	double Dmax = *std::max_element(D.begin(), D.end());

	// Advection-based timestep 
	double dt_adv = 1e-2 * params.alpha * std::abs(ell / ell_dot);

	// Diffusion-based timestep
	double dt_diff =  params.alpha * ell * ell / params.D_gb;

	// Desired timestep
	double dt_target = std::min(dt_adv, dt_diff);

	double dt_new = std::min(dt_target, params.dt);

	return dt_new;
}

void Solver::update_diffusivity()
{
    for(int i = 0; i < grid.Nx; ++i)
    {
        D[i] = diffusivity_model.compute_D(tau[i]);
    }
}

void Solver::update_concentration()
{
    int N = grid.Nx;

    // time-dependent helper variables
    double ell_n = ell_history[step-1];
    double ell_np1 = ell_history[step];

    // Tridiagonal matrix preparation
    std::vector<double> a(N, 0.0);
    std::vector<double> b(N, 0.0);
    std::vector<double> c(N, 0.0);
    std::vector<double> d(N, 0.0);

    //--------------------------------------------------
    // Left boundary
    // C = C_max
    //--------------------------------------------------

    b[0] = 1.0;
    d[0] = params.C_max;

    //--------------------------------------------------
    // Interior nodes
    //--------------------------------------------------

    for(int i = 1; i < N - 1; ++i)
    {
        // Coordinate

	double xi_i = grid.xi[i];

        //--------------------------------------------------
        // Diffusion coefficients
        //--------------------------------------------------

	// Old
        double Dleft_n = 0.5 * ( D_old[i] + D_old[i-1]);
        double Dright_n = 0.5 * ( D_old[i] + D_old[i+1]);

	// New
        double Dleft_np1 = 0.5 * ( D[i] + D[i-1]);
        double Dright_np1 = 0.5 * ( D[i] + D[i+1]);

        //--------------------------------------------------
        // Crank-Nicholson coefficients
        //--------------------------------------------------

	double S_n = dt * (ell_dot / ell_n) * xi_i / (4.0 * grid.dxi);
	double S_np1 = dt * (ell_dot / ell_np1) * xi_i / (4.0 * grid.dxi);
	
	double An = dt * Dleft_n 
			/
			(2.0 * (ell_n * grid.dxi) * (ell_n * grid.dxi));
	double Bn = dt * Dright_n 
			/
			(2.0 * (ell_n * grid.dxi) * (ell_n * grid.dxi));

	double Anp1 = dt * Dleft_np1 
			/
			(2.0 * (ell_np1 * grid.dxi) * (ell_np1 * grid.dxi));
	double Bnp1 = dt * Dright_np1
			/
			(2.0 * (ell_np1 * grid.dxi) * (ell_np1 * grid.dxi));

        //--------------------------------------------------
        // Matrix entries
        //--------------------------------------------------

        a[i] = -Anp1 + S_np1;

        b[i] = 1.0 + Anp1 + Bnp1;

        c[i] = -Bnp1 - S_np1;

        //--------------------------------------------------
        // Right-hand side
        //--------------------------------------------------

        d[i] = C[i - 1] * (An - S_n) + C[i] * (1 - An - Bn) + C[i + 1] * (Bn + S_n);
    }

    //--------------------------------------------------
    // Right boundary condition
    // C = C_eq
    //--------------------------------------------------

    b[N - 1] = 1.0;
    d[N - 1] = params.C_eq;

    //--------------------------------------------------
    // Solve tridiagonal system
    //--------------------------------------------------

    C = solve_tridiagonal(a, b, c, d);
}

void Solver::update_interface()
{
    int N = grid.Nx;

    //--------------------------------------------------
    // Concentration gradient
    //--------------------------------------------------

    double gradC = (C[N - 1] - C[N - 2]) / grid.dxi;

    //--------------------------------------------------
    // Interface velocity
    //--------------------------------------------------

    ell_dot = - D[N-1] * gradC / ell;

    dt = compute_timestep();
    //--------------------------------------------------
    // Update interface
    //--------------------------------------------------

    ell += dt * ell_dot;

    ell_history.push_back(ell);
}

void Solver::update_age()
{
    int N = grid.Nx;

    // time-dependent helper variables
    
    double ell_n = ell_history[step-1];
    double ell_np1 = ell_history[step];
    double time_np1 = time + dt;

    // Tridiagonal matrix preparation
    std::vector<double> a(N, 0.0);
    std::vector<double> b(N, 0.0);
    std::vector<double> c(N, 0.0);
    std::vector<double> d(N, 0.0);

    // Left boundary
    // tau[0] = t

    b[0] = 1.0;

    a[0] = 0.0;
    c[0] = 0.0;

    d[0] = time_np1;

    //--------------------------------------------------
    // Interior nodes
    //--------------------------------------------------

    for(int i = 1; i < N - 1; ++i)
    {
	double xi_i = grid.xi[i];

        //--------------------------------------------------
        // Crank-Nicholson coefficients
        //--------------------------------------------------

	double S_n = dt * (ell_dot / ell_n) * xi_i / (4.0 * grid.dxi);
	double S_np1 = dt * (ell_dot / ell_np1) * xi_i / (4.0 * grid.dxi);

        // Matrix coefficients
	
	a[i] = S_np1;
	b[i] = 1.0;
	c[i] = -S_np1;

	// Right-hand side coefficients
	d[i] = tau[i] + dt + S_n * ( tau[i+1] - tau[i-1] );

    }

    //--------------------------------------------------
    // Right boundary:
    // freshly formed material
    //--------------------------------------------------

    a[N-1] = 0.0;
    c[N-1] = 0.0;
    b[N-1] = 1.0;
    d[N-1] = 0.0;

    //--------------------------------------------------
    // Update field
    //--------------------------------------------------

    tau = solve_tridiagonal(a, b, c, d);
}


void Solver::time_step()
{

    update_interface();
    update_age();

    D_old = D;

    update_diffusivity();
    update_concentration();

    time += dt;

    time_history.push_back(time);

}

void Solver::run()
{
    initialize();

    output.write(step, time, ell, grid, C, tau, D);

    step++;

    while ( time < params.t_end )
    {
        time_step();

        if(step % params.output_interval == 0)
        {
    		output.write(step, time, ell, grid, C, tau, D);
        }
	step++;
    }

    save_results();
}

void Solver::save_results()
{
/*    write_profile(
        "results/final_profile.dat",
        grid.xi,
        C,
        ell,
        time
    );
*/
    output.write_interface_history(
        time_history,
        ell_history,
		params.output_interval / 10
    );
}
