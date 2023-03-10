#ifndef SOLVING_TECHNIQUE_H
#define SOLVING_TECHNIQUE_H

#include "Types.h"

class SolvingTechnique
{
public:
    enum ObservedComponent
    {
        ObserveValues,
        ObserveCells
    };

protected:
    TechniqueType mType;
    ObservedComponent mObservedComponent;
    SudokuGrid* mGrid;
    bool mFinished;

public:
    // Special functions
    SolvingTechnique(SudokuGrid* grid, TechniqueType type, ObservedComponent observedComponent);

    TechniqueType TypeGet() const;
    bool HasFinished() const;
    ObservedComponent ObservedComponentGet() const;

    virtual void NextStep() = 0;
    virtual void Reset() = 0;
};

class LockedCandidatesTechnique : public SolvingTechnique
{
    Region* mCurrentRegion;

public:
    LockedCandidatesTechnique(SudokuGrid* grid, ObservedComponent observedComponent);

    void NextStep() override;
    void Reset() override;

    /// <summary>
    /// Search for a locked candidates in the specified region.
    /// A candidate is locked when the cells that can host it are at the intersection of two or more Regions.
    /// Returns true if any progress was made (either the technique was successful or the puzzle was found to be impossible)
    /// </summary>
    void SearchLockedCandidates(unsigned short value);
};

class NakedSubsetTechnique : public SolvingTechnique
{
    Region* mCurrentRegion;

public :
    NakedSubsetTechnique(SudokuGrid* grid, ObservedComponent observedComponent);

    void NextStep() override;
    void Reset() override;

    /// <summary>
    /// Find the smallest precedently undiscovered naked subsets in the specified region.
    /// A naked subset is a set of N cells whose candidates are picked from a set of exactly N values.
    /// Returns true if any progress was maden (either the technique was successful or the puzzle was found to be impossible)
    /// </summary>
    void SearchNakedSubsets(const CellSet& cellsToSearch);
    bool SearchNakedSubsetInner(const CellList& cells, CellList::iterator it, const CellList::iterator endIt, const CellSet& excludeCells, std::set<unsigned short>& candidates, CellList& outNakedSubset, const size_t targetSize, bool& impossible);
};

class HiddenNakedSubsetTechnique : public SolvingTechnique
{
    Region* mCurrentRegion;

public:
    HiddenNakedSubsetTechnique(SudokuGrid* grid, ObservedComponent observedComponent);

    void NextStep() override;
    void Reset() override;

    /// <summary>
    /// Find the smallest precedently undiscovered hidden naked subsets in the specified region.
    /// A hidden naked subset is a set of N candidates whose viable hosting cells are picked from a set of exactly N cells.
    /// Returns true if any progress was maden (either the technique was successful or the puzzle was found to be impossible)
    /// </summary>
    void SearchHiddenNakedSubsets(const std::set<unsigned short>& valuesToSearch);
    bool SearchHiddenNakedSubsetInner(const std::list<ValueMapEntry>& values, std::list<ValueMapEntry>::iterator it, const std::list<ValueMapEntry>::iterator endIt, const std::set<unsigned short>& excludeValues, std::list<unsigned short>& candidates, CellSet& outNakedSubset, const size_t subsetFinalSize, bool& impossible);
};

class FishTechnique : public SolvingTechnique
{
    unsigned short mCurrentValue;
    RegionSet mRegionsToSearch;
    RegionSet mAvailableRegions;
    const unsigned short mMinSize;
    unsigned short mCurrentSize;

public:
    FishTechnique(SudokuGrid* grid, ObservedComponent observedComponent);

    void NextStep() override;
    void Reset() override;
    void UpdateRegions();
    void NotifyFailure();

    /// <summary>
    /// Search a fish of the specified value using a defining set containing at least one of the specified regions.
    /// Returns true if any progress was maden (either the technique was successful or the puzzle was found to be impossible)
    /// </summary>
    void SearchFish();
    void GetPossibeDefiningRegions(Region* includeRegion, const RegionSet& allowedRegions, std::list<RegionList>& definingSets, unsigned short size);
    void GetPossibeDefiningRegionsInner(std::list<RegionList>& definingSets, const unsigned short size, const RegionSet& allowedRegions, RegionSet::const_iterator regIt, RegionList& nextSet);
    bool SearchSecondaryFishRegion(RegionList& definingSet, unsigned short value, const SudokuGrid* grid, bool& impossible);
    bool SearchSecondaryFishRegionInner(RegionList& definingRegion, const std::map<SudokuCell*, RegionSet>& intersectionMap,
        std::map<SudokuCell*, RegionSet>::const_iterator& mapIt, const unsigned short& targetSize, RegionList& currentSet,
        CellList& fins, RegionSet& finsRegions, CellSet& cellsSeeingFins,
        const SudokuGrid* grid, const unsigned short& fishValue, bool& impossible);
    bool IsFishValid(RegionList& definingRegion, const std::map<SudokuCell*, RegionSet>& intersectionMap, RegionList& currentSet, const SudokuGrid* grid, const unsigned short& fishValue);
};
#endif // !SOLVING_TECHNIQUE_H
