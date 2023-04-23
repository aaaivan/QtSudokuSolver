#include "Scanning.h"
#include "Region.h"
#include "SudokuGrid.h"
#include "RegionUpdatesManager.h"
#include "RegionsManager.h"

void ScanNaked(const CellSet& nakedSubset, const std::set<unsigned short> & cellValues, const SudokuGrid* grid, bool isHidden)
{
    // Get all the regions the naked subset is in
    RegionSet regions;
    grid->RegionsManagerGet()->RegionsWithCellsGet(regions, nakedSubset);

    for (auto& region : regions)
    {
        region->UpdateManagerGet()->OnNakedSetFound(nakedSubset, cellValues, isHidden);
    }
    grid->RegionsManagerGet()->PartitionRegionsWithCells(nakedSubset);
    grid->GhostRegionsManagerGet()->PartitionRegionsWithCells(nakedSubset);

}

void ScanLocked(const CellSet& intersection, const Region* region, unsigned short lockedValue)
{
    region->UpdateManagerGet()->OnLockedCandidatesFound(intersection, lockedValue);
}
