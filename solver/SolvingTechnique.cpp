#include "SolvingTechnique.h"
#include "RegionUpdatesManager.h"
#include "Region.h"
#include "RegionsManager.h"
#include "SudokuGrid.h"

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
    mRegionsToSearch(),
    mAvailableRegions(),
    mMinSize(2),
    mCurrentSize(mMinSize)
{
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
    }

    if (mCurrentSize < mGrid->SizeGet() / 2)
    {
        SearchFish();
        ++mCurrentSize;
    }
    else
    {
        NotifyFailure();
        mCurrentSize = mMinSize;
        ++mCurrentValue;

        if (mCurrentValue > mGrid->SizeGet())
        {
            mFinished = true;
        }
        else
        {
            UpdateRegions();
        }
    }
}

void FishTechnique::Reset()
{
    mCurrentValue = 0;
    mCurrentSize = mMinSize;
    mFinished = false;
}

void FishTechnique::UpdateRegions()
{
    const RegionSet& regions = mGrid->RegionsManagerGet()->RegionsGet();
    mRegionsToSearch.clear();
    mAvailableRegions.clear();

    auto HasAtLeastTwoCells = [=](Region* r) {return r->CellsWithValueGet(mCurrentValue).size() >= 2; };

    for (Region* r : regions)
    {
        if (r->UpdateManagerGet()->IsRegionReadyForTechnique(mType, mCurrentValue))
        {
            mRegionsToSearch.insert(r);
        }
        if (r->HasConfirmedValue(mCurrentValue) && HasAtLeastTwoCells(r))
        {
            mAvailableRegions.insert(r);
        }
    }
}

