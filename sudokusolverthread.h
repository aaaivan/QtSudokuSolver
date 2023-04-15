#ifndef SUDOKUSOLVERTHREAD_H
#define SUDOKUSOLVERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "puzzledata.h"
#include "solver/SudokuGrid.h"
#include "bruteforcesolverthread.h"

class SudokuCell;

class SudokuSolverThread : public QThread
{
    Q_OBJECT
public:
    explicit SudokuSolverThread(unsigned short gridSize, QObject *parent = nullptr);
    ~SudokuSolverThread();

    void Init();

signals:
    void CellUpdated(unsigned short id, const std::set<unsigned short>& content);
    void PuzzleHasNoSolution(std::string message);

protected:
    void run() override;

private:
    std::unique_ptr<SudokuGrid> mGrid;
    PuzzleData mPuzzleData;
    std::unique_ptr<BruteForceSolverThread> mBruteForceSolver;

    std::set<CellCoord> mGivensToAdd;
    std::set<CellCoord> mHintsToAdd;
    std::set<unsigned short> mRegionsToAdd;
    std::set<CellCoord> mKillersToAdd;
    bool mAddPositiveDiagonal;
    bool mAddNegativeDiagonal;

    bool mReloadCells;
    bool mReloadGrid;
    bool mNewInput;
    bool mAbort;
    bool mPaused;

    QMutex mInputMutex;
    QMutex mSolverMutex;
    QWaitCondition mThreadCondition;

    void AddGivenValueToSubmissionQueue(CellCoord cell);
    void AddHintToSubmissionQueue(CellCoord cell);
    void AddRegionToSubmissionQueue(unsigned short index);
    void AddKillerToSubmissionQueue(CellCoord id);
    void AddDiagonalToSubmissionQueue(PuzzleData::Diagonal diagonal);
    void ReloadCells();
    void ReloadGrid();

    std::vector<std::array<unsigned short, 2>> DiagonalCellsGet(unsigned short gridSize, PuzzleData::Diagonal diagonal) const;
public:
    bool HasPositiveDiagonalConstraint() const;
    bool HasNegativeDiagonalConstraint() const;
    bool HasKillerCage(CellCoord id) const;
    bool IsSameKillerCage(CellCoord id, unsigned int otherTotal, const CellsInRegion &cells) const;
    std::set<unsigned short> HintsGet(CellCoord id) const;

    void SetRegion(unsigned short regionId, const std::set<CellCoord> &cells);
    void PositiveDiagonalConstraintSet(bool set);
    void NegativeDiagonalConstraintSet(bool set);
    void AddGiven(unsigned short value, CellCoord cellId);
    void RemoveGiven(CellCoord cellId);
    void AddKillerCage(CellCoord cageId, unsigned int total, const CellsInRegion &cells);
    void RemoveKillerCage(CellCoord cageId);
    void AddHint(CellCoord cellId, unsigned short value);
    void RemoveHint(CellCoord cellId, unsigned short value);
    void RemoveAllHints();

    void SubmitChangesToSolver();
    void NotifyCellChanged(SudokuCell* cell);

    BruteForceSolverThread* BruteSolverGet() const;
    void SetLogicalSolverPaused(bool paused);
};

#endif // SUDOKUSOLVERTHREAD_H
