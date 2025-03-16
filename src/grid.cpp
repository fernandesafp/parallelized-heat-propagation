#include <iostream>
#include <omp.h>
#include "grid.hpp"

using namespace std;

// Define the maximum number of updates
#define MAX_UPDATES 100000

Grid::Grid(int resolution) : rows(resolution), cols(resolution) {
    temperatures.resize(resolution, vector<float>(resolution, 0.0f));
}

auto Grid::setTemperature(int row, int col, float temperature) -> void{
    temperatures[row][col] = temperature;
}

auto Grid::getTemperature(int row, int col) const -> float {
    return temperatures[row][col];
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

auto Grid::increaseStep() -> void {
    step++;
}

auto Grid::getStep() const -> int {
    return step;
}

auto Grid::startTimer() -> void {
    startTime = omp_get_wtime();
}

auto Grid::stopTimer() -> void {
    endTime = omp_get_wtime();
    elapsedTime = endTime - startTime;
}
auto Grid::checkEquilibrium() const -> bool {
    for (int i = 1; i < rows; i++) {
        for (int j = 1; j < cols; j++) {
            float diff = temperatures[i][j] - temperatures[0][0];
            if (diff > 0.1f || diff < -0.1f) {
                return false;
            }
        }
    }
    return true;
}

auto Grid::updateGrid() -> void {
    vector<vector<float>> newTemperatures = temperatures;
    for (int i = 1; i < rows - 1; ++i) {
        for (int j = 1; j < cols - 1; ++j) {
            // todo: update the temperature of the cell
        }
    }
    temperatures = newTemperatures;
}

auto Grid::printPerformance() const -> void {
    printf("Threads used: %d\n", omp_get_num_threads());
    if (getStep() == MAX_UPDATES) {
        printf("Maximum number of updates reached of %d\n", MAX_UPDATES);
    } else {
        printf("Equilibrium reached after %d steps\n", getStep());
    }
    printf("Resolution: %dx%d\n", rows, cols);
    printf("Execution time: %.3f seconds\n", endTime - startTime);
}

auto Grid::reachEquilibrium() -> void {
    startTime = omp_get_wtime();
    while (!checkEquilibrium() && getStep() < MAX_UPDATES) {
        updateGrid();
        increaseStep();
    }
    endTime = omp_get_wtime();
}
