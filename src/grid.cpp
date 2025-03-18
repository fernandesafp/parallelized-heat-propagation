#include <iostream>
#include <omp.h>
#include "grid.hpp"

using namespace std;

#define AMBIENT_TEMPERATURE 300.0f     // Define the ambient temperature of 300 K
#define GRID_SIZE           1e-2f      // Define the grid size of 1 cm
#define MAX_UPDATES         1e4        // Define the maximum number of updates
#define THERMAL_DIFFUSIVITY 1.22e-3f   // Define the thermal diffusivity for pyrolytic graphite
#define TIME_STEP           1.0f/30.0f // Define the time step for 30 frames per second

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

auto Grid::stopTimer() const -> double {
    return omp_get_wtime() - startTime;
}

auto Grid::getElapsedTime() const -> double {
    return totalElapsedTime;
}

auto Grid::updateGrid() -> bool {
    vector<vector<float>> newTemperatures = temperatures;
    float tempReference = temperatures[0][0];
    bool  reachedEquilibrium = true;
    int   directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},          { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1},
    };
    startTimer();
    #pragma omp parallel for collapse(2) shared(reachedEquilibrium)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Initialize the sum of the temperatures of the neighbors
            float neighborTempSum = 0.0f;
            int   neighbors = 0;

            // Check for all 8 possible neighbors
           for (int k = 0; k < 8; k++) {
                int new_i = i + directions[k][0];
                int new_j = j + directions[k][1];
                if (new_i >= 0 && new_i < rows && new_j >= 0 && new_j < cols) {
                    neighborTempSum += temperatures[new_i][new_j];
                    neighbors++;
                }
            }

            // Calculate the new temperature
            float laplacian = (neighborTempSum - neighbors * temperatures[i][j]) / (GRID_SIZE * GRID_SIZE);

            // Update the temperature
            newTemperatures[i][j] = temperatures[i][j] + THERMAL_DIFFUSIVITY * TIME_STEP * laplacian;

            // Check if the grid has reached equilibrium
            if (reachedEquilibrium) {
                float referenceDifference = abs(tempReference - temperatures[i][j]);
                if (referenceDifference > 1.0f) {
                    reachedEquilibrium = false;
                }
            }
        }
    }
    totalElapsedTime += stopTimer();
    temperatures = newTemperatures;
    return reachedEquilibrium;
}

auto Grid::reachEquilibrium() -> void {
    int threads = getThreads();
    printf("Simulating heat transfer with %d thread%s...\n", threads, threads > 1 ? "s" : "");
    while (!updateGrid() && getStep() < MAX_UPDATES) {
        increaseStep();
    }
    if (threads > 1) return;
    if (getStep() == MAX_UPDATES) {
        printf("Maximum number of updates reached (%d).\n", MAX_UPDATES);
    } else {
        printf("Equilibrium reached after %d steps.\n", getStep());
    }
}

auto Grid::printPerformance() const -> void {
    printf("Elapsed time for %d thread%s: ", threads, threads > 1 ? "s" : "");
    printf("%.2g seconds.\n", totalElapsedTime);
}

auto Grid::printPerformance(double referenceTime) const -> void {
    printPerformance();
    printf("Time improvement: %.0f%%.\n", (referenceTime - totalElapsedTime) / referenceTime * 100.0);
}
