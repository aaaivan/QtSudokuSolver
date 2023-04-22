#include "SolvingTechnique.h"
#include "SudokuCell.h"
#include "Region.h"
#include "GridProgressManager.h"
#include "RegionsManager.h"
#include "SudokuGrid.h"
#include <algorithm>
#include <iterator>
#include <map>

bool FishTechnique::IsFishValid(RegionList& definingRegion, const IntersectionMap& intersectionMap, RegionList& currentSet)
{
    CellSet secondaryCells;
    CellSet cannibalCells;
    for (Region* const& r : currentSet)
    {
        for (const auto& c : r->CellsWithValueGet(mCurrentValue))
        {
            if (!secondaryCells.insert(c).second && intersectionMap.count(c) > 0)
            {
                // the cells is in the defining set AND in more than one secondary set
                cannibalCells.insert(c);
            }
        }
    }

    CellSet fins;
    for (auto& entry : intersectionMap)
    {
        if (secondaryCells.count(entry.first) == 0)
        {
            // this cell is in the defining set but not in the secondary set
            // therefore it is a fin.
            fins.insert(entry.first);
        }
    }
    if (fins.size() > 0)
    {
        // not all cells in the defining set are included in the secondary set
        CellSet cellsSeeingFins;
        CellSet cannibalCellsSeeingFins;

        mGrid->RegionsManagerGet()->FindConnectedCellsWithValue(fins, cellsSeeingFins, mCurrentValue);
        // remove from cellsSeeingFins all cells that are NOT in the secondary set
        for (auto it = cellsSeeingFins.begin(), end = cellsSeeingFins.end(); it != end;)
        {
            if (secondaryCells.count(*it) == 0)
            {
                it = cellsSeeingFins.erase(it);
            }
            else
            {
                ++it;
            }
        }
        // remove from cellsSeeingFins all cells that are in the defining set
        if (cellsSeeingFins.size() > 0)
        {
            for (auto& entry : intersectionMap)
            {
                if (cellsSeeingFins.erase(entry.first) > 0)
                {
                    // check whether the cell we are removing is a cannibal cell
                    if (cannibalCells.count(entry.first) > 0)
                    {
                        cannibalCellsSeeingFins.insert(entry.first);
                    }
                }
            }
        }
        if (cannibalCellsSeeingFins.size() > 0)
        {
            mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_CannibalFinnedFish>(definingRegion, currentSet, std::move(fins), std::move(cellsSeeingFins), std::move(cannibalCellsSeeingFins), mCurrentValue));
            return true;
        }
        else if (cellsSeeingFins.size() > 0)
        {
            mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_FinnedFish>(definingRegion, currentSet, std::move(fins), std::move(cellsSeeingFins), mCurrentValue));
            return true;
        }
    }
    else
    {
        // At this point we are sure that all cells in the defining set are included in the secondary set
        // and therefore we have a legitimate fish. Cannibal cells can be checked now.
        if (cannibalCells.size() > 0)
        {
            CellSet definingCells;
            for (const auto& entry : intersectionMap)
            {
                definingCells.insert(entry.first);
            }
            mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_CannibalFish>(definingRegion, std::move(definingCells), std::move(currentSet), std::move(cannibalCells), mCurrentValue));
            return true;
        }
        else if (secondaryCells.size() > intersectionMap.size())
        {
            // is there at least one cell in the secondary set that is not in the defining set?
            // if not, this fish is pointless as it would not lead to any elimination
            CellSet definingCells;
            for (const auto& entry : intersectionMap)
            {
                definingCells.insert(entry.first);
            }

            mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_Fish>(definingRegion, std::move(definingCells), std::move(currentSet), mCurrentValue));
            return true;
        }
    }
    return false;
}

bool FishTechnique::SearchSecondaryFishRegionInner(RegionList& definingRegion, const IntersectionMap& intersectionMap,
    IntersectionMap::const_iterator& mapIt, RegionList& currentSet,
    CellList& fins, RegionSet& finsRegions, CellSet& cellsSeeingFins, bool& impossible)
{
    if (impossible) // stop searching and notify the progress manager
    {
        return false;
    }
    else if (currentSet.size() == mCurrentSize)
    {
        return IsFishValid(definingRegion, intersectionMap, currentSet);
    }
    else if (mapIt != intersectionMap.end())
    {
        // check whether the current cell pointed at by mapIt is included in one of the regions
        // that have already been added to the secondary set. If so, move to the next cell.
        bool prune = false;
        for (Region* const& r : currentSet)
        {
            if (mapIt->second.count(r) > 0)
            {
                prune = true;
                break;
            }
        }
        if (prune)
        {
            bool result = SearchSecondaryFishRegionInner(definingRegion, intersectionMap, ++mapIt, currentSet, fins, finsRegions, cellsSeeingFins, impossible);
            --mapIt;
            return result;
        }
        else
        {
            // try and add each region intersecting the current cell to the secondary set
            for (Region* r : mapIt->second)
            {
                if (fins.size() > 0)
                {
                    // skip this regions if it contains a fin
                    if (finsRegions.count(r) > 0) continue;

                    // skip this region if none of its cells sees a fin
                    bool skip = true;
                    for (const auto& c : r->CellsWithValueGet(mCurrentValue))
                    {
                        if (cellsSeeingFins.count(c) > 0)
                        {
                            skip = false;
                            break;
                        }
                    }
                    if (skip) continue;
                }

                currentSet.push_back(r);
                bool result = SearchSecondaryFishRegionInner(definingRegion, intersectionMap, ++mapIt, currentSet, fins, finsRegions, cellsSeeingFins, impossible);
                --mapIt;

                if (result)
                {
                    return true;
                }

                // Current guess did not work out. Backtrack.
                currentSet.pop_back();
            }

            // we could not find a fish containing the cell pointed at by mapIt->first
            // keep going to find out whether it can be a fin
            fins.push_back(mapIt->first);

            CellSet tempCellsSeeingFins = cellsSeeingFins;
            mGrid->RegionsManagerGet()->FindConnectedCellsWithValue(fins, cellsSeeingFins, mCurrentValue);
            if (cellsSeeingFins.empty()) return false;

            RegionSet tempFinsRegions = finsRegions;
            finsRegions.insert(mapIt->second.begin(), mapIt->second.end());

            bool result = SearchSecondaryFishRegionInner(definingRegion, intersectionMap, ++mapIt, currentSet, fins, finsRegions, cellsSeeingFins, impossible);

            // backtrack
            --mapIt;
            finsRegions = tempFinsRegions;
            cellsSeeingFins = tempCellsSeeingFins;
            fins.pop_back();
            return result;
        }
    }
    return false;
}

