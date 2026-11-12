#include "Grid.h"

Grid::Grid(int Nx_) : Nx(Nx_)
{
    dxi = 1.0 / (Nx - 1);

    xi.resize(Nx);

    for(int i = 0; i < Nx; ++i)
    {
        xi[i] = i * dxi;
    }
}
