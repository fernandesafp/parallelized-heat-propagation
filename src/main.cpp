#include <iostream>
#include <omp.h>
#include "grid.hpp"

auto main() -> int {
    // Setting the grid resolution and temperature
    int resolution = 5;
    float temperature = 900.0f;

    // Creating the grid
    Grid grid(resolution);

    // Setting a temperature in the middle of the grid
    int center = resolution / 2;
    grid.setTemperature(center, center, temperature);

    Grid serialized_grid = grid;
    Grid parallelized_grid = grid;

    // Printing the initial grid
    printf("Initial grid:\n");
    grid.printGrid();

    // Reach for equilibrium through serial updates
    printf("Updating temperatures through serial updates...\n");
    serialized_grid.reachEquilibrium();
    printf("Finished.\n");

    // Resetting the grid
    //printf("Resetting the grid...\n");
    //grid = Grid(resolution);
    //grid.setTemperature(center, center, temperature);

    // Reach for equilibrium through parallel updates
    //int threads = omp_get_max_threads();
    //printf("Updating temperatures through parallel updates with %d threads...\n", threads);
    //grid.reachEquilibrium();

    // Checking if the grids resulted in the same temperature distribution
    //for (int i = 0; i < resolution; i++) {
    //    for (int j = 0; j < resolution; j++) {
    //        if (serialized_grid.getTemperature(i, j) != parallelized_grid.getTemperature(i, j)) {
    //            break;
    //        }
    //    }
    //}

    serialized_grid.printPerformance();
    //parallelized_grid.printPerformance();
    return 0;
}