bool FishTechnique::SearchSecondaryFishRegion(bool& impossible)
{
    RegionList definingSet;

    // The following map is used to map each fishable cell to the secondary regions containg it
    IntersectionMap intersectionMapCells;
    // The following map is used to map each region in the defining set to the secondary regions who share at least a fish cell
    std::map<Region*, RegionSet> intersectionMapRegions;

    // Find all the cells in the defining set that can accomodate value.
    // None of these cell can be shared between two or more regions of the defining set
    for (const auto& rIt : *mCurrentSet)
    {
        definingSet.push_back(*rIt);
        for (SudokuCell* const& c : (*rIt)->CellsWithValueGet(mCurrentValue))
        {
            // the intersection between any two regions in the defining set
            // cannot include any cell that can accomodate value.
            if (intersectionMapCells.count(c) > 0)
            {
                return false;
            }
            else
            {
                intersectionMapCells[c] = mGrid->RegionsManagerGet()->RegionsWithCellGet(c);
                // regions in the defining set can't be in the secondary set
                for (auto& r : *mCurrentSet)
                {
                    intersectionMapCells.at(c).erase(*r);
                }

                // update the map
                for (Region* const& secReg : intersectionMapCells.at(c))
                {
                    if (intersectionMapRegions.count(secReg) == 0)
                    {
                        intersectionMapRegions[secReg] = RegionSet();
                    }
                    intersectionMapRegions.at(secReg).insert(*rIt);
                }
            }
        }
    }
    RegionSet regionsToRemove;
    // we want to keep only the secondary regions that intersect two or more defining regions
    for (const auto& secReg : intersectionMapRegions)
    {
        if (secReg.second.size() < 2)
        {
            regionsToRemove.insert(secReg.first);
        }
    }
    if (regionsToRemove.size() > 0)
    {
        for (auto& defCell : intersectionMapCells)
        {
            for (Region* const& r : regionsToRemove)
            {
                defCell.second.erase(r);
            }
        }
    }

    // Value can go in too many cells to make it possible to find a fish
    if (intersectionMapCells.size() >= mGrid->SizeGet() * mCurrentSize)
    {
        return false;
    }

    RegionList outSecondarySet;
    CellList fins;
    RegionSet finsRegions;
    CellSet cellsSeeingFins;
    auto mapIt = intersectionMapCells.cbegin();
    // searh for a secondary set that incorporates all the fish cells
    return SearchSecondaryFishRegionInner(definingSet, intersectionMapCells, mapIt, outSecondarySet, fins, finsRegions, cellsSeeingFins, impossible);
}

void FishTechnique::GetPossibeDefiningRegionsInner(std::list<DefininfSet>& definingSets, RegListIt regIt, DefininfSet& nextSet)
{
    if (nextSet.size() == mCurrentSize) // the defining set has the desired size. We are done
    {
        definingSets.emplace_back(nextSet.begin(), nextSet.end());
    }
    else
    {
        std::set<SudokuCell*> cells;
        for (const auto& r : nextSet)
        {
            const auto& c = (*r)->CellsWithValueGet(mCurrentValue);
            cells.insert(c.begin(), c.end());
        }
        // make sure the number of regions that have yet to be processed is at least as big
        // as the number of regions missing to reach the desired set size
        while (std::distance(regIt, mAvailableRegions.end()) >= (int)mCurrentSize - (int)nextSet.size())
        {
            bool prune = false;
            for (const auto& c : (*regIt)->CellsWithValueGet(mCurrentValue))
            {
                if(cells.count(c) > 0)
                {
                    prune = true;
                    break;
                }
            }
            if(prune)
            {
                ++regIt;
                continue;
            }
            nextSet.push_back(regIt);
            ++regIt;
            GetPossibeDefiningRegionsInner(definingSets, regIt, nextSet);
            nextSet.pop_back();
        }
    }
}

void FishTechnique::GetPossibeDefiningRegions()
{
    std::list<DefininfSet> newDefiningSets;
    for (auto& oldSet : mDefiningSets)
    {
        RegListIt startIt = oldSet.back();
        // Recursively get all possible defining sets
        GetPossibeDefiningRegionsInner(newDefiningSets, ++startIt, oldSet);
    }
    mDefiningSets.clear();
    mDefiningSets = std::move(newDefiningSets);
}

void FishTechnique::SearchFish()
{
    bool impossible = false;
    if (SearchSecondaryFishRegion(impossible))
    {
        return;
    }
    else if (impossible)
    {
        return;
    }
}

void FishTechnique::NotifyFailure()
{
    size_t count = 0;
    for (Region* r : mAvailableRegions)
    {
        if(count >= mRegionsToSearchCount)
            break;

        mGrid->ProgressManagerGet()->RegisterFailure(TechniqueType::Fish, r, nullptr, mCurrentValue);
        ++count;
    }
}
