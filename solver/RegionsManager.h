#ifndef REGIONS_MANAGER_H
#define REGIONS_MANAGER_H

// Include
#include "Types.h"
#include "Region.h"
#include "SudokuCell.h"

// Typedefs
typedef std::vector<RegionSet> CellToRegionMap;
typedef std::vector<CellSet> CellToCellsMap;
typedef std::vector<std::list<RegionSPtr>> RegionsList;

/// <summary>
/// This class keeps track of the regions defined by the puzzle.
/// A region must contain cells with different values.
/// A region that contains all the allowed digits once ic called "house".
/// </summary>
class RegionsManager
{
    RegionsList mStartingRegions;		// list of all regions defined by the setter (typically row, columns and cages)
    RegionSet mLeafRegions;				// list of all leaf regions (see Region class)
    CellToRegionMap mCellToRegionsMap;	// maps each cell in the grid to the regions that cell is in
    CellToCellsMap mCellToConnectedCellsMap;	// maps each cell in the grid to the cells that can see it
    SudokuGrid* mParentGrid;			// pointer to the grid the cell belongs to

    struct Snapshot
    {
        RegionSet mLeafRegions;
        CellToRegionMap mCellToRegionsMap;
        CellToCellsMap mCellToConnectedCellsMap;
        Snapshot(const RegionSet& leaves, const CellToRegionMap& map, const CellToCellsMap& cellsMap):
            mLeafRegions(leaves),
            mCellToRegionsMap(map),
            mCellToConnectedCellsMap(cellsMap)
        {}
    };
    std::unique_ptr<Snapshot> mSnapshot;

public:
// Constructors/Destructors

    RegionsManager(SudokuGrid* parentGrid);
    ~RegionsManager();

// Public getters

    const RegionsList& StartingRegionsGet() const;
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

    /// <summary>
    /// Find the list of cells that can "see" all the defining cells
    /// </summary>
    template<class T>
    void FindConnectedCells(const T& definingCells, CellSet& outConnectedCells) const;
    /// <summary>
    /// Find the list of cells that can "see" all the defining cells AND have "value" as a viable option
    /// </summary>
    template<class T>
    void FindConnectedCellsWithValue(const T& definingCells, CellSet& outConnectedCells, unsigned short value) const;


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
    void RegisterRegion(RegionSPtr regionSPtr, RegionType regionType);

    /// <summary>
    /// Reset all the regions to their initial state
    /// </summary>
    void Reset();

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

    /// <summary>
    /// Update the set of cells that can see cell c
    /// </summary>
    void UpdateConnectedCells(SudokuCell* c);
};

template<class T>
void RegionsManager::FindConnectedCells(const T& definingCells, CellSet& outConnectedCells) const
{
    auto it = definingCells.begin();
    const auto end = definingCells.end();

    if (it != end)
    {
        const auto& cells = mCellToConnectedCellsMap[(*it)->IdGet()];
        outConnectedCells.insert(cells.begin(), cells.end());
        ++it;
    }

    while (it != end)
    {
        const auto& cells = mCellToConnectedCellsMap[(*it)->IdGet()];

        auto inIt = outConnectedCells.begin();
        while (inIt != outConnectedCells.end())
        {
            if (cells.count(*inIt) == 0)
            {
                inIt = outConnectedCells.erase(inIt);
            }
            else
            {
                ++inIt;
            }
        }
        ++it;
    }
}

template<class T>
void RegionsManager::FindConnectedCellsWithValue(const T& definingCells, CellSet& outConnectedCells, unsigned short value) const
{
    auto it = definingCells.begin();
    const auto end = definingCells.end();

    if (it != end)
    {
        const auto& cells = mCellToConnectedCellsMap[(*it)->IdGet()];
        outConnectedCells.insert(cells.begin(), cells.end());
        ++it;

        auto inIt = outConnectedCells.begin();
        while (inIt != outConnectedCells.end())
        {
            if ((*inIt)->HasGuess(value))
            {
                ++inIt;
            }
            else
            {
                inIt = outConnectedCells.erase(inIt);
            }
        }
    }

    while (it != end)
    {
        const auto& cells = mCellToConnectedCellsMap[(*it)->IdGet()];

        auto inIt = outConnectedCells.begin();
        while (inIt != outConnectedCells.end())
        {
            if (cells.count(*inIt) == 0)
            {
                inIt = outConnectedCells.erase(inIt);
            }
            else
            {
                ++inIt;
            }
        }
        ++it;
    }
}
#endif // REGIONS_MANAGER_H

