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
    serialized_grid.printPerformance();

    // Go over increasing number of threads for the parallelized grid
    do {
        threads *= 2;
        if (threads > max_threads) {
            threads = max_threads;
        }
        parallelized_grid.setThreads(threads);
        // Reach for equilibrium for the parallelized grid
        parallelized_grid.reachEquilibrium();
        parallelized_grid.printPerformance(serialized_grid.getElapsedTime());
        // Reset the parallelized grid
        parallelized_grid = grid;
    } while (threads < max_threads);

    return 0;
}