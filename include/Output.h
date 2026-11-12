#pragma once

#include <fstream>
#include <string>
#include <vector>

class Grid;

class Output
{
public:

    explicit Output(const std::string& directory);

    void write(
        int step,
        double time,
        double ell,
        const Grid& grid,
        const std::vector<double>& concentration,
        const std::vector<double>& age,
        const std::vector<double>& diffusion);

    void write_interface(
        double time,
        double ell,
        double ell_dot);

    void write_interface_history(
        const std::vector<double>& time_history,
        const std::vector<double>& ell_history,
	int output_interval
    );

private:

    struct TimeEntry
    {
        double time;
        std::string filename;
    };

    std::string directory;

    std::ofstream interface_file;

    std::vector<TimeEntry> entries;

    void write_pvd() const;

    void write_scalar(
        std::ofstream& file,
        const std::string& name,
        const std::vector<double>& values) const;
};
