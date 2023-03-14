#include "SolvingTechnique.h"
#include "RegionUpdatesManager.h"
#include "Region.h"
#include "RegionsManager.h"
#include "SudokuGrid.h"
#include "SudokuCell.h"
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



const std::map<unsigned short, TechniqueType> FishTechnique::sFishSizeToTechnique{
    {2, TechniqueType::XWing},
    {3, TechniqueType::Swordfish},
    {4, TechniqueType::Jellyfish},
};

FishTechnique::FishTechnique(unsigned short fishSize, SudokuGrid* regionsManager, ObservedComponent observedComponent):
    SolvingTechnique(regionsManager, sFishSizeToTechnique.at(fishSize), observedComponent),
    mCurrentValue(0),
    mRegionsToSearch(),
    mAvailableRegions(),
    mAllowedRegions(),
    mCurrentRegion(nullptr),
    mFishSize(fishSize)
{
    if(mFishSize > mGrid->SizeGet() / 2 || mGrid->ParentNodeGet()) // don't use the fish while bifurcating... it's too slow
    {
        mFinished = true;
    }
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
        mCurrentRegion = nullptr;
    }

    if(mCurrentRegion == nullptr)
    {
        mAllowedRegions.clear();
        mAllowedRegions.insert(mAvailableRegions.begin(), mAvailableRegions.end());
        if(mRegionsToSearch.size() > 0)
        {
            mCurrentRegion = *mRegionsToSearch.begin();
        }
    }

    if(mCurrentValue <= mGrid->SizeGet())
    {
        if(mCurrentRegion)
        {
            auto it = mRegionsToSearch.find(mCurrentRegion);
            SearchFish();
            ++it;

            if(it != mRegionsToSearch.end())
            {
                mCurrentRegion = *it;
            }
            else
            {
                mCurrentRegion = nullptr;
                ++mCurrentValue;
                if(mCurrentValue <= mGrid->SizeGet())
                {
                    UpdateRegions();
                }
            }
        }
        else
        {
            mCurrentRegion = nullptr;
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
    mFinished = false;
    if(mFishSize > mGrid->SizeGet() / 2 || mGrid->ParentNodeGet()) // don't use the fish while bifurcating... it's too slow
    {
        mFinished = true;
    }
    mCurrentRegion = nullptr;
}

void FishTechnique::UpdateRegions()
{
    const RegionSet& regions = mGrid->RegionsManagerGet()->RegionsGet();
    mRegionsToSearch.clear();
    mAvailableRegions.clear();

    for (Region* r : regions)
    {
        if (r->UpdateManagerGet()->IsRegionReadyForTechnique(mType, mCurrentValue))
        {
            mRegionsToSearch.insert(r);
        }
        if (r->HasConfirmedValue(mCurrentValue))
        {
            mAvailableRegions.insert(r);
        }
    }
}




const unsigned int BifurcationTechnique::sMaxDepth = 3;

BifurcationTechnique::BifurcationTechnique(SudokuGrid *grid, ObservedComponent observedComponent, unsigned int depth, unsigned int maxDepth):
    SolvingTechnique(grid, TechniqueType::Bifurcation, observedComponent),
    mDepth(depth),
    mTargetDepth(maxDepth),
    mCells(),
    mCellOrder(),
    mOptionEliminationMatrix(),
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
    mCells.clear();
    mCellOrder.clear();
    mOptionEliminationMatrix.clear();
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

    std::stable_sort(mCells.begin(), mCells.end(), [](SudokuCell* const &a, SudokuCell* const &b){ return a->OptionsGet().size() < b->OptionsGet().size(); });
    for (unsigned int i = 0; i < mCells.size(); ++i)
    {
        mCellOrder[mCells.at(i)] = i;
    }

    mOptionEliminationMatrix.reserve(mCells.size());
    for (unsigned int i = 0; i < mCells.size(); ++i)
    {
        mOptionEliminationMatrix.push_back({});
        for (unsigned int j = 0; j < mCells.at(i)->OptionsGet().size(); ++j)
        {
            mOptionEliminationMatrix.back();
        }
    }
}

void BifurcationTechnique::CreateRootNode()
{
    mRoot.reset();
    mRoot = std::make_unique<RandomGuessTreeRoot>(mGrid, mCells.at(mCurrentIndex), this);
}
