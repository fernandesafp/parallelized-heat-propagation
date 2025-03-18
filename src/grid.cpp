#include <iostream>
#include <omp.h>
#include "grid.hpp"

using namespace std;

#define AMBIENT_TEMPERATURE 300.0f     // Define the ambient temperature of 300 K
#define GRID_SIZE           0.01f      // Define the grid size of 1 cm
#define MAX_UPDATES         1e4        // Define the maximum number of updates
#define THERMAL_DIFFUSIVITY 1.22e-3f   // Define the thermal diffusivity for pyrolytic graphite
#define TIME_STEP           1.0f/30.0f // Define the time step for 15 frames per second

Grid::Grid(int resolution) : rows(resolution), cols(resolution) {
    printf("Initializing grid with resolution %dx%d.\n", resolution, resolution);
    printf("Setting ambient temperature to %.0f K.\n", AMBIENT_TEMPERATURE);
    temperatures.resize(resolution, vector<float>(resolution, AMBIENT_TEMPERATURE));
}

auto Grid::printGrid() const -> void {
    printf("\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%0*.0f ", 3, temperatures[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

auto Grid::setTemperature(int row, int col, float temperature) -> void{
    printf("Setting temperature at (%d, %d) to %.0f K.\n", row, col, temperature);
    temperatures[row][col] = temperature;
}

auto Grid::getTemperature(int row, int col) const -> float {
    return temperatures[row][col];
}

auto Grid::setThreads(int threads) -> void {
    this->threads = threads;
    omp_set_num_threads(threads);
}

auto Grid::getThreads() const -> int {
    return threads;
}

auto Grid::increaseStep() -> void {
    step++;
}

auto Grid::getStep() const -> int {
    return step;
}

auto Grid::startTimer() -> void {
    startTime = omp_get_wtime();
}

auto Grid::getTime() const -> double {
    return omp_get_wtime() - startTime;
}

auto Grid::updateGrid() -> bool {
    vector<vector<float>> newTemperatures = temperatures;
    float tempReference = temperatures[0][0];
    bool  reachedEquilibrium = true;
    startTimer();
    #pragma omp parallel for collapse(2) shared(reachedEquilibrium)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Initialize the sum of the temperatures of the neighbors
            float neighborTempSum = 0.0f;
            int   neighbors = 0;

            // Checking for all 8 possible neighbors
            if (i > 0) {
                neighborTempSum += temperatures[i - 1][j];
                neighbors++;
            }
            if (i < rows - 1) {
                neighborTempSum += temperatures[i + 1][j];
                neighbors++;
            }
            if (j > 0) {
                neighborTempSum += temperatures[i][j - 1];
                neighbors++;
            }
            if (j < cols - 1) {
                neighborTempSum += temperatures[i][j + 1];
                neighbors++;
            }
            if (i > 0 && j > 0) {
                neighborTempSum += temperatures[i - 1][j - 1];
                neighbors++;
            }
            if (i > 0 && j < cols - 1) {
                neighborTempSum += temperatures[i - 1][j + 1];
                neighbors++;
            }
            if (i < rows - 1 && j > 0) {
                neighborTempSum += temperatures[i + 1][j - 1];
                neighbors++;
            }
            if (i < rows - 1 && j < cols - 1) {
                neighborTempSum += temperatures[i + 1][j + 1];
                neighbors++;
            }

            // Calculate the new temperature
            float laplacian = (neighborTempSum - neighbors * temperatures[i][j]) / (GRID_SIZE * GRID_SIZE);

            // Update the temperature
            newTemperatures[i][j] = temperatures[i][j] + THERMAL_DIFFUSIVITY * TIME_STEP * laplacian;

            // Check if the grid has reached equilibrium
            float referenceDifference = abs(tempReference - temperatures[i][j]);
            if (referenceDifference > 1.0f) {
                reachedEquilibrium = false;
            }
      }
    }
    elapsedGridUpdates += getTime();
    if (reachedEquilibrium) {
        return true;
    } else {
        temperatures = newTemperatures;
        return false;
    }
}

auto Grid::reachEquilibrium() -> void {
    printf("Simulating heat transfer with %d threads...\n", getThreads());
    while (!updateGrid() && getStep() < MAX_UPDATES) {
        increaseStep();
    }
    if (getStep() == MAX_UPDATES) {
        printf("Maximum number of updates reached (%d).\n", MAX_UPDATES);
    } else {
        printf("Equilibrium reached after %d steps.\n", getStep());
    }
    printf("Finished.\n");
}

auto Grid::isEqual(const Grid& grid) -> void {
    int  rows_len = rows;
    int  cols_len = cols;
    startTimer();
    #pragma omp parallel for shared(rows_len,cols_len) collapse(2)
    for (int i = 0; i < rows_len; ++i) {
        for (int j = 0; j < cols_len; ++j) {
            if (temperatures[i][j] != grid.getTemperature(i, j)) {
                rows_len = -1;
                cols_len = -1;
            }
        }
    }
    getTime();
    printf("Grids are %s.\n", rows_len == rows ? "equal" : "different");
    printf("Elapsed time for comparison: %.2g seconds.\n", getTime());
}

auto Grid::printPerformance() const -> void {
    printf("\nPerformance for %d threads:\n", threads);
    printf("Elapsed time for temperature updates: %.2g seconds.\n", elapsedGridUpdates);
}
