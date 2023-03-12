#include "RegionsManager.h"
#include "SudokuGrid.h"
#include "Region.h"
#include "SudokuCell.h"
#include "RegionUpdatesManager.h"
#include <iterator>
#include <algorithm>
#include <cassert>

RegionsManager::RegionsManager(SudokuGrid* parentGrid):
    mStartingRegions(),
    mLeafRegions(),
    mCellToRegionsMap(),
    mParentGrid(parentGrid)
{
    for (size_t i = 0; i < static_cast<size_t>(RegionType::MAX_TYPES); i++)
    {
        mStartingRegions.emplace_back(std::list<RegionSPtr>());
    }

    unsigned short gridSize = parentGrid->SizeGet();
    mCellToRegionsMap = CellToRegionMap(gridSize * gridSize);
}

RegionsManager::~RegionsManager()
{
}

const RegionsList& RegionsManager::StartingRegionsGet() const
{
    return mStartingRegions;
}

const RegionSet& RegionsManager::RegionsGet() const
{
    return mLeafRegions;
}

RegionSPtr RegionsManager::RegionSharedPtrGet(const Region* r) const
{
    RegionSPtr regionSPtr;
    regionSPtr = r->RegionSharedPtrGet();
    if (regionSPtr.get() == nullptr) // the region is one of the starting regions
    {
        for (size_t i = 0; i < static_cast<size_t>(RegionType::MAX_TYPES); i++)
        {
            const std::list<RegionSPtr>& l = mStartingRegions.at(i);
            auto it = std::find_if(l.begin(), l.end(), [=](RegionSPtr s_ptr) {return s_ptr.get() == r; });
            if (it != l.end())
            {
                regionSPtr = *it;
                break;
            }
        }
    }
    return regionSPtr;
}

const RegionSet& RegionsManager::RegionsWithCellGet(const SudokuCell* cell) const
{
    return mCellToRegionsMap.at(cell->IdGet());
}

void RegionsManager::RegionsWithCellsGet(RegionSet& outSet, const CellSet& cells) const
{
    if (cells.size() == 0)
        return;

    outSet = RegionsWithCellGet(*cells.begin());

    // find the regions containing ALL the cells in the set
    for (const SudokuCell* cell : cells)
    {
        RegionSet group1(std::move(outSet));
        const RegionSet& group2 = RegionsWithCellGet(cell);
        outSet.clear();
        std::set_intersection(group1.begin(), group1.end(), group2.begin(), group2.end(), std::inserter(outSet, outSet.begin()));

        if (outSet.size() == 0)
        {
            break;
        }
    }
}

void RegionsManager::PartitionRegionsWithCells(CellSet cells)
{
    RegionSet outSplitRegions;
    RegionsWithCellsGet(outSplitRegions, cells);
    // there is no region containing this group of cells
    // Nothing to do
    if (outSplitRegions.size() == 0)
    {
        return;
    }
    // check whether the only region containing all cells contains ONLY those cells
    // in this case there is nothing to do.
    if (outSplitRegions.size() == 1 && (*outSplitRegions.begin())->SizeGet() == cells.size())
    {
        return;
    }

    // check whether a region made up by these cells already exists
    RegionSPtr regionSPtr;
    for (const Region* r : outSplitRegions)
    {
        // region with these cells alreday exists
        if (r->CellsGet().size() == cells.size() && !r->IsStartingRegion())
        {
            regionSPtr = RegionSharedPtrGet(r);
            break;
        }
    }
    if (!regionSPtr) // create a new Region
    {
        regionSPtr = std::make_shared<Region>(mParentGrid, std::move(cells), false);
    }

    for (Region* r : outSplitRegions)
    {
        PartitionRegionWithCells(regionSPtr, r);
    }
}

