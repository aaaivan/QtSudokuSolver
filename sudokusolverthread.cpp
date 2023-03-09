#include "sudokusolverthread.h"

SudokuSolverThread::SudokuSolverThread(QObject *parent)
    : QThread{parent},
      mPuzzleData(0),
      mGivensToAdd(),
      mHintsToAdd(),
      mRegionsToAdd(),
      mKillersToAdd(),
      mDiagonalsToAdd(),
      mClearCells(false),
      mClearGrid(false),
      mAbort(false),
      mMutex(),
      mThreadCondition()
{
}

SudokuSolverThread::~SudokuSolverThread()
{
    mMutex.lock();
    mAbort = true;
    mThreadCondition.wakeOne();
    mMutex.unlock();
    wait();
}

void SudokuSolverThread::run()
{

}

void SudokuSolverThread::AddGivenValue(CellCoord cell)
{
    QMutexLocker locker(&mMutex);
    mGivensToAdd.push_back(cell);
}

void SudokuSolverThread::AddHint(CellCoord cell)
{
    QMutexLocker locker(&mMutex);
    mHintsToAdd.push_back(cell);
}

void SudokuSolverThread::AddRegion(unsigned short index)
{
    QMutexLocker locker(&mMutex);
    mRegionsToAdd.push_back(index);
}

void SudokuSolverThread::AddKiller(CellCoord id)
{
    QMutexLocker locker(&mMutex);
    mKillersToAdd.push_back(id);
}

void SudokuSolverThread::AddDiagonal(PuzzleData::Diagonal diagonal)
{
    QMutexLocker locker(&mMutex);
    mDiagonalsToAdd.push_back(diagonal);
}

void SudokuSolverThread::ClearCells(const PuzzleData &puzzle)
{
    QMutexLocker locker(&mMutex);
    mPuzzleData = puzzle;
    mClearCells = true;
}

void SudokuSolverThread::ClearGrid(const PuzzleData &puzzle)
{
    QMutexLocker locker(&mMutex);
    mPuzzleData = puzzle;
    mClearGrid = true;
    mClearCells = true;
}

void SudokuSolverThread::SubmitChangesToSolver()
{
    QMutexLocker locker(&mMutex);
    if (!isRunning())
    {
        start(HighestPriority);
    }
    else
    {
        mThreadCondition.wakeOne();
    }
}
