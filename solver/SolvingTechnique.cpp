#include "SolvingTechnique.h"
#include "RegionUpdatesManager.h"
#include "Region.h"
#include "RegionsManager.h"
#include "SudokuGrid.h"
#include "SudokuCell.h"
#include "GridProgressManager.h"
#include <algorithm>

SolvingTechnique::SolvingTechnique(SudokuGrid* grid, TechniqueType type, ObservedComponent observedComponent):
    mType(type),
    mObservedComponent(observedComponent),
    mGrid(grid),
    mFinished(false)
{
}

TechniqueType SolvingTechnique::TypeGet() const
{
    return mType;
}

bool SolvingTechnique::HasFinished() const
{
    return mFinished;
}

SolvingTechnique::ObservedComponent SolvingTechnique::ObservedComponentGet() const
{
    return mObservedComponent;
}




LockedCandidatesTechnique::LockedCandidatesTechnique(SudokuGrid* regionsManager, ObservedComponent observedComponent):
    SolvingTechnique(regionsManager, TechniqueType::LockedCandidates, observedComponent),
    mCurrentRegion(nullptr)
{
}

void LockedCandidatesTechnique::NextStep()
{
    if (HasFinished())
    {
        return;
    }

    const RegionSet& regions = mGrid->RegionsManagerGet()->RegionsGet();
    RegionSet::iterator it;

    if (!mCurrentRegion)
    {
        it = regions.begin();
        mCurrentRegion = *it;
    }
    else
    {
        it = regions.find(mCurrentRegion);
    }

    std::set<unsigned short> hotValues;
    if (mCurrentRegion->UpdateManagerGet()->IsRegionReadyForTechnique(mType, hotValues))
    {
        for (const auto v : hotValues)
        {
            SearchLockedCandidates(v);
        }
    }

    ++it;
    if (it != regions.end())
    {
        mCurrentRegion = *it;
    }
    else
    {
        mCurrentRegion = nullptr;
        mFinished = true;
    }
}

void LockedCandidatesTechnique::Reset()
{
    mCurrentRegion = nullptr;
    mFinished = false;
}




NakedSubsetTechnique::NakedSubsetTechnique(SudokuGrid* regionsManager, ObservedComponent observedComponent):
    SolvingTechnique(regionsManager, TechniqueType::NakedSubset, observedComponent),
    mCurrentRegion(nullptr)
{
}

void NakedSubsetTechnique::NextStep()
{
    if (HasFinished())
    {
        return;
    }

    const RegionSet& regions = mGrid->RegionsManagerGet()->RegionsGet();
    RegionSet::iterator it;

    if (!mCurrentRegion)
    {
        it = regions.begin();
        mCurrentRegion = *it;
    }
    else
    {
        it = regions.find(mCurrentRegion);
    }

    CellSet hotCells;
    if (mCurrentRegion->UpdateManagerGet()->IsRegionReadyForTechnique(mType, hotCells))
    {
        SearchNakedSubsets(hotCells);
    }

    ++it;
    if (it != regions.end())
    {
        mCurrentRegion = *it;
    }
    else
    {
        mCurrentRegion = nullptr;
        mFinished = true;
    }
}

void NakedSubsetTechnique::Reset()
{
    mCurrentRegion = nullptr;
    mFinished = false;
}




HiddenNakedSubsetTechnique::HiddenNakedSubsetTechnique(SudokuGrid* regionsManager, ObservedComponent observedComponent) :
    SolvingTechnique(regionsManager, TechniqueType::HiddenNakedSubset, observedComponent),
    mCurrentRegion(nullptr)
{
}

void HiddenNakedSubsetTechnique::NextStep()
{
    if (HasFinished())
    {
        return;
    }

    const RegionSet& regions = mGrid->RegionsManagerGet()->RegionsGet();
    RegionSet::iterator it;

    if (!mCurrentRegion)
    {
        it = regions.begin();
        mCurrentRegion = *it;
    }
    else
    {
        it = regions.find(mCurrentRegion);
    }

    std::set<unsigned short> hotValues;
    if (mCurrentRegion->UpdateManagerGet()->IsRegionReadyForTechnique(mType, hotValues))
    {
        SearchHiddenNakedSubsets(hotValues);
    }

    ++it;
    if (it != regions.end())
    {
        mCurrentRegion = *it;
    }
    else
    {
        mCurrentRegion = nullptr;
        mFinished = true;
    }
}

void HiddenNakedSubsetTechnique::Reset()
{
    mCurrentRegion = nullptr;
    mFinished = false;
}




FishTechnique::FishTechnique(SudokuGrid* regionsManager, ObservedComponent observedComponent):
    SolvingTechnique(regionsManager, TechniqueType::Fish, observedComponent),
    mCurrentValue(0),
    mAvailableRegions(),
    mCurrentRegion(),
    mRegionsToSearchCount(0),
    mCurrentRegionIndex(0),
    mMinSize(2),
    mCurrentSize(mMinSize)
{
    mFinished = (mGrid->ParentNodeGet() != nullptr);
}

