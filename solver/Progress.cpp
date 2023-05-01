#include "Progress.h"
#include "Scanning.h"
#include "SudokuCell.h"
#include "Region.h"
#include "RegionUpdatesManager.h"
#include "SudokuGrid.h"
#include "GridProgressManager.h"
#include "RegionsManager.h"
#include "../sudokusolverthread.h"
#include "GhostCagesManager.h"

void Progress_GivenCellAdded::ProcessProgress()
{
    ScanNaked({ mCell }, { mValue }, mCell->GridGet(), false);
    mCell->ValueSet(mValue);

    PrintMessage();
}

void Progress_GivenCellAdded::PrintMessage() const
{
    SudokuSolverThread* st = mCell->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Scan given " + std::to_string(mValue) + " in " + mCell->CellNameGet() + ".";
        st->NotifyLogicalDeduction(message);
    }
}

void Progress_SingleOptionLeftInCell::ProcessProgress()
{
    if(mCell->IsSolved()) return;

    ScanNaked({ mCell }, { mValue }, mCell->GridGet(), false);
    mCell->ValueSet(mValue);

    PrintMessage();
}

void Progress_SingleOptionLeftInCell::PrintMessage() const
{
    SudokuSolverThread* st = mCell->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Naked single in " + mCell->CellNameGet() + ". " +
                              std::to_string(mValue) + " is the only candidate.";
        st->NotifyLogicalDeduction(message);
    }
}

void Progress_SingleCellForOption::ProcessProgress()
{
    if(mCell->IsSolved()) return;

    ScanNaked({ mCell }, { mValue }, mCell->GridGet(), true);
    mCell->ValueSet(mValue);

    PrintMessage();
}

void Progress_SingleCellForOption::PrintMessage() const
{
    SudokuSolverThread* st = mCell->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Hidden single on " + std::to_string(mValue) + ". " +
                              "It can only go in " + mCell->CellNameGet() + " in " +
                              mRegion->RegionNameGet() + ".";
        st->NotifyLogicalDeduction(message);
    }

}

void Progress_NakedSubset::ProcessProgress()
{
    auto cIt = mCells.begin();
    SudokuGrid* grid = (*cIt)->GridGet();
    ScanNaked(mCells, mValues, grid, false);

    PrintMessage();
}

void Progress_NakedSubset::PrintMessage() const
{
    auto cIt = mCells.begin();
    SudokuGrid* grid = (*cIt)->GridGet();
    SudokuSolverThread* st = grid->SolverThreadGet();
    if(st && mCells.size() < grid->SizeGet())
    {
        std::string message = "->Naked subset in {";
        while(cIt != mCells.end())
        {
            message += (*cIt)->CellNameGet() + ",";
            cIt++;
        }
        message.pop_back();
        message += "} on values {";

        auto vIt = mValues.begin();
        while(vIt != mValues.end())
        {
            message += std::to_string(*vIt) + ",";
            vIt++;
        }
        message.pop_back();
        message += "}.";

        st->NotifyLogicalDeduction(message);
    }
}

void Progress_HiddenSubset::ProcessProgress()
{
    auto cIt = mCells.begin();
    SudokuGrid* grid = (*cIt)->GridGet();
    ScanNaked(mCells, mValues, grid, true);

    PrintMessage();
}

void Progress_HiddenSubset::PrintMessage() const
{
    auto cIt = mCells.begin();
    SudokuGrid* grid = (*cIt)->GridGet();
    SudokuSolverThread* st = grid->SolverThreadGet();
    if(st && mCells.size() < grid->SizeGet())
    {
        std::string message = "->Hidden subset on values {";
        auto vIt = mValues.begin();
        while(vIt != mValues.end())
        {
            message += std::to_string(*vIt) + ",";
            vIt++;
        }
        message.pop_back();
        message += "} in " + mRegion->RegionNameGet();
        message += ".";

        st->NotifyLogicalDeduction(message);
    }
}

