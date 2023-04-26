#include "SolvingTechnique.h"
#include "RegionUpdatesManager.h"
#include "Region.h"
#include "RegionsManager.h"
#include "SudokuGrid.h"
#include "SudokuCell.h"
#include "GridProgressManager.h"
#include "GhostCagesManager.h"
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
    mCurrentRegion(nullptr),
    mProcessingGhostRegions(false)
{
}

void LockedCandidatesTechnique::NextStep()
{
    if (HasFinished())
    {
        return;
    }

    const RegionSet& regions = mProcessingGhostRegions ? mGrid->GhostRegionsManagerGet()->RegionsGet()
                                                       : mGrid->RegionsManagerGet()->RegionsGet();
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

    if(it != regions.end())
    {
        std::set<unsigned short> hotValues;
        if (mCurrentRegion->UpdateManagerGet()->IsRegionReadyForTechnique(mType, hotValues))
        {
            for (const auto v : hotValues)
            {
                SearchLockedCandidates(v);
            }
        }
        ++it;
    }


    if (it != regions.end())
    {
        mCurrentRegion = *it;
    }
    else if(!mProcessingGhostRegions)
    {
        mProcessingGhostRegions = true;
        mCurrentRegion = nullptr;
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
    mProcessingGhostRegions = false;
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




HiddenSubsetTechnique::HiddenSubsetTechnique(SudokuGrid* regionsManager, ObservedComponent observedComponent) :
    SolvingTechnique(regionsManager, TechniqueType::HiddenSubset, observedComponent),
    mCurrentRegion(nullptr)
{
}

void HiddenSubsetTechnique::NextStep()
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
        SearchHiddenSubsets(hotValues);
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

void HiddenSubsetTechnique::Reset()
{
    mCurrentRegion = nullptr;
    mFinished = false;
}




FishTechnique::FishTechnique(SudokuGrid* regionsManager, ObservedComponent observedComponent):
    SolvingTechnique(regionsManager, TechniqueType::Fish, observedComponent),
    mCurrentValue(0),
    mAvailableRegions(),
    mRegionsToSearchCount(0),
    mMinSize(2),
    mCurrentSize(mMinSize),
    mDefiningSets(),
    mCurrentSet()
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
        GetPossibeDefiningRegions();
        mCurrentSet = mDefiningSets.end();
    }

    if(mCurrentSet == mDefiningSets.end())
    {
        if(mRegionsToSearchCount > 0)
        {
            mCurrentSet = mDefiningSets.begin();
        }
    }

    if(mCurrentValue <= mGrid->SizeGet())
    {
        if(mCurrentSet != mDefiningSets.end() && mCurrentSize <= mGrid->SizeGet() / 2)
        {
            SearchFish();
            ++mCurrentSet;

            if(mCurrentSet == mDefiningSets.end())
            {
                ++mCurrentSize;
                GetPossibeDefiningRegions();
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
            else
            {
                mFinished = true;
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
    mAvailableRegions.clear();
    mDefiningSets.clear();
}

void FishTechnique::UpdateRegions()
{
    const RegionSet& regions = mGrid->RegionsManagerGet()->RegionsGet();
    mAvailableRegions.clear();
    mDefiningSets.clear();
    mRegionsToSearchCount = 0;

    for (Region* r : regions)
    {
        bool willBeSearched = false;
        if (r->UpdateManagerGet()->IsRegionReadyForTechnique(mType, mCurrentValue))
        {
            if(r->HasConfirmedValue(mCurrentValue) &&
               r->CellsWithValueGet(mCurrentValue).size() < mGrid->SizeGet())
            {
                mAvailableRegions.push_front(r);
                mDefiningSets.push_front({mAvailableRegions.begin()});
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





InniesAndOuties::InniesAndOuties(SudokuGrid *grid, ObservedComponent observedComponent):
    SolvingTechnique(grid, TechniqueType::InniesOuties, observedComponent),
    mRegions(),
    mCurrentRegion(),
    mCurrentRegionTotal(0),
    mContainedKillers(),
    mIntersectingKillers(),
    mKillerCombinations(),
    mKillerUnions(),
    mInnieCages(),
    mOutieCages()
{
    // do not use this technique if there are no killer cages
    mFinished = mGrid->RegionsManagerGet()->StartingRegionsGet()[static_cast<size_t>(RegionType::KillerCage)].size() == 0;
}

void InniesAndOuties::NextStep()
{
    if(HasFinished())
    {
        return;
    }

    if(mRegions.empty())
    {
        BuildMaps();
        if(mRegions.size() == 0)
        {
            mFinished = true;
            return;
        }
        mCurrentRegion = mRegions.begin();
        mCurrentRegionTotal = (*mCurrentRegion)->SumGet();
    }

    if(mContainedKillers.count(*mCurrentRegion) > 0)
    {
        SearchInnies();
    }
    if(mIntersectingKillers.count(*mCurrentRegion) > 0)
    {
        SearchOuties();
    }

    mCurrentRegion++;
    if(mCurrentRegion == mRegions.end())
    {
        for(KillerCage_t k : mInnieCages)
        {
            mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_GhostCage>(std::move(k.second), k.first, true, mGrid));
        }
        for(KillerCage_t k : mOutieCages)
        {
            mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_GhostCage>(std::move(k.second), k.first, false, mGrid));
        }
        mFinished = true;
    }
    else
    {
        mCurrentRegionTotal = (*mCurrentRegion)->SumGet();
    }
}

void InniesAndOuties::Reset()
{
    // do not use this technique if there are no killer cages
    mFinished = mGrid->RegionsManagerGet()->StartingRegionsGet()[static_cast<size_t>(RegionType::KillerCage)].size() == 0;
    mRegions.clear();
    mIntersectingKillers.clear();
    mContainedKillers.clear();
    mKillerCombinations.clear();
    mKillerUnions.clear();
    mInnieCages.clear();
    mOutieCages.clear();
}

void InniesAndOuties::BuildMaps()
{
    mRegions.clear();
    mContainedKillers.clear();
    mIntersectingKillers.clear();
    mKillerCombinations.clear();
    mKillerUnions.clear();

    const auto& regionManager = mGrid->RegionsManagerGet();
    const auto& ghostRegionManager = mGrid->GhostRegionsManagerGet();
    const auto& leafReg = regionManager->RegionsGet();
    const auto& ghostLeafReg = ghostRegionManager->RegionsGet();

    for (const auto r : leafReg)
    {
        FillMapEntry(r);
    }

    for (const auto r : ghostLeafReg)
    {
        FillMapEntry(r);
    }
}

void InniesAndOuties::FillMapEntry(Region *r)
{
    const auto& regionManager = mGrid->RegionsManagerGet();
    const KillerConstraint* k = static_cast<const KillerConstraint*>(r->GetConstraintByType(RegionType::KillerCage));
    if(k)
    {
        RegionSet containingRegions;
        regionManager->RegionsWithCellsGet(containingRegions, r->CellsGet());
        for (const auto& cr : containingRegions)
        {
            if(cr->SumGet() == 0) continue;
            if(cr == k->RegionGet()) continue;

            if(mContainedKillers.count(cr) == 0)
            {
                mContainedKillers[cr] = {};
            }
            mContainedKillers[cr].insert(k);
        }

        for (const auto& c : r->CellsGet())
        {
            const auto& intersectingRegions = regionManager->RegionsWithCellGet(c);
            for (const auto& ir : intersectingRegions)
            {
                if(ir->SumGet() == 0) continue;
                if(ir == k->RegionGet()) continue;

                mRegions.insert(ir);
                if(mIntersectingKillers.count(ir) == 0)
                {
                    mIntersectingKillers[ir] = {};
                }
                mIntersectingKillers[ir].insert(k);
            }
        }
    }
}


