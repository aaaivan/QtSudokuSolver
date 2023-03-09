#ifndef SUDOKUSOLVERTHREAD_H
#define SUDOKUSOLVERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "puzzledata.h"

class SudokuSolverThread : public QThread
{
    Q_OBJECT
public:
    explicit SudokuSolverThread(QObject *parent = nullptr);
    ~SudokuSolverThread();

signals:
    void UpdateGrid();
    void PuzzleHasNoSolution(std::string message);

protected:
    void run() override;

private:
    PuzzleData mPuzzleData;

    std::vector<CellCoord> mGivensToAdd;
    std::vector<CellCoord> mHintsToAdd;
    std::vector<unsigned short> mRegionsToAdd;
    std::vector<CellCoord> mKillersToAdd;
    std::vector<PuzzleData::Diagonal> mDiagonalsToAdd;

    bool mClearCells;
    bool mClearGrid;
    bool mAbort;

    QMutex mMutex;
    QWaitCondition mThreadCondition;

public:
    void AddGivenValue(CellCoord cell);
    void AddHint(CellCoord cell);
    void AddRegion(unsigned short index);
    void AddKiller(CellCoord id);
    void AddDiagonal(PuzzleData::Diagonal diagonal);
    void ClearCells(const PuzzleData &puzzle);
    void ClearGrid(const PuzzleData &puzzle);
    void SubmitChangesToSolver();
};

#endif // SUDOKUSOLVERTHREAD_H
