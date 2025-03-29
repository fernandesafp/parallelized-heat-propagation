#include <iostream>
#include <omp.h>
#include "grid.hpp"

auto main() -> int {
    // Setting the grid resolution
    int   resolution = 1000;
    Grid  initial_grid(resolution);

    // Setting a temperature in the middle of the grid
    float temperature = 900.0f;
    int   position = resolution / 2;
    initial_grid.setTemperature(position, position, temperature);

    // Getting number of threads for the grids
    int    threads[] = {1, 2, 4, 8, 12, 16};
    int    max_threads = omp_get_max_threads();
    int    referenceThreads = -1;
    double referenceTime = -1.0f;

    // Iterate over the number of threads
    for (int t : threads) {
        if (t > max_threads) {
            continue;
        }
        Grid grid = initial_grid;
        grid.setThreads(t);
        // Reach for equilibrium for the parallelized grid
        grid.reachEquilibrium();
        // Print the performance of the grid
        grid.printPerformance(referenceThreads, referenceTime);
        // Store the reference values for the next iteration
        referenceThreads = grid.getThreads();
        referenceTime = grid.getElapsedTime();
    }

    return 0;
}