void RegionsManager::PartitionRegionWithCells(const RegionSPtr& regionSPtr, Region* region)
{
    assert(region->IsLeafNode());
    Region* newRegion;
    bool success = region->PartitionRegion(regionSPtr, newRegion);

    if (success)
    {
        mLeafRegions.erase(region);
        if (newRegion)
        {
            mLeafRegions.insert(newRegion);
        }
        if (regionSPtr)
        {
            mLeafRegions.insert(regionSPtr.get());
        }
        // update the cell to regions map
        for (SudokuCell* c : region->CellsGet())
        {
            RegionSet& regionsWithCell = mCellToRegionsMap.at(c->IdGet());
            regionsWithCell.erase(region);
            if (newRegion && newRegion->CellsGet().count(c) > 0)
            {
                regionsWithCell.insert(newRegion);
            }
            else if (regionSPtr && regionSPtr->CellsGet().count(c) > 0)
            {
                regionsWithCell.insert(regionSPtr.get());
            }
        }

        if (newRegion)
        {
            // check whether there is an existing region congruent to the right node,
            // if so merge them
            RegionSet outSplitRegions;
            RegionsWithCellsGet(outSplitRegions, newRegion->CellsGet());
            // there is no region containing this group of cells.
            // Nothing to do
            if (outSplitRegions.size() == 0)
            {
                return;
            }

            // only select the regions that are congruent
            auto pred = [&](Region* r)
            {
                return r != newRegion && r->SizeGet() == newRegion->SizeGet();
            };
            auto it = outSplitRegions.begin();
            auto endIt = outSplitRegions.end();
            while (it != endIt)
            {
                if (pred(*it))
                {
                    ++it;
                }
                else
                {
                    it = outSplitRegions.erase(it);
                }
            }

            for (Region* r : outSplitRegions)
            {
                r->MergeRegions(RegionSharedPtrGet(newRegion));
                mLeafRegions.erase(r);
                // update the cell to regions map
                for (SudokuCell* c : newRegion->CellsGet())
                {
                    RegionSet& regionsWithCell = mCellToRegionsMap.at(c->IdGet());
                    regionsWithCell.erase(r);
                }
            }
        }
    }
}

void RegionsManager::RegisterRegion(RegionSPtr regionSPtr, RegionType regionType)
{
    Region* region = regionSPtr.get();
    // Add region to the starting regions if it's not there already
    auto it = std::find_if(mStartingRegions.at(static_cast<size_t>(regionType)).begin(),
        mStartingRegions.at(static_cast<size_t>(regionType)).end(),
        [=](const RegionSPtr& r) { return region == r.get(); });
    if (it == mStartingRegions.at(static_cast<size_t>(regionType)).end())
    {
        mStartingRegions.at(static_cast<size_t>(regionType)).emplace_back(regionSPtr);
    }

    // add the group to the list
    mLeafRegions.insert(region);
    // update the cell-to-regions map
    for (const auto& cell : region->CellsGet())
    {
        mCellToRegionsMap.at(cell->IdGet()).insert(region);
    }

    // if the new region contains an existing closed region,
    // then the former can be partitioned.
    // Any additional constraint needs to be already attached
    // to the region at this point.
    std::list<RegionSPtr> subsets;
    Region* congruentRegion = nullptr;
    for (Region* r : mLeafRegions)
    {
        if(r != region && region->ContainsRegion(r))
        {
            if(r->SizeGet() == region->SizeGet())
            {
                congruentRegion = r;
            }
            else if(r->IsClosed())
            {
                subsets.push_back(RegionSharedPtrGet(r));
            }
        }
    }
    if (subsets.size() > 0)
    {
        for (const RegionSPtr& sub : subsets)
        {
            region->UpdateManagerGet()->OnNakedSetFound(sub->CellsGet(), sub->ConfirmedValuesGet(), false);
            if (sub->IsStartingRegion())
            {
                PartitionRegionsWithCells(sub->CellsGet());
            }
            else
            {
                PartitionRegionWithCells(sub, region);
            }

            if (region->RightNodeGet())
            {
                region = region->RightNodeGet();
            }
        }
    }
    else if (congruentRegion)
    {
        if (congruentRegion->IsStartingRegion())
        {
            PartitionRegionWithCells(nullptr, region);
        }
        else
        {
            region->MergeRegions(RegionSharedPtrGet(congruentRegion));
            mLeafRegions.erase(region);
            // update the cell to regions map
            for (SudokuCell* c : region->CellsGet())
            {
                RegionSet& regionsWithCell = mCellToRegionsMap.at(c->IdGet());
                regionsWithCell.erase(region);
            }
        }
    }
}

void RegionsManager::Reset()
{
    mLeafRegions.clear();
    for (auto& c : mCellToRegionsMap)
    {
        c.clear();
    }
    for (size_t i = 0; i < static_cast<size_t>(RegionType::MAX_TYPES); i++)
    {
        for (auto& r : mStartingRegions.at(i))
        {
            r->Reset();
        }
    }
    for (size_t i = 0; i < static_cast<size_t>(RegionType::MAX_TYPES); i++)
    {
        for (auto& r : mStartingRegions.at(i))
        {
            RegisterRegion(r, static_cast<RegionType>(i));
        }
    }
}

void RegionsManager::Clear()
{
    mLeafRegions.clear();
    for (auto& c : mCellToRegionsMap)
    {
        c.clear();
    }
    for (size_t i = 0; i < static_cast<size_t>(RegionType::MAX_TYPES); i++)
    {
        mStartingRegions.at(i).clear();
    }
}