void Progress_OptionRemoved::ProcessProgress()
{
    const RegionSet& intersectingRegions = mCell->GetRegionsWithCell();
    for (Region* region : intersectingRegions)
    {
        region->UpdateManagerGet()->OnCellOptionRemoved(mCell, mValue);
    }

    const RegionSet& intersectingGhostRegions = mCell->GridGet()->GhostRegionsManagerGet()->RegionsWithCellGet(mCell);
    for (Region* region : intersectingGhostRegions)
    {
        region->UpdateManagerGet()->OnCellOptionRemoved(mCell, mValue);
    }

    PrintMessage();
}

void Progress_LockedCandidates::ProcessProgress()
{
    for (const Region* region : mIntersectingRegions)
    {
        ScanLocked(mIntersection, region, mValue);
    }

    PrintMessage();
}

void Progress_LockedCandidates::PrintMessage() const
{
    SudokuSolverThread* st = (*mIntersectingRegions.begin())->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Locked " + std::to_string(mValue) + " in " + mDefiningRegion->RegionNameGet() + ". " +
                "Cover region(s): ";

        auto it = mIntersectingRegions.begin();
        while(it != mIntersectingRegions.end())
        {
            message += (*it)->RegionNameGet() + ",";
            it++;
        }
        message.pop_back();
        message += ".";

        st->NotifyLogicalDeduction(message);
    }
}

void Progress_FinnedLockedCandidates::ProcessProgress()
{
    for (SudokuCell* const & c : mCells)
    {
        c->RemoveOption(mValue);
    }

    PrintMessage();
}

void Progress_FinnedLockedCandidates::PrintMessage() const
{
    SudokuSolverThread* st = mRegion->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Value " + std::to_string(mValue) + " excluded from cells {";

        auto cIt = mCells.begin();
        while(cIt != mCells.end())
        {
            message += (*cIt)->CellNameGet() + ",";
            cIt++;
        }
        message.pop_back();
        message += "} as there would be no way to place " + std::to_string(mValue) +
                " in " + mRegion->RegionNameGet();

        st->NotifyLogicalDeduction(message);
    }
}

void Progress_Fish::ProcessProgress()
{
    // remove mValue from all the cells in the secondary set that are not in the defining set
    for (Region* r : mSecondaryRegions)
    {
        r->UpdateManagerGet()->OnFishFound(mDefiningCells, mValue);
    }

    PrintMessage();
}

void Progress_Fish::PrintMessage() const
{
    auto rIt = mSecondaryRegions.begin();
    SudokuSolverThread* st = (*rIt)->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Fish on " + std::to_string(mValue) +
                              ". Base regions: ";

        auto bIt = mDefiningRegions.begin();
        while(bIt != mDefiningRegions.end())
        {
            message += (*bIt)->RegionNameGet() + ",";
            bIt++;
        }
        message.pop_back();
        message += "; Cover regions: ";
        while(rIt != mSecondaryRegions.end())
        {
            message += (*rIt)->RegionNameGet() + ",";
            rIt++;
        }
        message.pop_back();
        message += ".";

        st->NotifyLogicalDeduction(message);
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

void Progress_CannibalFish::PrintMessage() const
{
    auto rIt = mSecondaryRegions.begin();
    SudokuSolverThread* st = (*rIt)->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Cannibal fish on " + std::to_string(mValue) +
                              ". Base regions: ";

        auto bIt = mDefiningRegions.begin();
        while(bIt != mDefiningRegions.end())
        {
            message += (*bIt)->RegionNameGet() + ",";
            bIt++;
        }
        message.pop_back();
        message += "; Cover regions: ";
        while(rIt != mSecondaryRegions.end())
        {
            message += (*rIt)->RegionNameGet() + ",";
            rIt++;
        }
        message.pop_back();
        message += "; Eaten cells: {";
        auto cIt = mCannibalCells.begin();
        while(cIt != mCannibalCells.end())
        {
            message += (*cIt)->CellNameGet() + ",";
            cIt++;
        }
        message.pop_back();
        message += "}.";

        st->NotifyLogicalDeduction(message);
    }
}

