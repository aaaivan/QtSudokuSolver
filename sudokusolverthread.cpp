#include "sudokusolverthread.h"
#include "solver/GridProgressManager.h"
#include "solver/SudokuCell.h"
#include "solver/VariantConstraints.h"
#include "solver/BruteForceSolver.h"
#include <QDebug>

SudokuSolverThread::SudokuSolverThread(unsigned short gridSize, QObject *parent)
    : QThread{parent},
      mGrid(nullptr),
      mBruteForceSolver(),
      mPuzzleData(gridSize),
      mGivensToAdd(),
      mHintsToAdd(),
      mRegionsToAdd(),
      mAddPositiveDiagonal(false),
      mAddNegativeDiagonal(false),
      mReloadCells(false),
      mReloadGrid(false),
      mNewInput(false),
      mAbort(false),
      mPaused(false),
      mInputMutex(),
      mSolverMutex(),
      mThreadCondition()
{
}

SudokuSolverThread::~SudokuSolverThread()
{
    mInputMutex.lock();
    mSolverMutex.lock();
    mAbort = true;
    mThreadCondition.wakeOne();
    mSolverMutex.unlock();
    mInputMutex.unlock();
    wait();
}

void SudokuSolverThread::Init()
{
    mGrid = std::make_unique<SudokuGrid>(mPuzzleData.mSize, this);
    mBruteForceSolver = std::make_unique<BruteForceSolver>(mGrid.get());
}

void SudokuSolverThread::run()
{
    GridProgressManager* progressManager = mGrid->ProgressManagerGet();
    forever
    {
        // collect input
        mInputMutex.lock();
        const unsigned short gridSize = mPuzzleData.mSize;
        const PuzzleData puzzleData = mPuzzleData;
        std::set<CellCoord> newGivens;
        std::set<CellCoord> newHints;
        std::set<unsigned short> newRegions;
        std::set<CellCoord> newKillers;
        const bool positiveDiagonal = mAddPositiveDiagonal;
        const bool negativeDiagonal = mAddNegativeDiagonal;
        const bool reloadGrid = mReloadGrid;
        const bool reloadCells = mReloadCells;
        const bool newInput = mNewInput;

        if(!reloadCells)
        {
            newGivens = mGivensToAdd;
            newHints = mHintsToAdd;
        }
        if(!reloadGrid)
        {
            newRegions = mRegionsToAdd;
            newKillers = mKillersToAdd;
        }

        mGivensToAdd.clear();
        mHintsToAdd.clear();
        mRegionsToAdd.clear();
        mKillersToAdd.clear();
        mAddNegativeDiagonal = false;
        mAddPositiveDiagonal = false;
        mReloadGrid = false;
        mReloadCells = false;
        mNewInput = false;
        mInputMutex.unlock();

        if(newInput)
        {
            QMutexLocker locker(&mSolverMutex);
            mBruteForceSolver->DirtySolutions();

            if(reloadGrid)
            {
                mGrid->Clear();
            }
            // Clear given cells if necessary
            else if(reloadCells)
            {
                mGrid->ResetContents();
            }

            // define regions
            for (size_t i = 0; i < puzzleData.mRegions.size(); ++i)
            {
                const auto& region = puzzleData.mRegions.at(i);
                if(region.size() == 0) continue;

                if(reloadGrid || newRegions.count(i))
                {
                    std::vector<std::array<unsigned short, 2>> cells;
                    cells.reserve(region.size());
                    const auto pred = [&](const CellCoord &id) -> std::array<unsigned short, 2>
                    {
                        return {static_cast<unsigned short>(id / gridSize),
                                static_cast<unsigned short>(id % gridSize)};
                    };
                    std::transform(region.begin(), region.end(), std::back_inserter(cells), pred);
                    mGrid->DefineRegion(cells, cells.size() == gridSize ? RegionType::House_Region : RegionType::Generic_region);
                }
            }

            // define killers
            for (const auto& killer : puzzleData.mKillerCages)
            {
                if(reloadGrid || newKillers.count(killer.first))
                {
                    unsigned int killerSum = killer.second.first;
                    const auto& killerCells = killer.second.second;
                    std::vector<std::array<unsigned short, 2>> cells;
                    cells.reserve(killerCells.size());
                    const auto pred = [&](const CellCoord &id) -> std::array<unsigned short, 2>
                    {
                        return {static_cast<unsigned short>(id / gridSize),
                                static_cast<unsigned short>(id % gridSize)};
                    };
                    std::transform(killerCells.begin(), killerCells.end(), std::back_inserter(cells), pred);
                    mGrid->DefineRegion(cells, RegionType::KillerCage, new KillerConstraint(killerSum));
                }
            }

            // define negative diagonal
            if((reloadGrid && puzzleData.mNegativeDiagonal) ||
               (!reloadGrid && negativeDiagonal))
            {
                std::vector<std::array<unsigned short, 2>> cells = DiagonalCellsGet(gridSize, PuzzleData::Diagonal_Negative);
                mGrid->DefineRegion(cells, RegionType::Generic_region);
            }

            // define positive diagonal
            if((reloadGrid && puzzleData.mPositiveDiagonal) ||
               (!reloadGrid && positiveDiagonal))
            {
                std::vector<std::array<unsigned short, 2>> cells = DiagonalCellsGet(gridSize, PuzzleData::Diagonal_Positive);
                mGrid->DefineRegion(cells, RegionType::Generic_region);
            }

            // define givens
            for (const auto& given : puzzleData.mGivens)
            {
                if(reloadCells || newGivens.count(given.first))
                {
                    mGrid->AddGivenCell(given.first / gridSize, given.first % gridSize, given.second);
                }
            }

            // define hints
            for (const auto& hints : puzzleData.mHints)
            {
                if(reloadCells || newHints.count(hints.first))
                {
                    mGrid->SetCellEliminationHints(hints.first / gridSize, hints.first % gridSize, hints.second);
                }
            }
        }

        // solve
        while (!progressManager->HasFinished())
        {
            QMutexLocker locker(&mSolverMutex);
            if(mNewInput || mPaused)
            {
                break;
            }
            if(mAbort)
            {
                QMutexLocker locked(&mInputMutex);
                mAbort = false;
                return;
            }
            progressManager->NextStep();
        }

        // we exited the solve loop. There can be two reasons:
        // 1- the solver has finished (sudoku solved or run out of techniques)
        // 2- there is a new user input to be processed
        mInputMutex.lock();
        if(mAbort)
        {
            mAbort = false;
            return;
        }
        if(!mNewInput)
        {
            // pause this thread while there is nothing to do
            mThreadCondition.wait(&mInputMutex);
        }
        mInputMutex.unlock();
    }
}

