#ifndef GRID_PROGRESS_MANAGER_H
#define GRID_PROGRESS_MANAGER_H

#include "Types.h"
#include "Progress.h"
#include "SolvingTechnique.h"
#include <queue>
#include <functional>

/// <summary>
/// Deals with notifying relevant Regions when new progress
/// has been made on the grid.
/// </summary>
class GridProgressManager
{
    SudokuGrid* mSudokuGrid;
    std::queue<std::shared_ptr<Progress>> mProgressQueue;
    std::queue<std::shared_ptr<Progress>> mHighPriorityProgressQueue;

    std::vector<std::unique_ptr<SolvingTechnique>> mTechniques;
    std::vector<bool> mTechniqueActive;
    TechniqueType mCurrentTechnique;
    bool mFinished;
    bool mAbort;

public:
// Constructors

    GridProgressManager(SudokuGrid* sudoku);

// Const methods
    const SolvingTechnique* TechniqueGet(TechniqueType type) const;
    bool HasFinished() const;
    bool HasAborted() const;

// Non-const methods

    void RegisterProgress(std::shared_ptr<Progress>&& deduction);
    void RegisterFailure(TechniqueType type, Region* region, SudokuCell* cell = nullptr, unsigned short value = 0);
    void NextStep();

    /// <summary>
    /// Clear the progress queues
    /// </summary>
    void Clear();

    /// <summary>
    /// Reset the current region and the current techniques
    /// So that the solver can go back and check from the first technique
    /// </summary>
    void Reset();

    /// <summary>
    /// Tells the solver to stop computing a solution. This is ususally because the
    /// puzzle is impossible.
    /// </summary>
    void Abort();

    /// <summary>
    /// Enable/disable a technique
    /// </summary>
    void TechniqueActiveSet(TechniqueType t, bool enable);
private:
    void NextTechnique();
};


#endif // !GRID_PROGRESS_MANAGER_H