void Progress_FinnedFish::ProcessProgress()
{
    for (SudokuCell* const& c : mCellsSeingFins)
    {
        c->RemoveOption(mValue);
    }

    PrintMessage();
}

void Progress_FinnedFish::PrintMessage() const
{
    auto rIt = mSecondaryRegions.begin();
    SudokuSolverThread* st = (*rIt)->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Finned fish on " + std::to_string(mValue) +
                              ". Base regions: ";

        auto bIt = mDefiningRegions.begin();
        while(bIt != mDefiningRegions.end())
        {
            message += (*bIt)->RegionNameGet() + ",";
            bIt++;
        }
        message.pop_back();
        message += "; Cover regions: ";
        while(rIt != mSecondaryRegions.end())
        {
            message += (*rIt)->RegionNameGet() + ",";
            rIt++;
        }
        message.pop_back();
        message += "; Fins: ";

        auto cIt = mFins.begin();
        while(cIt != mFins.end())
        {
            message += (*cIt)->CellNameGet() + ",";
            cIt++;
        }
        message.pop_back();
        message += ".";

        st->NotifyLogicalDeduction(message);
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

void Progress_CannibalFinnedFish::PrintMessage() const
{
    auto rIt = mSecondaryRegions.begin();
    SudokuSolverThread* st = (*rIt)->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Cannibal finned fish on " + std::to_string(mValue) +
                              ". Base regions";

        auto bIt = mDefiningRegions.begin();
        while(bIt != mDefiningRegions.end())
        {
            message += (*bIt)->RegionNameGet() + ",";
            bIt++;
        }
        message.pop_back();
        message += "; Cover regions: ";
        while(rIt != mSecondaryRegions.end())
        {
            message += (*rIt)->RegionNameGet() + ",";
            rIt++;
        }
        message.pop_back();
        message += "; Fins: ";

        auto cIt = mFins.begin();
        while(cIt != mFins.end())
        {
            message += (*cIt)->CellNameGet() + ",";
            cIt++;
        }
        message.pop_back();
        message += "; Eaten cells: {";
        auto eIt = mCannibalCells.begin();
        while(eIt != mCannibalCells.end())
        {
            message += (*eIt)->CellNameGet() + ",";
            eIt++;
        }
        message.pop_back();
        message += ".";

        st->NotifyLogicalDeduction(message);
    }
}

void Progress_RegionBecameClosed::ProcessProgress()
{
    ScanNaked(mRegion->CellsGet(), mRegion->ConfirmedValuesGet(), mRegion->GridGet(), false);

    PrintMessage();
}

void Progress_RegionBecameClosed::PrintMessage() const
{
    SudokuSolverThread* st = mRegion->GridGet()->SolverThreadGet();
    if(st && !mRegion->IsHouse() && mRegion->IsStartingRegion())
    {
        std::string message = "->" + mRegion->RegionNameGet() + " forms a naked set.";
        st->NotifyLogicalDeduction(message);
    }
}

void Progress_ValueNotInKiller::ProcessProgress()
{
    mCell->RemoveMultipleOptions(mValues);

    PrintMessage();
}

void Progress_ValueNotInKiller::PrintMessage() const
{
    SudokuSolverThread* st = mCell->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Values {";
        auto vIt = mValues.begin();
        while(vIt != mValues.end())
        {
            message += std::to_string(*vIt) + ",";
            vIt++;
        }
        message.pop_back();
        message += "} removed from " + mCell->CellNameGet() +
                " as they break the sum in " + mRegion->RegionNameGet() + ".";

        st->NotifyLogicalDeduction(message);
    }
}

void Progress_ValueForcedInKiller::ProcessProgress()
{
    mRegion->AddConfirmedValue(mValue);

    PrintMessage();
}

void Progress_ValueDisallowedByBifurcation::ProcessProgress()
{
    mCell->RemoveMultipleOptions(mValues);

    PrintMessage();
}

