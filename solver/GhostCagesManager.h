#ifndef GHOSTCAGESMANAGER_H
#define GHOSTCAGESMANAGER_H

// Include
#include "Types.h"
#include "Region.h"

// Typedefs
typedef std::vector<RegionSet> CellToRegionMap;

class GhostCagesManager
{
    std::list<RegionSPtr> mGhostCages;
    RegionSet mLeafRegions;				// list of all leaf regions (see Region class)
    CellToRegionMap mCellToRegionsMap;	// maps each cell in the grid to the regions that cell is in
    SudokuGrid* mParentGrid;			// pointer to the grid the cell belongs to

    struct Snapshot
    {
        std::list<RegionSPtr> mGhostCages;
        RegionSet mLeafRegions;
        CellToRegionMap mCellToRegionsMap;
        Snapshot(const std::list<RegionSPtr> ghostCages, const RegionSet& leaves, const CellToRegionMap& map):
            mGhostCages(ghostCages),
            mLeafRegions(leaves),
            mCellToRegionsMap(map)
        {}
    };
    std::unique_ptr<Snapshot> mSnapshot;
public:
    GhostCagesManager(SudokuGrid* parentGrid);
    ~GhostCagesManager();

// Public getters
    const RegionSet& RegionsGet() const;


// Constant methods

    /// <summary>
    /// Finds and returns all the regions containing the cell passed as parameter.
    /// </summary>
    const RegionSet& RegionsWithCellGet(const SudokuCell* cell) const;

    /// <summary>
    /// Finds a returns all the regions cantaining all the cells passed as parameters.
    /// </summary>
    void RegionsWithCellsGet(RegionSet& outSet, const CellSet& cells) const;

// Non-constant methods

    /// <summary>
    /// Find all regions containing all the cells passed as an argument,
    /// then partition each of these regions into two new smaller regions,
    /// one made up by the cells in the argument and the other made up by
    /// the remaining cells.
    /// </summary>
    void PartitionRegionsWithCells(CellSet cells);

    /// <summary>
    /// Define a staring region of the puzzle
    /// </summary>
    void RegisterRegion(RegionSPtr regionSPtr);

    /// <summary>
    /// Clear all the starting regions
    /// </summary>
    void Clear();

    void TakeSnapshot();
    void RestoreSnapshot();

private:
    RegionSPtr RegionSharedPtrGet(const Region* r) const;

    /// <summary>
    /// Partition the argument region into two new smaller regions,
    /// one made up by the cells in the argument regionSPtr and the other
    /// made up by theremaining cells.
    /// </summary>
    void PartitionRegionWithCells(const RegionSPtr& regionSPtr, Region* region);
};

#endif // GHOSTCAGESMANAGER_H
