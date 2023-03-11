#ifndef SUDOKUSOLVERTHREAD_H
#define SUDOKUSOLVERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "puzzledata.h"
#include "solver/SudokuGrid.h"

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

    std::set<CellCoord> mGivensToAdd;
    std::set<CellCoord> mHintsToAdd;
    std::set<unsigned short> mRegionsToAdd;
    bool mAddPositiveDiagonal;
    bool mAddNegativeDiagonal;

    bool mReloadCells;
    bool mReloadGrid;
    bool mNewInput;
    bool mAbort;

    QMutex mMutex;
    QWaitCondition mThreadCondition;

    void AddGivenValueToSubmissionQueue(CellCoord cell);
    void AddHintToSubmissionQueue(CellCoord cell);
    void AddRegionToSubmissionQueue(unsigned short index);
    void AddDiagonalToSubmissionQueue(PuzzleData::Diagonal diagonal);
    void ReloadCells();
    void ReloadGrid();

    std::vector<std::array<unsigned short, 2>> DiagonalCellsGet(unsigned short gridSize, PuzzleData::Diagonal diagonal) const;
public:
    bool HasPositiveDiagonalConstraint() const;
    bool HasNegativeDiagonalConstraint() const;
    std::pair<unsigned int, CellsInRegion> KillerCageGet(CellCoord id) const;
    std::set<unsigned short> HintsGet(CellCoord id) const;

    void SetRegion(unsigned short regionId, const std::set<CellCoord> &cells);
    void PositiveDiagonalConstraintSet(bool set);
    void NegativeDiagonalConstraintSet(bool set);
    void AddGiven(unsigned short value, CellCoord cellId);
    void RemoveGiven(CellCoord cellId);
    void AddCellToKillerCage(CellCoord cageId, CellCoord cellId);
    void RemoveCellFromKillerCage(CellCoord cageId, CellCoord cellId);
    void AddKillerCage(CellCoord cageId, const std::pair<unsigned int, CellsInRegion>& cageContent);
    void RemoveKillerCage(CellCoord cageId);
    void KillerCageTotalSet(CellCoord cageId, CellCoord cellId);
    void AddHint(CellCoord cellId, unsigned short value);
    void RemoveHint(CellCoord cellId, unsigned short value);
    void RemoveAllHints();

    void SubmitChangesToSolver();
    void NotifyCellChanged(SudokuCell* cell);
};

#endif // SUDOKUSOLVERTHREAD_H