void Progress_ValueDisallowedByBifurcation::PrintMessage() const
{
    SudokuSolverThread* st = mCell->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Values {";
        auto vIt = mValues.begin();
        while(vIt != mValues.end())
        {
            message += std::to_string(*vIt) + ",";
            vIt++;
        }
        message.pop_back();
        message += "} excluded from cell " + mCell->CellNameGet() +
                " after bifurcating cell " + mPivot->CellNameGet() + ".";

        st->NotifyLogicalDeduction(message);
    }
}

void Progress_OptionRemovedViaGuessing::ProcessProgress()
{
    mCell->RemoveOption(mValue);

    PrintMessage();
}

void Progress_OptionRemovedViaGuessing::PrintMessage() const
{
    SudokuSolverThread* st = mCell->GridGet()->SolverThreadGet();
    if(st)
    {
        std::string message = "->Value " + std::to_string(mValue) + " removed from " + mCell->CellNameGet() +
                "as it breaks the puzzle.";

        st->NotifyLogicalDeduction(message);
    }
}

void Progress_GhostCage::ProcessProgress()
{
    CellSet cells = mCells;
    RegionSPtr regionSPtr = std::make_shared<Region>(mGrid, std::move(cells), true);
    regionSPtr->AddVariantConstraint(std::make_unique<KillerConstraint>(mTotal));

    std::string type = mInnie ? "innie" : "outie";
    std::string name = "the " + std::to_string(mTotal) + " " + type + " at {";
    auto cIt = mCells.begin();
    while(cIt != mCells.end())
    {
        name += (*cIt)->CellNameGet() + ",";
        cIt++;
    }
    name.pop_back();
    name += "}";

    regionSPtr->RegionNameSet(name);
    mGrid->GhostRegionsManagerGet()->RegisterRegion(regionSPtr);

    PrintMessage();
}

void Progress_GhostCage::PrintMessage() const
{
    SudokuSolverThread* st = mGrid->SolverThreadGet();
    if(st)
    {
        std::string type = mInnie ? "Innie" : "Outie";
        std::string message = "->" + type + " cage added to the grid. Total: " + std::to_string(mTotal) + "; Cells: {";
        auto cIt = mCells.begin();
        while(cIt != mCells.end())
        {
            message += (*cIt)->CellNameGet() + ",";
            cIt++;
        }
        message.pop_back();
        message += "}.";

        st->NotifyLogicalDeduction(message);
    }
}

void Progress_SplitOutie::ProcessProgress()
{
    for (const auto& c : mCells)
    {
        for (unsigned short i = mMinAllowedValue - 1; i > 0; --i)
        {
            c->RemoveOption(i);
        }
        for (unsigned short i = mMaxAllowedValue + 1; i <= mGrid->SizeGet(); ++i)
        {
            c->RemoveOption(i);
        }
    }

    PrintMessage();
}

void Progress_SplitOutie::PrintMessage() const
{
    SudokuSolverThread* st = mGrid->SolverThreadGet();
    if(st)
    {
        std::string message = "->Broken outie in cells {";
        auto cIt = mCells.begin();
        while(cIt != mCells.end())
        {
            message += (*cIt)->CellNameGet() + ",";
            cIt++;
        }
        message.pop_back();
        message += "} has total " + std::to_string(mTotal) + ". Only values between " +
                std::to_string(mMinAllowedValue) + " and " + std::to_string(mMaxAllowedValue) + " are allowed.";

        st->NotifyLogicalDeduction(message);
    }
}







/*
 * ==========================================================
 * Impossible Puzzles
 * ==========================================================
*/
void Progress_ImpossiblePuzzle::ProcessProgress()
{
    mGrid->ProgressManagerGet()->Abort();

    PrintMessage();
}

void Progress_ImpossiblePuzzle::PrintMessage() const
{
    SudokuSolverThread* st = mGrid->SolverThreadGet();
    if(st)
    {
        std::string message = "The puzzle is broken!";
        st->NotifyImpossiblePuzzle(message);
    }
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

void Impossible_NoSolutionByBifurcation::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}

void Impossible_BrokenInnie::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}

void Impossible_ClashingGhostCages::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}

void Impossible_BrokenOutie::ProcessProgress()
{
    Progress_ImpossiblePuzzle::ProcessProgress();
}