void SudokuSolverThread::AddGivenValueToSubmissionQueue(CellCoord cell)
{
    if(mReloadGrid || mReloadCells)
        return;

    mGivensToAdd.insert(cell);
}

void SudokuSolverThread::AddHintToSubmissionQueue(CellCoord cell)
{
    if(mReloadGrid || mReloadCells)
        return;

    mHintsToAdd.insert(cell);
}

void SudokuSolverThread::AddRegionToSubmissionQueue(unsigned short index)
{
    if(mReloadGrid)
        return;

    mRegionsToAdd.insert(index);
}

void SudokuSolverThread::AddKillerToSubmissionQueue(CellCoord id)
{
    if(mReloadGrid)
        return;

    mKillersToAdd.insert(id);
}

void SudokuSolverThread::AddDiagonalToSubmissionQueue(PuzzleData::Diagonal diagonal)
{
    if(mReloadGrid)
        return;

    if(diagonal == PuzzleData::Diagonal_Negative)
    {
        mAddNegativeDiagonal = true;
    }
    else if (diagonal == PuzzleData::Diagonal_Positive)
    {
        mAddPositiveDiagonal = true;
    }
}

void SudokuSolverThread::ReloadCells()
{
    mReloadCells = true;
}

void SudokuSolverThread::ReloadGrid()
{
    mReloadGrid = true;
    mReloadCells = true;
}

std::vector<std::array<unsigned short, 2>> SudokuSolverThread::DiagonalCellsGet(unsigned short gridSize, PuzzleData::Diagonal diagonal) const
{
    std::vector<std::array<unsigned short, 2>> cells;
    cells.reserve(gridSize);

    if(diagonal == PuzzleData::Diagonal_Negative)
    {
        for (unsigned short i = 0; i < gridSize; ++i)
        {
            cells.push_back({i, i});
        }
    }
    else if(diagonal == PuzzleData::Diagonal_Positive)
    {
        for (unsigned short i = 0; i < gridSize; ++i)
        {
            cells.push_back({i, static_cast<unsigned short>(gridSize - 1 - i)});
        }
    }

    return cells;
}

void SudokuSolverThread::SubmitChangesToSolver()
{
    QMutexLocker locker(&mInputMutex);

    mNewInput = true;
    if (!isRunning())
    {
        start(HighestPriority);
    }
    else
    {
        mThreadCondition.wakeOne();
    }
}

void SudokuSolverThread::NotifyCellChanged(SudokuCell *cell)
{
    emit CellUpdated(cell->IdGet(), cell->OptionsGet());
}

void SudokuSolverThread::SetAutoSolverPaused(bool paused)
{
    QMutexLocker locker(&mInputMutex);
    if(mPaused != paused)
    {
        mPaused = paused;
        if(!mPaused && !isRunning())
        {
            mThreadCondition.wakeOne();
        }
    }
}

void SudokuSolverThread::CountSolutions(size_t maxSolutionCount, bool useHints)
{
    QMutexLocker locker(&mSolverMutex);

    size_t solutions = mBruteForceSolver->GenerateSolutions(maxSolutionCount, useHints);
    emit NumberOfSolutionsComputed(solutions);
}
