#include "Progress.h"
#include "Scanning.h"
#include "SudokuCell.h"
#include "Region.h"
#include "RegionUpdatesManager.h"
#include "SudokuGrid.h"
#include "GridProgressManager.h"
#include "../sudokusolverthread.h"

void Progress_GivenCellAdded::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("Given cell added: %s = %u\n", mCell->PrintPosition().c_str(), mValue);
#endif // PRINT_LOG_MESSAGES
    ScanNaked({ mCell }, { mValue }, mCell->GridGet(), false);
    mCell->ValueSet(mValue);
}

void Progress_SingleOptionLeftInCell::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("Value of cell found: %s = %u\n", mCell->PrintPosition().c_str(), mValue);
#endif // PRINT_LOG_MESSAGES
    ScanNaked({ mCell }, { mValue }, mCell->GridGet(), false);
    mCell->ValueSet(mValue);
}

void Progress_SingleCellForOption::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("Naked Single found: %s = %u\n", mCell->PrintPosition().c_str(), mValue);
#endif // PRINT_LOG_MESSAGES
    ScanNaked({ mCell }, { mValue }, mCell->GridGet(), true);
}

void Progress_NakedSubset::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("Naked subset found:");
    for (const auto& c : mCells)
    {
        printf("%s%s, ", c->PrintPosition().c_str(), c->PrintOptions().c_str());
    }
    printf("\n");
#endif // PRINT_LOG_MESSAGES
    ScanNaked(mCells, mValues, (*mCells.begin())->GridGet(), false);
}

void Progress_HiddenNakedSubset::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("Hidden naked subset found:");
    for (const auto& c : mCells)
    {
        printf("%s%s, ", c->PrintPosition().c_str(), c->PrintOptions().c_str());
    }
    printf("\n");
#endif // PRINT_LOG_MESSAGES
    ScanNaked(mCells, mValues, (*mCells.begin())->GridGet(), true);
}

void Progress_OptionRemoved::ProcessProgress()
{
#if PRINT_LOG_MESSAGES && 0
    printf("Option removed form cell: %s != %u\n", mCell->PrintPosition().c_str(), mValue);
#endif // PRINT_LOG_MESSAGES
    mCell->GridGet()->SolverThreadManagerGet()->NotifyCellChanged(mCell);
    const RegionSet& intersectingRegions = mCell->GetRegionsWithCell();
    for (Region* region : intersectingRegions)
    {
        region->UpdateManagerGet()->OnCellOptionRemoved(mCell, mValue);
    }
}

void Progress_LockedCandidates::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("Locked %u found:", mValue);
    for (const auto& c : mIntersection)
    {
        printf("%s, ", c->PrintPosition().c_str());
    }
    printf("\n");
#endif // PRINT_LOG_MESSAGES
    for (const Region* region : mIntersectingRegions)
    {
        ScanLocked(mIntersection, region, mValue);
    }
}

void Progress_AlmostLockedCandidates::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("Almost Locked %u found:", mValue);
    for (const auto& c : mCells)
    {
        printf("%s, ", c->PrintPosition().c_str());
    }
    printf("\n");
#endif // PRINT_LOG_MESSAGES

    for (SudokuCell* const & c : mCells)
    {
        c->RemoveOption(mValue);
    }
}

void Progress_Fish::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("Fish of size %u on %u found:\n", mSecondaryRegions.size(), mValue);
    for (const auto& r : mDefiningRegions)
    {
        printf("%s, ", r->IdGet().c_str());
    }
    printf("\n");
    for (const auto& r : mSecondaryRegions)
    {
        printf("%s, ", r->IdGet().c_str());
    }
    printf("\n");
#endif // PRINT_LOG_MESSAGES
    // remove mValue from all the cells in the secondary set that are not in the defining set
    for (Region* r : mSecondaryRegions)
    {
        r->UpdateManagerGet()->OnFishFound(mDefiningCells, mValue);
    }
}

void Progress_CannibalFish::ProcessProgress()
{
    for (const auto& cell : mCannibalCells)
    {
        cell->RemoveOption(mValue);
    }
    Progress_Fish::ProcessProgress();
}

void Progress_FinnedFish::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("Finned fish on %u found: \n", mValue);
    for (const auto& r : mDefiningRegions)
    {
        printf("%s, ", r->IdGet().c_str());
    }
    printf("\n");
    for (const auto& r : mSecondaryRegions)
    {
        printf("%s, ", r->IdGet().c_str());
    }
    printf("\n");
    printf("eliminations(");
    for (const auto& c : mCellsSeingFins)
    {
        printf("%s, ", c->PrintPosition().c_str());
    }
    printf(")");
    printf("\n");
#endif // PRINT_LOG_MESSAGES

    for (SudokuCell* const& c : mCellsSeingFins)
    {
        c->RemoveOption(mValue);
    }
}

void Progress_CannibalFinnedFish::ProcessProgress()
{
    for (const auto& cell : mCannibalCells)
    {
        cell->RemoveOption(mValue);
    }
    Progress_FinnedFish::ProcessProgress();
}

void Progress_RegionBecameClosed::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("Region became closed\n");
#endif // PRINT_LOG_MESSAGES

    ScanNaked(mRegion->CellsGet(), mRegion->ConfirmedValuesGet(), mRegion->GridGet(), false);
}

void Progress_ValueNotInKiller::ProcessProgress()
{
#if PRINT_LOG_MESSAGES
    printf("%u not allowed in killer cage.\n", mValue);
#endif // PRINT_LOG_MESSAGES

    for (const auto& c : mRegion->CellsGet())
    {
        c->RemoveOption(mValue);
    }
}

void Progress_ValueForcedInKiller::ProcessProgress()
{
    mRegion->AddConfirmedValue(mValue);
}


/*
 * ==========================================================
 * Impossible Puzzles
 * ==========================================================
*/
void Progress_ImpossiblePuzzle::ProcessProgress()
{
    mGrid->ProgressManagerGet()->Abort();
}

void Impossible_ClashWithGivenCell::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}

void Impossible_ClashWithSolvedCell::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}

void Impossible_NoOptionsLeftInCell::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}

void Impossible_NoCellLeftForValueInRegion::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}

void Impossible_TooFewValuesForRegion::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}

void Impossible_TooManyValuesForRegion::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}

void Impossible_Fish::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}

void Impossible_NoKillerSum::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}
