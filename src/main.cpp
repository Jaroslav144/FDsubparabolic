#include "Parameters.h"
#include "Solver.h"

#include <exception>
#include <iostream>

int main()
{
    try
    {
        // Read all simulation cases
        std::vector<Parameters> cases = Parameters::read("parameters.txt");

        std::cout
            << "Found "
            << cases.size()
            << " simulation case(s).\n";

        // Run all cases
        for (const auto& params : cases)
        {
            std::cout
                << "----------------------------------------\n";

            std::cout
                << "Running case: "
                << params.case_name
                << std::endl;

            Solver solver(params);

            solver.run();

            std::cout
                << "Finished.\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "Fatal error:\n"
            << e.what()
            << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
