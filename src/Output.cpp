#include "Output.h"
#include "Grid.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

Output::Output(const std::string& directory)
    : directory(directory)
{
    fs::create_directories(directory);

    //interface_file.open(directory + "/interface.txt");

    // interface_file         << "# time"        << '\t'        << "ell"        << '\t'        << "ell_dot"        << '\n';
}

void Output::write(
    int step,
    double time,
    double ell,
    const Grid& grid,
    const std::vector<double>& concentration,
    const std::vector<double>& age,
    const std::vector<double>& diffusion)
{
    //--------------------------------------------------
    // File name
    //--------------------------------------------------

    std::ostringstream name;

    name
        << "field_"
        << std::setw(6)
        << std::setfill('0')
        << step
        << ".vtk";

    std::string filename = name.str();

    std::ofstream file(directory + "/" + filename);

    //--------------------------------------------------
    // Header
    //--------------------------------------------------

    file
        << "# vtk DataFile Version 3.0\n";

    file
        << "Reaction-diffusion simulation\n";

    file
        << "ASCII\n\n";

    //--------------------------------------------------
    // Dataset
    //--------------------------------------------------

    file
        << "DATASET STRUCTURED_GRID\n";

    file
        << "DIMENSIONS "
        << grid.Nx
        << " 1 1\n\n";

    //--------------------------------------------------
    // Coordinates
    //--------------------------------------------------

    file
        << "POINTS "
        << grid.Nx
        << " float\n";

    for (int i = 0; i < grid.Nx; ++i)
    {
        double x = ell * grid.xi[i];

        file
            << x
            << " 0.0 0.0\n";
    }

    file << "\n";

    //--------------------------------------------------
    // Fields
    //--------------------------------------------------

    file
        << "POINT_DATA "
        << grid.Nx
        << "\n\n";

    write_scalar(file, "xi", grid.xi);

    write_scalar(file, "concentration", concentration);

    write_scalar(file, "age", age);

    write_scalar(file, "diffusion", diffusion);

    file.close();

    //--------------------------------------------------
    // Update PVD
    //--------------------------------------------------

    entries.push_back({time, filename});

    write_pvd();
}

void Output::write_scalar(
    std::ofstream& file,
    const std::string& name,
    const std::vector<double>& values) const
{
    file
        << "SCALARS "
        << name
        << " float\n";

    file
        << "LOOKUP_TABLE default\n";

    for (double value : values)
    {
        file << value << '\n';
    }

    file << '\n';
}

void Output::write_pvd() const
{
    std::ofstream file(directory + "/fields.pvd");

    file
        << "<?xml version=\"1.0\"?>\n";

    file
        << "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\">\n";

    file
        << "  <Collection>\n";

    for (const auto& entry : entries)
    {
        file
            << "    <DataSet timestep=\""
            << std::setprecision(16)
            << entry.time
            << "\" group=\"\" part=\"0\" file=\""
            << entry.filename
            << "\"/>\n";
    }

    file
        << "  </Collection>\n";

    file
        << "</VTKFile>\n";
}

void Output::write_interface(
    double time,
    double ell,
    double ell_dot)
{
    interface_file
        << std::setprecision(16)
        << time
        << '\t'
        << ell
        << '\t'
        << ell_dot
        << '\n';
}

void Output::write_interface_history(
    const std::vector<double>& time_history,
    const std::vector<double>& ell_history,
	int output_interval
)
{
    std::ofstream file(directory + "/interface_history.csv");

    file << "# time;ell\n";

    for(size_t i = 0; i < time_history.size(); ++i)
    {
        if(i % output_interval == 0)
        file << time_history[i]
             << ";"
             << ell_history[i]
             << "\n";
    }
}
