#ifndef PROGRESS_H
#define PROGRESS_H

#include "Types.h"

class KillerConstraint;

class Progress
{
protected:
    ProgressType mType;

public:
// Constructors

    Progress(ProgressType type) : mType(type) {}

// Non-const functions

    /// <summary>
    /// Apply the effect of the deduction to the relevalnt cells in the grid
    /// </summary>
    virtual void ProcessProgress() = 0;

    /// <summary>
    /// High priority progress is processed before any other
    /// </summary>
    virtual bool isHighPriotity() const { return false; }

    /// <summary>
    /// Sends a message to the UI detailing the logical deduction
    /// </summary>
    virtual void PrintMessage() const {;}

    ProgressType TypeGet() { return mType; }
};

class Progress_GivenCellAdded : public Progress
{
    SudokuCell* mCell;
    unsigned short mValue;
public:
    Progress_GivenCellAdded(SudokuCell* cell, unsigned short value) :
        Progress(ProgressType::GivenCellAdded),
        mCell(cell),
        mValue(value)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_SingleOptionLeftInCell : public Progress
{
    SudokuCell* mCell;
    unsigned short mValue;
public:
    Progress_SingleOptionLeftInCell(SudokuCell* cell, unsigned short value) :
        Progress(ProgressType::SingleOptionLeftInCell),
        mCell(cell),
        mValue(value)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_SingleCellForOption : public Progress
{
    SudokuCell* mCell;
    Region* mRegion;
    unsigned short mValue;
public:
    Progress_SingleCellForOption( SudokuCell* cell, Region* region, unsigned short value) :
        Progress(ProgressType::SingleCellInRegionForOption),
        mCell(cell),
        mRegion(region),
        mValue(value)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_NakedSubset : public Progress
{
    CellSet mCells;
    std::set<unsigned short> mValues;
public:
    Progress_NakedSubset(CellList&& cells, std::set<unsigned short>&& values) :
        Progress(ProgressType::NakedSubetFound),
        mCells(cells.begin(), cells.end()),
        mValues(values)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_HiddenNakedSubset : public Progress
{
    CellSet mCells;
    Region* mRegion;
    std::set<unsigned short> mValues;
public:
    Progress_HiddenNakedSubset(CellSet&& cells, Region* region, std::list<unsigned short>&& values) :
        Progress(ProgressType::HiddenNakedSubsetFound),
        mCells(cells),
        mRegion(region),
        mValues(values.begin(), values.end())
    {}
    Progress_HiddenNakedSubset(CellSet& cells, std::set<unsigned short>& values) :
        Progress(ProgressType::HiddenNakedSubsetFound),
        mCells(cells),
        mValues(values)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_OptionRemoved : public Progress
{
    SudokuCell* mCell;
    unsigned short mValue;
public:
    Progress_OptionRemoved(SudokuCell* cell, unsigned short value) :
        Progress(ProgressType::OptionRemoved),
        mCell(cell),
        mValue(value)
    {}
    void ProcessProgress() override;
};

class Progress_LockedCandidates : public Progress
{
    Region* mDefiningRegion;
    RegionSet mIntersectingRegions;
    CellSet mIntersection;
    unsigned short mValue;
public:
    Progress_LockedCandidates(Region* definingRegion, RegionSet&& intersectingRegions, const CellSet& intersection, unsigned short value) :
        Progress(ProgressType::LockedCandidatesFound),
        mDefiningRegion(definingRegion),
        mIntersectingRegions(intersectingRegions),
        mIntersection(intersection),
        mValue(value)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_AlmostLockedCandidates : public Progress
{
    Region* mRegion;
    CellSet mCells;
    unsigned short mValue;
public:
    Progress_AlmostLockedCandidates(Region* region, CellSet&& cells, unsigned short value) :
        Progress(ProgressType::AlmostLockedCandidatesFound),
        mRegion(region),
        mCells(cells),
        mValue(value)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_Fish : public Progress
{
protected:
    RegionList mDefiningRegions;
    CellSet mDefiningCells;
    RegionList mSecondaryRegions;
    unsigned short mValue;
public:
    Progress_Fish(RegionList& definingRegions, const CellList& definingCells, RegionList&& secondaryRegions, unsigned short value) :
        Progress(ProgressType::FishFound),
        mDefiningRegions(definingRegions),
        mDefiningCells(definingCells.begin(), definingCells.end()),
        mSecondaryRegions(secondaryRegions),
        mValue(value)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_CannibalFish : public Progress_Fish
{
    CellSet mCannibalCells;
public:
    Progress_CannibalFish(RegionList& definingRegions, const CellList& definingCells, RegionList&& secondaryRegions, CellSet&& cannibalCells, unsigned short value) :
        Progress_Fish(definingRegions, std::move(definingCells), std::move(secondaryRegions), value),
        mCannibalCells(cannibalCells)
    {
        mType = ProgressType::CannibalFishFound;
    }
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_FinnedFish : public Progress
{
protected:
    RegionList mDefiningRegions;
    RegionList mSecondaryRegions;
    CellSet mFins;
    CellSet mCellsSeingFins;
    unsigned short mValue;
public:
    Progress_FinnedFish(RegionList definingRegions, RegionList secondaryRegions, CellSet&& fins, CellSet&& cellsSeingFins, unsigned short value) :
        Progress(ProgressType::FinnedFishFound),
        mDefiningRegions(definingRegions),
        mSecondaryRegions(secondaryRegions),
        mFins(fins),
        mCellsSeingFins(cellsSeingFins),
        mValue(value)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_CannibalFinnedFish : public Progress_FinnedFish
{
    CellSet mCannibalCells;
public:
    Progress_CannibalFinnedFish(RegionList definingRegions, RegionList secondaryRegions, CellSet&& fins, CellSet&& cellsSeingFins, CellSet&& cannibalCells, unsigned short value) :
        Progress_FinnedFish(definingRegions, secondaryRegions, std::move(fins), std::move(cellsSeingFins), value),
        mCannibalCells(cannibalCells)
    {
        mType = ProgressType::CannibalFinnedFishFound;
    }
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_RegionBecameClosed : public Progress
{
    Region* mRegion;
public:
    Progress_RegionBecameClosed(Region* region) :
        Progress(ProgressType::RegionBecameClosed),
        mRegion(region)
    {}
    void ProcessProgress() override;
};

class Progress_ValueNotInKiller : public Progress
{
    Region* mRegion;
    unsigned short mValue;
public:
    Progress_ValueNotInKiller(Region* region, unsigned short value) :
        Progress(ProgressType::ValueNotAllowedInKillerSum),
        mRegion(region),
        mValue(value)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_ValueForcedInKiller : public Progress
{
    Region* mRegion;
    unsigned short mValue;
public:
    Progress_ValueForcedInKiller(Region* region, unsigned short value) :
        Progress(ProgressType::ValueForcedInKillerSum),
        mRegion(region),
        mValue(value)
    {}
    void ProcessProgress() override;
};

class Progress_ValueDisallowedByBifurcation : public Progress
{
    SudokuCell* mPivot;
    SudokuCell* mCell;
    std::set<unsigned short> mValues;
public:
    Progress_ValueDisallowedByBifurcation(SudokuCell* cell, std::set<unsigned short>&& values, SudokuCell* pivot):
        Progress(ProgressType::ValueDisallowedByBifurcation),
        mPivot(pivot),
        mCell(cell),
        mValues(values)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};

class Progress_OptionRemovedViaGuessing : public Progress
{
    SudokuCell* mCell;
    unsigned short mValue;
public:
    Progress_OptionRemovedViaGuessing(SudokuCell* cell, unsigned short value):
        Progress(ProgressType::ValueRemovedViaGuessing),
        mCell(cell),
        mValue(value)
    {}
    void ProcessProgress() override;
    void PrintMessage() const override;
};








/*
 * ==========================================================
 * Impossible Puzzles
 * ==========================================================
*/

class Progress_ImpossiblePuzzle : public Progress
{
protected:
    SudokuGrid* mGrid;
    Progress_ImpossiblePuzzle(ProgressType progressType, SudokuGrid* grid) :
        Progress(progressType),
        mGrid(grid)
    {}
public:
    void ProcessProgress() override;
    bool isHighPriotity() const override { return true; }
    void PrintMessage() const override;
};

class Impossible_ClashWithGivenCell : public Progress_ImpossiblePuzzle
{
    SudokuCell* mCell;
public:
    Impossible_ClashWithGivenCell(SudokuCell* cell, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_ClashWithGivenCell, grid),
        mCell(cell)
    {}
    void ProcessProgress() override;
};

class Impossible_ClashWithSolvedCell : public Progress_ImpossiblePuzzle
{
    SudokuCell* mCell;
public:
    Impossible_ClashWithSolvedCell(SudokuCell* cell, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_ClashWithSolvedCell, grid),
        mCell(cell)
    {}
    void ProcessProgress() override;
};

class Impossible_NoOptionsLeftInCell : public Progress_ImpossiblePuzzle
{
    SudokuCell* mCell;
public:
    Impossible_NoOptionsLeftInCell(SudokuCell* cell, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_NoOptionsLeftInCell, grid),
        mCell(cell)
    {}
    void ProcessProgress() override;
};

class Impossible_NoCellLeftForValueInRegion : public Progress_ImpossiblePuzzle
{
    Region* mRegion;
    unsigned short mValue;
public:
    Impossible_NoCellLeftForValueInRegion(Region* region, unsigned short value, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_NoCellLeftForValueInRegion, grid),
        mRegion(region),
        mValue(value)
    {}
    void ProcessProgress() override;
};

class Impossible_TooFewValuesForRegion : public Progress_ImpossiblePuzzle
{
    CellSet mCells;
    std::set<unsigned short> mValues;
public:
    Impossible_TooFewValuesForRegion(CellList&& cells, std::set<unsigned short>&& values, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_TooFewValuesForRegion, grid),
        mCells(cells.begin(), cells.end()),
        mValues(values)
    {}
    Impossible_TooFewValuesForRegion(CellSet& cells, std::set<unsigned short>& values, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_TooFewValuesForRegion, grid),
        mCells(cells),
        mValues(values)
    {}
    void ProcessProgress() override;
};

class Impossible_TooManyValuesForRegion : public Progress_ImpossiblePuzzle
{
    CellSet mCells;
    std::set<unsigned short> mValues;
public:
    Impossible_TooManyValuesForRegion(CellSet&& cells, std::list<unsigned short>&& values, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_TooManyValuesForRegion, grid),
        mCells(cells),
        mValues(values.begin(), values.end())
    {}
    Impossible_TooManyValuesForRegion(CellSet& cells, std::set<unsigned short>& values, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_TooManyValuesForRegion, grid),
        mCells(cells),
        mValues(values)
    {}
    void ProcessProgress() override;
};

class Impossible_Fish : public Progress_ImpossiblePuzzle
{
    RegionList mDefiningRegion;
    RegionList mSecondaryRegion;
    unsigned short mValue;
public:
    Impossible_Fish( RegionList&& definingRegion, RegionList&& secondaryRegions, unsigned short value, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_Fish, grid),
        mDefiningRegion(definingRegion),
        mSecondaryRegion(secondaryRegions),
        mValue(value)
    {}
    void ProcessProgress() override;
};

class Impossible_NoKillerSum : public Progress_ImpossiblePuzzle
{
    KillerConstraint* mKiller;
public:
    Impossible_NoKillerSum(KillerConstraint* killer, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_NoSumForKillerCage, grid),
        mKiller(killer)
    {}
    void ProcessProgress() override;
};

class Impossible_NoSolutionByBifurcation : public Progress_ImpossiblePuzzle
{
    SudokuCell* mPivotCell;
public:
    Impossible_NoSolutionByBifurcation(SudokuCell* pivotCell, SudokuGrid* grid) :
        Progress_ImpossiblePuzzle(ProgressType::Impossible_NoSolutionByBifurcation, grid),
        mPivotCell(pivotCell)
    {}
    void ProcessProgress() override;
};
#endif // !PROGRESS_H
