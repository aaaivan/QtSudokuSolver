#include "bruteforcesolverthread.h"
#include "solver/BruteForceSolver.h"
#include <QDebug>

BruteForceSolverThread::BruteForceSolverThread(QObject *parent)
    : QThread{parent}
    , mGrid(nullptr)
    , mMaxSolutionsCount(0)
    , mUseHints(false)
    , mDisplaySolution(false)
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
    mBruteForceSolver = std::make_unique<BruteForceSolver>(this, grid, &mAbort);
    mSolverMutex = solverMutex;
    mGrid = grid;
}

void BruteForceSolverThread::run()
{
    mInputMutex.lock();
    size_t maxSolutionCount = mMaxSolutionsCount;
    bool useHints = mUseHints;
    bool displaySolution = mDisplaySolution;
    mInputMutex.unlock();

    mSolverMutex->lock();
    mBruteForceSolver->GenerateIncidenceMatrix(useHints);
    mSolverMutex->unlock();
    emit CalculationStarted();
    if(displaySolution)
    {
        mBruteForceSolver->FindSolution(maxSolutionCount);
    }
    else
    {
        mBruteForceSolver->CountSolutions(maxSolutionCount);
    }
    emit CalculationFinished();

    qDebug() << "Brute force finished!";
    if(mAbort)
    {
        NotifyGridChanged();
    }
}

void BruteForceSolverThread::CountSolutions(size_t maxSolutionCount, bool useHints)
{
    if(!isRunning())
    {
        QMutexLocker locker(&mInputMutex);
        mAbort = false;
        mDisplaySolution = false;
        mMaxSolutionsCount = maxSolutionCount;
        mUseHints = useHints;
        start(HighestPriority);
    }
}

void BruteForceSolverThread::DisplaySolution(size_t maxSolutionCount, bool useHints)
{
    if(!isRunning())
    {
        QMutexLocker locker(&mInputMutex);
        mAbort = false;
        mDisplaySolution = true;
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

void BruteForceSolverThread::NotifySolutionsCountReady(size_t count, bool stopped)
{
    emit NumberOfSolutionsComputed(count, stopped);
}

void BruteForceSolverThread::NotifySolutionReady(const std::vector<unsigned short> &solution)
{
    for (unsigned int i = 0; i < solution.size(); ++i)
    {
        emit CellUpdated(i, solution[i]);
    }
}

void BruteForceSolverThread::DisplayCandidatesForCell(CellId id)
{
    QMutexLocker locker(mSolverMutex);
    mGrid->NotifyCellChanged(id);
}

void BruteForceSolverThread::ResetGridContents()
{
    mSolverMutex->lock();
    unsigned short size = mGrid->SizeGet();
    mSolverMutex->unlock();

    for (unsigned int i = 0; i < size * size; ++i)
    {
        DisplayCandidatesForCell(i);
    }
}
