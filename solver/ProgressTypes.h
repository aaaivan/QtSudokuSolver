#ifndef PROGRSS_TYPES_H
#define PROGRSS_TYPES_H

// enums
enum class TechniqueType
{
    LockedCandidates,
    NakedSubset,
    HiddenSubset,
    Fish,
    InniesOuties,
    Bifurcation,

    MAX_TECHNIQUES
};

enum class ProgressType : int
{
    GivenCellAdded,
    SingleOptionLeftInCell,
    SingleCellInRegionForOption,
    NakedSubetFound,
    HiddenSubsetFound,
    OptionRemoved,
    LockedCandidatesFound,
    AlmostLockedCandidatesFound,
    FishFound,
    CannibalFishFound,
    FinnedFishFound,
    CannibalFinnedFishFound,
    RegionBecameClosed,
    ValueNotAllowedInKillerSum,
    ValueForcedInKillerSum,
    ValueDisallowedByBifurcation,
    ValueRemovedViaGuessing,
    GhostCageAddedToGrid,
    SumConstraintOnSplitOutie,

    Impossible_ClashWithGivenCell,
    Impossible_ClashWithSolvedCell,
    Impossible_NoOptionsLeftInCell,
    Impossible_NoCellLeftForValueInRegion,
    Impossible_TooManyValuesForRegion,
    Impossible_TooFewValuesForRegion,
    Impossible_Fish,
    Impossible_NoSumForKillerCage,
    Impossible_NoSolutionByBifurcation,
    Impossible_BrokenInnie,
    Impossible_ClashingInnies,
    Impossible_BrokenOutie,

    MAX_PROGRESS_TYPES
};


#endif // !PROGRSS_TYPES_H
