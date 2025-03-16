#ifndef GRID_HPP
#define GRID_HPP

#include <vector>

using namespace std;

class Grid {
    private:
        vector<vector<float>> temperatures;
        int rows, cols, step{0};
        double equilibriumChecksStart, equilibriumChecksEnd, updatesStart, updatesEnd;
        double elapsedTimeEquilibriumChecks, elapsedGridUpdates;
    public:
        Grid(int rows, int cols);
        Grid(int resolution);
        void  setTemperature(int row, int col, float temperature);
        float getTemperature(int row, int col) const;
        void  printGrid() const;
        void  increaseStep();
        int   getStep() const;
        bool  checkEquilibrium() const;
        void  updateGrid();
        void  reachEquilibrium();
        void  printPerformance() const;
};

#endif