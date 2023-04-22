#include "GridProgressManager.h"
#include "Progress.h"
#include "Region.h"
#include "RegionUpdatesManager.h"
#include "SudokuGrid.h"
#include "../sudokusolverthread.h"

GridProgressManager::GridProgressManager(SudokuGrid* sudoku) :
    mSudokuGrid(sudoku),
    mProgressQueue(),
    mTechniques(static_cast<size_t>(TechniqueType::MAX_TECHNIQUES)),
    mCurrentTechnique(static_cast<TechniqueType>(0)),
    mFinished(false),
    mAbort(false)
{
    mTechniques[static_cast<size_t>(TechniqueType::LockedCandidates)] =
        std::make_unique<LockedCandidatesTechnique>(mSudokuGrid, SolvingTechnique::ObserveValues);

    mTechniques[static_cast<size_t>(TechniqueType::NakedSubset)] =
        std::make_unique<NakedSubsetTechnique>(mSudokuGrid, SolvingTechnique::ObserveCells);

    mTechniques[static_cast<size_t>(TechniqueType::HiddenNakedSubset)] =
        std::make_unique<HiddenNakedSubsetTechnique>(mSudokuGrid, SolvingTechnique::ObserveValues);

    mTechniques[static_cast<size_t>(TechniqueType::Fish)] =
        std::make_unique<FishTechnique>(mSudokuGrid, SolvingTechnique::ObserveValues);

    unsigned int depth = 0;
    unsigned int targetDepth = 1;
    const SudokuGrid* parent = mSudokuGrid->ParentNodeGet();
    if(parent)
    {
        const auto& technique = parent->ProgressManagerGet()->mTechniques[static_cast<size_t>(TechniqueType::Bifurcation)];
        BifurcationTechnique* bifurcation = static_cast<BifurcationTechnique*>(technique.get());
        depth = bifurcation->DepthGet() + 1;
        targetDepth = bifurcation->TargetDepthGet();
    }
    mTechniques[static_cast<size_t>(TechniqueType::Bifurcation)] =
        std::make_unique<BifurcationTechnique>(mSudokuGrid, SolvingTechnique::ObserveNothing, depth, targetDepth);
}

const SolvingTechnique* GridProgressManager::TechniqueGet(TechniqueType type) const
{
    return mTechniques[static_cast<size_t>(type)].get();
}

bool GridProgressManager::HasFinished() const
{
    return  mAbort ||
            (
                mFinished &&
                mHighPriorityProgressQueue.empty() &&
                mProgressQueue.empty()
                );
}

bool GridProgressManager::HasAborted() const
{
    return mAbort;
}

void GridProgressManager::RegisterProgress(std::shared_ptr<Progress>&& deduction)
{
    if (deduction->isHighPriotity())
    {
        mHighPriorityProgressQueue.push(deduction);
    }
    else
    {
        mProgressQueue.push(deduction);
    }
}

void GridProgressManager::RegisterFailure(TechniqueType type, Region* region, SudokuCell* cell /* = nullptr */, unsigned short value /* = 0 */)
{
    if (value)
    {
        region->UpdateManagerGet()->OnTechniqueFailed(type, value);
    }
    else if (cell)
    {
        region->UpdateManagerGet()->OnTechniqueFailed(type, cell);
    }
    else
    {
        region->UpdateManagerGet()->OnTechniqueFailed(type);
    }
}

void  GridProgressManager::NextStep()
{
    if(mAbort)
    {
        return;
    }

    if (!mHighPriorityProgressQueue.empty())
    {
        Reset();
        mHighPriorityProgressQueue.front()->ProcessProgress();
        mHighPriorityProgressQueue.pop();
    }
    else if (!mProgressQueue.empty())
    {
        Reset();
        mProgressQueue.front()->ProcessProgress();
        mProgressQueue.pop();
    }
    else if (!mAbort && !mFinished)
    {
        NextTechnique();
    }

    if(mSudokuGrid->IsSolved())
    {
        while(!mHighPriorityProgressQueue.empty())
        {
            Reset();
            mHighPriorityProgressQueue.front()->ProcessProgress();
            mHighPriorityProgressQueue.pop();
        }
        while(!mProgressQueue.empty())
        {
            Reset();
            mProgressQueue.front()->ProcessProgress();
            mProgressQueue.pop();
        }
        mFinished = true;

        if(mSudokuGrid->ParentNodeGet() == nullptr)
        {
            mSudokuGrid->SolverThreadGet()->NotifyLogicalDeduction("Puzzle Solved!");
        }
    }
}

void GridProgressManager::Clear()
{
    {
        std::queue<std::shared_ptr<Progress>> empty;
        std::swap(mProgressQueue, empty);
    }
    {
        std::queue<std::shared_ptr<Progress>> empty;
        std::swap(mHighPriorityProgressQueue, empty);
    }
    mAbort = false;
    Reset();
}

void GridProgressManager::Reset()
{
    mCurrentTechnique = static_cast<TechniqueType>(0);
    mFinished = false;
    for (const auto& t : mTechniques)
    {
        t->Reset();
    }
}

void GridProgressManager::Abort()
{
    mAbort = true;
    mFinished = true;
}

void GridProgressManager::NextTechnique()
{
    size_t index = static_cast<size_t>(mCurrentTechnique);
    mTechniques.at(index)->NextStep();
    if (mTechniques.at(index)->HasFinished())
    {
        ++index;
        mCurrentTechnique = static_cast<TechniqueType>(index);
        if (index >= mTechniques.size())
        {
            mFinished = true;
        }
    }
}
