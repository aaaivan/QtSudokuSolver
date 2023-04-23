#ifndef REGION_H
#define REGION_H

// Includes
#include "Types.h"
#include "VariantConstraints.h"
#include <string>

// Classes
class RegionUpdatesManager;

/// <summary>
/// Group of cells that must contain different values once the puzzle is solved
/// </summary>
class Region
{
protected:
    std::string mName;
    CellSet mCells;				// cells that make up the region
    std::set<unsigned short> mConfirmedValues;	// values that must be in this group
    std::set<unsigned short> mAllowedValues;	// values that could be in this group
    std::map<unsigned short, CellSet> mValueToCellMap;
    SudokuGrid* mParentGrid;					// pointer to the grid the cell belongs to
    bool mIsStartingRegion;						// whether this is a starting region
    unsigned short mSize;						// number of cells in the group

    std::list<std::unique_ptr<VariantConstraint>> mAdditionalConstraints;	// additional constraints applying to this region
    std::unique_ptr<RegionUpdatesManager> mUpdateManager;

    RegionSPtr mRightNode;			// for efficiency purposes, a region can be partitioned into child regions
    RegionSPtr mLeftNode;			// for efficiency purposes, a region can be partitioned into child regions
    RegionList mParents;			// parent Regions that this Region has been partitioned from

    friend class RegionsManager;

    struct Snapshot
    {
        std::set<unsigned short> mConfirmedValues;
        std::set<unsigned short> mAllowedValues;
        std::map<unsigned short, CellSet> mValueToCellMap;

        Snapshot(const std::set<unsigned short>& confirmed,
                 const std::set<unsigned short>& allowed,
                 const std::map<unsigned short, CellSet>& map):
            mConfirmedValues(confirmed),
            mAllowedValues(allowed),
            mValueToCellMap(map)
        {}
    };
    std::unique_ptr<Snapshot> mSnapshot;

public:
// Constructors

    Region(SudokuGrid* parentGrid, CellSet&& cells, bool startingRegion);
    ~Region();
    void Init();

// Public getters

    std::string IdGet() const;
    const CellSet& CellsGet() const;
    std::vector<std::array<unsigned short, 2>> CellCoordsGet() const;
    unsigned short SizeGet() const;
    const std::set<unsigned short>& ConfirmedValuesGet() const;
    const std::set<unsigned short>& AllowedValuesGet() const;
    const CellSet& CellsWithValueGet(unsigned short value) const;
    bool HasConfirmedValue(unsigned short value) const;
    bool ContainsRegion(Region* r);
    bool IsValueAllowed(unsigned short value) const;
    SudokuGrid* GridGet() const;
    RegionUpdatesManager* UpdateManagerGet() const;
    const RegionList& ParentNodesGet() const;
    Region* LeftNodeGet() const;
    Region* RightNodeGet() const;
    bool IsLeafNode() const;
    void LeafNodesGet(RegionSet& regions);
    /// <summary>
    /// get the shared pointer managing this Region
    /// </summary>
    RegionSPtr RegionSharedPtrGet() const;
    /// <summary>
    /// Find the leaf node containing the specified cell
    /// </summary>
    Region* FindLeafNodeWithCell(const SudokuCell* cell);
    /// <summary>
    /// Get the constraint class of the specified type
    /// </summary>
    const VariantConstraint* GetConstraintByType(RegionType type) const;
    const std::list<std::unique_ptr<VariantConstraint>>& VariantConstraintsGet() const;
    std::string RegionNameGet() const;

// Constant methods

    bool IsStartingRegion() const;
    bool IsClosed() const;
    unsigned int SumGet() const;
    bool ContainsCells(const CellSet& cells) const;
    bool IsContainedInCells(const CellSet& cells) const;

// Non-constant methods

    /// <summary>
    /// Add the specified confirmed value to this Region.
    /// </summary>
    void AddConfirmedValue(unsigned value);
    void UpdateValueMap(unsigned short removedValue, SudokuCell* removedFrom);
    void AddVariantConstraint(std::unique_ptr<VariantConstraint> constraint);
    void RegionNameSet(std::string name);

    /// <summary>
    /// clear all the cells in the region
    /// </summary>
    void Reset();
    void TakeSnapshot();
    void RestoreSnapshot();

private:
    /// <summary>
    /// Remove the specified allowed value from this Region
    /// </summary>
    void RemoveAllowedValue(unsigned short value);
    /// <summary>
    /// If this Region is a leaf node, creates two childred nodes from this Region.
    /// The two new nodes will not share any cell or value
    /// </summary>
    bool PartitionRegion(const RegionSPtr& leftNode, Region*& outRightNode);

    /// <summary>
    /// Merge this region to the one passed as parameter
    /// </summary>
    void MergeRegions(const RegionSPtr& rightNode);
};

#endif // !REGION_H
