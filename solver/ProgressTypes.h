#ifndef PROGRSS_TYPES_H
#define PROGRSS_TYPES_H

// enums
enum class TechniqueType
{
    LockedCandidates,
    NakedSubset,
    HiddenNakedSubset,
    Fish,
    Bifurcation,

    MAX_TECHNIQUES
};

enum class ProgressType : int
{
    GivenCellAdded,
    SingleOptionLeftInCell,
    SingleCellInRegionForOption,
    NakedSubetFound,
    HiddenNakedSubsetFound,
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

    Impossible_ClashWithGivenCell,
    Impossible_ClashWithSolvedCell,
    Impossible_NoOptionsLeftInCell,
    Impossible_NoCellLeftForValueInRegion,
    Impossible_TooManyValuesForRegion,
    Impossible_TooFewValuesForRegion,
    Impossible_Fish,
    Impossible_NoSumForKillerCage,
    Impossible_NoSolutionByBifurcation,

    MAX_PROGRESS_TYPES
};


#endif // !PROGRSS_TYPES_H
