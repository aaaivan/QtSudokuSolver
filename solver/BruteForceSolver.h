#ifndef BRUTEFORCESOLVER_H
#define BRUTEFORCESOLVER_H

// Includes
#include "Types.h"

typedef unsigned int CellId;
typedef std::pair<CellId, unsigned short> Possibility; // cell-candidate pair

class BruteForceSolverThread;

class BruteForceSolver
{
    SudokuGrid* mGrid;           // reference to the grid
    BruteForceSolverThread* mBruteForceThread;

    bool mUseHintsAsConstraints; // whether the hints should be used as constraints
    bool mSolutionsDirty;        // whether the grid has changed since the last time the solutions were computed
    size_t mMaxSolutionCount;     // max number of solutions to search
    const bool* mAbort;

    std::list<std::vector<unsigned short>> mSolutions;            // list of possible solutions
    std::list<std::vector<unsigned short>>::iterator mSolutionIt;

public:
    BruteForceSolver(BruteForceSolverThread* bruteForceThread, SudokuGrid* grid, bool* abortFlag);

private:
    Possibility PossibilityFromRowIndex(size_t row);
    size_t IndexFromPossibility(CellId id, unsigned short value);
    void GenerateIncidenceMatrix();
    void FillIncidenceMatrix(bool** M, const size_t rows);

public:
    void DirtySolutions();
    void CountSolutions(size_t maxSolutionsCount, bool useHints);
    void FindSolution(size_t maxSolutionsCount, bool useHints);
};

#endif // BRUTEFORCESOLVER_H
