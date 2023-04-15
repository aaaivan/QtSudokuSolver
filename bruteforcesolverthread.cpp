#include "bruteforcesolverthread.h"
#include "solver/BruteForceSolver.h"
#include <QDebug>

BruteForceSolverThread::BruteForceSolverThread(QObject *parent)
    : QThread{parent}
    , mMaxSolutionsCount(0)
    , mUseHints(false)
    , mAbort(false)
    , mInputMutex()
    , mSolverMutex(nullptr)
{
}

BruteForceSolverThread::~BruteForceSolverThread()
{
    mInputMutex.lock();
    mAbort = true;
    mInputMutex.unlock();
    wait();
}

void BruteForceSolverThread::Init(SudokuGrid* grid, QMutex* solverMutex)
{
    mBruteForceSolver = std::make_unique<BruteForceSolver>(grid, &mAbort);
    mSolverMutex = solverMutex;
}

void BruteForceSolverThread::run()
{
    mInputMutex.lock();
    size_t maxSolutionCount = mMaxSolutionsCount;
    bool useHints = mUseHints;
    mInputMutex.unlock();

    mSolverMutex->lock();
    size_t solutions = mBruteForceSolver->GenerateSolutions(maxSolutionCount, useHints);
    mSolverMutex->unlock();

    qDebug() << "Brute force finished!";
    if(mAbort)
    {
        NotifyGridChanged();
    }
    else
    {
        emit NumberOfSolutionsComputed(solutions);
    }
}

void BruteForceSolverThread::CountSolutions(size_t maxSolutionCount, bool useHints)
{
    if(!isRunning())
    {
        QMutexLocker locker(&mInputMutex);
        mAbort = false;
        mMaxSolutionsCount = maxSolutionCount;
        mUseHints = useHints;
        start(HighestPriority);
    }
}

void BruteForceSolverThread::AbortCalculation()
{
    if(isRunning())
    {
        QMutexLocker locker(&mInputMutex);
        mAbort = true;
    }
}

void BruteForceSolverThread::NotifyGridChanged()
{
    mBruteForceSolver->DirtySolutions();
}

