#ifndef BRUTEFORCESOLVER_H
#define BRUTEFORCESOLVER_H

// Includes
#include "Types.h"

typedef unsigned int CellId;
typedef std::pair<CellId, unsigned short> Possibility; // cell-candidate pair

class BruteForceSolver
{
    SudokuGrid* mGrid;           // refernce to the grid
    bool mUseHintsAsConstraints; // whether the hints should be used as constraints
    bool mSolutionsDirty;        // whether the grid has changed since the last time the solutions were computed
    size_t mMaxSolutionCount;     // max number of solutions to search
    const bool* mAbort;

    std::list<std::vector<unsigned short>> mSolutions;            // list of possible solutions

public:
    BruteForceSolver(SudokuGrid* grid, bool* abortFlag);

private:
    Possibility PossibilityFromRowIndex(size_t row);
    size_t IndexFromPossibility(CellId id, unsigned short value);
    void GenerateIncidenceMatrix();
    void FillIncidenceMatrix(bool** M, const size_t rows);

public:
    void DirtySolutions();
    size_t GenerateSolutions(size_t maxSolutionsCount, bool useHints);
};

#endif // BRUTEFORCESOLVER_H
