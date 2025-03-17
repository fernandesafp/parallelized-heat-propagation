#include <iostream>
#include <omp.h>
#include "grid.hpp"

auto main() -> int {
    // Setting the grid resolution
    int   resolution = 1000;
    Grid  grid(resolution);

    // Setting a temperature in the middle of the grid
    float temperature = 900.0f;
    int   position = resolution / 2;
    grid.setTemperature(position, position, temperature);

    // Creating two copies of the grid
    Grid serialized_grid = grid;
    Grid parallelized_grid = grid;

    // Getting number of threads for the grids
    int threads = 1;
    int max_threads = omp_get_max_threads();

    // Reach for equilibrium for the first grid
    serialized_grid.setThreads(threads);
    serialized_grid.reachEquilibrium();

    // Get max number of threads
    parallelized_grid.setThreads(max_threads);
    // Reach for equilibrium for the second grid
    parallelized_grid.reachEquilibrium();

    // Check if both grids are equal
    serialized_grid.isEqual(parallelized_grid);

    // Print the performance of both grids
    serialized_grid.printPerformance();
    parallelized_grid.printPerformance();

    return 0;
}