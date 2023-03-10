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

signals:
    void CellUpdated(unsigned short id, const std::set<unsigned short>& content);
    void PuzzleHasNoSolution(std::string message);

protected:
    void run() override;

private:
    SudokuGrid mGrid;
    PuzzleData mPuzzleData;

    std::set<CellCoord> mGivensToAdd;
    std::set<CellCoord> mHintsToAdd;
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
    void AddDiagonalToSubmissionQueue(PuzzleData::Diagonal diagonal);
    void ReloadCells();
    void ReloadGrid();

    std::vector<std::array<unsigned short, 2>> DiagonalCellsGet(unsigned short gridSize, PuzzleData::Diagonal diagonal) const;
public:
    unsigned short CellCountInRegion(unsigned short regionId) const;
    bool HasPositiveDiagonalConstraint() const;
    bool HasNegativeDiagonalConstraint() const;
    std::pair<unsigned int, CellsInRegion> KillerCageGet(CellCoord id) const;
    std::set<unsigned short> HintsGet(CellCoord id) const;

    void AddCellToRegion(unsigned short regionId, CellCoord cellId);
    void RemoveCellFromRegion(unsigned short regionId, CellCoord cellId);
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