void FishTechnique::NextStep()
{
    if (HasFinished())
    {
        return;
    }

    if (mCurrentValue == 0)
    {
        mCurrentValue = 1;
        UpdateRegions();
        mCurrentSize = 0;
    }

    if(mCurrentSize == 0)
    {
        mCurrentSize = mMinSize;
        mCurrentRegionIndex = mRegionsToSearchCount;
    }

    if(mCurrentRegionIndex == mRegionsToSearchCount)
    {
        if(mRegionsToSearchCount > 0)
        {
            mCurrentRegionIndex = 0;
            mCurrentRegion = mAvailableRegions.begin();
        }
    }

    if(mCurrentValue <= mGrid->SizeGet())
    {
        if(mCurrentRegionIndex < mRegionsToSearchCount && mCurrentSize <= mGrid->SizeGet() / 2)
        {
            SearchFish();
            ++mCurrentRegionIndex;

            if(mCurrentRegionIndex == mRegionsToSearchCount)
            {
                ++mCurrentSize;
            }
        }
        else
        {
            NotifyFailure();
            mCurrentSize = 0;
            ++mCurrentValue;
            if(mCurrentValue <= mGrid->SizeGet())
            {
                UpdateRegions();
            }
        }
    }
    else
    {
        mFinished = true;
    }
}

void FishTechnique::Reset()
{
    mCurrentValue = 0;
    mCurrentSize = 0;
    mFinished = (mGrid->ParentNodeGet() != nullptr);
    mRegionsToSearchCount = 0;
    mRegionsToSearchCount = 0;
}

void FishTechnique::UpdateRegions()
{
    const RegionSet& regions = mGrid->RegionsManagerGet()->RegionsGet();
    mAvailableRegions.clear();
    mRegionsToSearchCount = 0;

    for (Region* r : regions)
    {
        bool willBeSearched = false;
        if (r->UpdateManagerGet()->IsRegionReadyForTechnique(mType, mCurrentValue))
        {
            if(r->CellsWithValueGet(mCurrentValue).size() < mGrid->SizeGet())
            {
                mAvailableRegions.push_front(r);
                willBeSearched = true;
                mRegionsToSearchCount++;
            }
            else
            {
                mGrid->ProgressManagerGet()->RegisterFailure(TechniqueType::Fish, r, nullptr, mCurrentValue);
            }
        }
        if (r->HasConfirmedValue(mCurrentValue) &&
            r->CellsWithValueGet(mCurrentValue).size() >= 2)
        {
            if(!willBeSearched)
            {
                mAvailableRegions.push_back(r);
            }
        }
    }
}




const unsigned int BifurcationTechnique::sMaxDepth = 3;

BifurcationTechnique::BifurcationTechnique(SudokuGrid *grid, ObservedComponent observedComponent, unsigned int depth, unsigned int maxDepth):
    SolvingTechnique(grid, TechniqueType::Bifurcation, observedComponent),
    mDepth(depth),
    mTargetDepth(maxDepth),
    mBifurcationGrid(),
    mCells(),
    //mOptionEliminationMatrix(),
    mCurrentIndex(0),
    mRoot()
{
    if(mDepth >= mTargetDepth && mGrid->ParentNodeGet())
    {
        mFinished = true;
    }
    else if(!mGrid->ParentNodeGet() && mTargetDepth >= sMaxDepth)
    {
        mFinished = true;
    }
}

unsigned int BifurcationTechnique::DepthGet() const
{
    return mDepth;
}

unsigned int BifurcationTechnique::TargetDepthGet() const
{
    return mTargetDepth;
}

void BifurcationTechnique::NextStep()
{
    if (HasFinished())
    {
        return;
    }

    if(mCells.size() == 0)
    {
        Init();
        if(mCells.size() == 0)
        {
            mFinished = true;
            return;
        }
        mCurrentIndex = 0;
        CreateRootNode();
    }

    if(!mRoot->HasFinished())
    {
        mRoot->NextStep();
    }
    else
    {
        mCurrentIndex++;
        if(mCurrentIndex < mCells.size())
        {
            CreateRootNode();
        }
        else if(mGrid->ParentNodeGet())
        {
            mFinished = true;
        }
        else if(mTargetDepth >= sMaxDepth)
        {
            mFinished = true;
        }
        else
        {
            ++mTargetDepth;
            mCurrentIndex = 0;
            CreateRootNode();
        }
    }
}

void BifurcationTechnique::Reset()
{
    mFinished = false;
    if(mDepth >= mTargetDepth && mGrid->ParentNodeGet())
    {
        mFinished = true;
    }
    else if(!mGrid->ParentNodeGet())
    {
        mTargetDepth = 1;
    }
    mBifurcationGrid.reset();
    mCells.clear();
    //mOptionEliminationMatrix.clear();
    mCurrentIndex = 0;
    mRoot.reset();
}

void BifurcationTechnique::Init()
{
    unsigned int gridSize = mGrid->SizeGet();
    mCells.reserve(gridSize * gridSize);

    for (size_t i = 0; i < gridSize; i++)
    {
        for (size_t j = 0; j < gridSize; j++)
        {
            SudokuCell* c = mGrid->CellGet(i, j);
            if(!c->IsSolved())
            {
                mCells.push_back(c);
            }
        }
    }

    std::stable_sort(mCells.begin(), mCells.end(), [](SudokuCell* const &a, SudokuCell* const &b)
    {
        return a->OptionsGet().size() < b->OptionsGet().size();
    });

    mBifurcationGrid = std::make_unique<SudokuGrid>(mGrid);
}

void BifurcationTechnique::CreateRootNode()
{
    mRoot.reset();
    mRoot = std::make_unique<RandomGuessTreeRoot>(mGrid, mBifurcationGrid.get(), mCells[mCurrentIndex]->IdGet(), this);
}
