#include <iostream>
#include <omp.h>
#include "grid.hpp"

using namespace std;

// Define the maximum number of updates
#define MAX_UPDATES 100000

Grid::Grid(int resolution) : rows(resolution), cols(resolution) {
    temperatures.resize(resolution, vector<float>(resolution, 0.0f));
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

auto Grid::checkEquilibrium() -> bool {
    int rows_len = rows;
    int cols_len = cols;
    startTimer();
    #pragma omp parallel for shared(rows_len,cols_len) collapse(2)
    for (int i = 1; i < rows_len; ++i) {
        for (int j = 1; j < cols_len; ++j) {
            float diff = temperatures[i][j] - temperatures[0][0];
            if (diff > 0.1f || diff < -0.1f) {
                rows_len = -1;
                cols_len = -1;
            }
        }
    }
    elapsedEquilibriumChecks += getTime();
    return rows_len == rows;
}

auto Grid::updateGrid() -> void {
    vector<vector<float>> newTemperatures = temperatures;
    startTimer();
    #pragma omp parallel for collapse(2)
    for (int i = 1; i < rows - 1; ++i) {
        for (int j = 1; j < cols - 1; ++j) {
            if (newTemperatures[i][j] != 0.0f) {
                newTemperatures[i][j] -= 1.0f;
            }
        }
    }
    elapsedGridUpdates += getTime();
    temperatures = newTemperatures;
}

auto Grid::reachEquilibrium() -> void {
    printf("Reaching equilibrium with %d threads...\n", getThreads());
    while (!checkEquilibrium() && getStep() < MAX_UPDATES) {
        updateGrid();
        increaseStep();
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
    printf("\nPerformance:\n");
    printf("Threads used: %d.\n", threads);
    printf("Resolution: %dx%d.\n", rows, cols);
    if (getStep() == MAX_UPDATES) {
        printf("Maximum number of updates reached of %d.\n", MAX_UPDATES);
    } else {
        printf("Equilibrium reached after %d steps.\n", getStep());
    }
    printf("Elapsed time for equilibrium checks:  %.2g seconds.\n", elapsedEquilibriumChecks);
    printf("Elapsed time for temperature updates: %.2g seconds.\n", elapsedGridUpdates);
}
