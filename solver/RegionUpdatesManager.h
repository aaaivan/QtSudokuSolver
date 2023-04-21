#ifndef REGION_UPDATES_MANAGER_H
#define REGION_UPDATES_MANAGER_H

#include"Types.h"

typedef std::map<TechniqueType, CellSet> HotCellsArray;
typedef std::map<TechniqueType, std::set<unsigned short>> HotValuesArray;

/// <summary>
/// Deals with updating the viable options of the cells in a Region
/// </summary>
class RegionUpdatesManager
{
    Region* mRegion;	// Region this class acts on
    // keep track of the cells whose options have changed
    // since the last time a certain technique was applied
    HotCellsArray mHotCells;
    // keep track of the values that have been removed from any cell in the region
    // since the last time a certain technique was applied
    HotValuesArray mHotValues;

    struct Snapshot
    {
        HotCellsArray mHotCells;
        HotValuesArray mHotValues;
        Snapshot(const HotCellsArray& cells, const HotValuesArray& vals):
            mHotCells(cells),
            mHotValues(vals)
        {}
    };
    std::unique_ptr<Snapshot> mSnapshot;

public:
// Constructors

    RegionUpdatesManager(Region* region);

// Const methods

    /// <summary>
    /// Check whether any cell has change that would justify the use of certain technique on this region
    /// </summary>
    bool IsRegionReadyForTechnique(TechniqueType type, std::set<SudokuCell*>& outCells) const;
    /// <summary>
    /// Check whether cell has changed since the last time a certain technique was used
    /// </summary>
    bool IsRegionReadyForTechnique(TechniqueType type, SudokuCell* cell) const;
    /// <summary>
    /// Check whether any value has been removed that would justify the use of certain technique on this region
    /// </summary>
    bool IsRegionReadyForTechnique(TechniqueType type, std::set<unsigned short>& outValues) const;
    /// <summary>
    /// Check whether a value has been removed since the last time a certain technique was used
    /// </summary>
    bool IsRegionReadyForTechnique(TechniqueType type, unsigned short value) const;

// Non-const methods

    /// <summary>
    /// Callback function for when a naked subset is found within the managed Region
    /// </summary>
    void OnNakedSetFound(const CellSet& nakedSubset, const std::set<unsigned short>& values, bool isHidden);

    /// <summary>
    /// Callback function for when an option is removed from a cell within the managed Region
    /// </summary>
    void OnCellOptionRemoved(SudokuCell* cell, unsigned short value);

    /// <summary>
    /// Callback function for when a locked candidate is found within the managed Region
    /// </summary>
    void OnLockedCandidatesFound(const CellSet& intersection, unsigned short lockedValue);

    /// <summary>
    /// Callback function for when a fish is found whose defining set contains the managed Region
    /// </summary>
    void OnFishFound(const CellSet& definingSet, unsigned short fishValue);

    /// <summary>
    /// Notifies the region updates manager that a certain technique
    /// has not been successful on this region
    /// </summary>
    void OnTechniqueFailed(TechniqueType type);
    void OnTechniqueFailed(TechniqueType type, SudokuCell* cell);
    void OnTechniqueFailed(TechniqueType type, unsigned short value);

    /// <summary>
    /// Clear the hot cells and hot values
    /// </summary>
    void ResetHotArrays();

    /// <summary>
    /// Callback for when mRegion is partitioned into smaller regions
    /// </summary>
    void OnRegionPartitioned();

    /// <summary>
    /// Callback for when a confirmed value is added to mRegion
    /// </summary>
    void OnConfirmedValueAdded(unsigned short value);

    /// <summary>
    /// Clear all the cached hot values and hot cells
    /// </summary>
    void Reset();

    void TakeSnapshot();
    void RestoreSnapshot();
};

#endif // !REGION_UPDATES_MANAGER_H


