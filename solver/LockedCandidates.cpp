#include "SolvingTechnique.h"
#include "SudokuCell.h"
#include "Region.h"
#include "GridProgressManager.h"
#include "RegionsManager.h"
#include "SudokuGrid.h"
#include <iterator>


void LockedCandidatesTechnique::SearchLockedCandidates(unsigned short value)
{
	if (!mCurrentRegion->HasConfirmedValue(value))
	{
		mGrid->ProgressManagerGet()->RegisterFailure(TechniqueType::LockedCandidates, mCurrentRegion, nullptr, value);
		return;
	}

	RegionSet lockedRegions; // regions that contain all cells with v in region
	mGrid->RegionsManagerGet()->RegionsWithCellsGet(lockedRegions, mCurrentRegion->CellsWithValueGet(value));
	CellSet almostLockedCells; // cells that can see all cells with v in region
	mGrid->RegionsManagerGet()->FindConnectedCellsWithValue(mCurrentRegion->CellsWithValueGet(value), almostLockedCells, value);
	
	for (auto it = lockedRegions.begin(), end = lockedRegions.end(); it != end;)
	{
		if ((*it)->CellsWithValueGet(value).size() == mCurrentRegion->CellsWithValueGet(value).size() && (*it)->HasConfirmedValue(value))
		{
			mGrid->ProgressManagerGet()->RegisterFailure(TechniqueType::LockedCandidates, *it, nullptr, value);
			it = lockedRegions.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (lockedRegions.size() > 0)
	{
		mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_LockedCandidates>(std::move(lockedRegions), mCurrentRegion->CellsWithValueGet(value), value));
	}
	if (almostLockedCells.size() > 0)
	{
		mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_AlmostLockedCandidates>(mCurrentRegion, std::move(almostLockedCells), value));
	}
}
