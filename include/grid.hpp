#ifndef GRID_HPP
#define GRID_HPP

#include <vector>

using namespace std;

class Grid {
    private:
        int    rows, cols, step{0}, threads;
        double startTime, elapsedGridUpdates;
        vector<vector<float>> temperatures;
    public:
        Grid(int resolution);
        void   printGrid() const;
        void   setTemperature(int row, int col, float temperature);
        float  getTemperature(int row, int col) const;
        void   setThreads(int threads);
        int    getThreads() const;
        void   increaseStep();
        int    getStep() const;
        void   startTimer();
        double getTime() const;
        bool   updateGrid();
        void   reachEquilibrium();
        void   isEqual(const Grid& grid);
        void   printPerformance() const;
};

#endif