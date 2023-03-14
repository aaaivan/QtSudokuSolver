#include "GridProgressManager.h"
#include "Progress.h"
#include "Region.h"
#include "RegionUpdatesManager.h"
#include "SudokuGrid.h"

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

    mTechniques[static_cast<size_t>(TechniqueType::XWing)] =
        std::make_unique<FishTechnique>(2, mSudokuGrid, SolvingTechnique::ObserveValues);

    mTechniques[static_cast<size_t>(TechniqueType::Swordfish)] =
        std::make_unique<FishTechnique>(3, mSudokuGrid, SolvingTechnique::ObserveValues);

    mTechniques[static_cast<size_t>(TechniqueType::Jellyfish)] =
        std::make_unique<FishTechnique>(4, mSudokuGrid, SolvingTechnique::ObserveValues);

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
#if PRINT_LOG_MESSAGES
    printf("Technique %u failed on region:", static_cast<size_t>(type));
    printf("%s, ", region->IdGet().c_str());
    printf("\n");
#endif // PRINT_LOG_MESSAGES
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

        if(mSudokuGrid->IsSolved())
        {
            mFinished = true;
        }
    }
    else if (!mProgressQueue.empty())
    {
        Reset();
        mProgressQueue.front()->ProcessProgress();
        mProgressQueue.pop();

        if(mSudokuGrid->IsSolved())
        {
            mFinished = true;
        }
    }
    else if (!mAbort && !mFinished)
    {
        NextTechnique();
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
