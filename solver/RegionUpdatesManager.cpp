#include "RegionUpdatesManager.h"
#include "Region.h"
#include "SudokuCell.h"
#include "SudokuGrid.h"
#include "GridProgressManager.h"
#include "SolvingTechnique.h"
#include <cassert>

RegionUpdatesManager::RegionUpdatesManager(Region* region):
	mRegion(region),
	mHotCells(),
	mHotValues()
{
	for (int i = 0; i < static_cast<int>(TechniqueType::MAX_TECHNIQUES); ++i)
	{
		if (mRegion->GridGet()->ProgressManagerGet()->TechniqueGet(static_cast<TechniqueType>(i))
			->ObservedComponentGet() == SolvingTechnique::ObserveCells)
		{
			mHotCells[static_cast<TechniqueType>(i)] = CellSet();
		}
		else if (mRegion->GridGet()->ProgressManagerGet()->TechniqueGet(static_cast<TechniqueType>(i))
			->ObservedComponentGet() == SolvingTechnique::ObserveValues)
		{
			mHotValues[static_cast<TechniqueType>(i)] = std::set<unsigned short>();
		}
	}
}

bool RegionUpdatesManager::IsRegionReadyForTechnique(TechniqueType type, std::set<SudokuCell*>& outCells) const
{
	assert(mRegion->IsLeafNode());

	if (mHotCells.count(type))
	{
		outCells = mHotCells.at(type);
		return outCells.size() > 0;
	}
	return false;
}

bool RegionUpdatesManager::IsRegionReadyForTechnique(TechniqueType type, SudokuCell* cell) const
{
	assert(mRegion->IsLeafNode());

	if (mHotCells.count(type))
	{
		return mHotCells.at(type).count(cell);
	}
	return false;
}

bool RegionUpdatesManager::IsRegionReadyForTechnique(TechniqueType type, std::set<unsigned short>& outValues) const
{
	assert(mRegion->IsLeafNode());

	if (mHotValues.count(type))
	{
		outValues = mHotValues.at(type);
		return outValues.size() > 0;
	}
	return false;
}

bool RegionUpdatesManager::IsRegionReadyForTechnique(TechniqueType type, unsigned short value) const
{
	assert(mRegion->IsLeafNode());

	if (mHotValues.count(type))
	{
		return mHotValues.at(type).count(value);
	}
	return false;
}

void RegionUpdatesManager::OnNakedSetFound(const CellSet& nakedSubset, const std::set<unsigned short>& values, bool isHidden)
{
	if (!mRegion->IsLeafNode()) // call OnNakedSubsetFound on each child node
	{
		CellSet leftNakedSubset;
		CellSet rightNakedSubset;
		std::set<unsigned short> leftValues;
		std::set<unsigned short> rightValues;
		auto leftNode = mRegion->LeftNodeGet();
		auto rightNode = mRegion->RightNodeGet();

		for (auto it = nakedSubset.begin(), end = nakedSubset.end(); it != end; ++it)
		{
			if (leftNode && leftNode->CellsGet().count(*it))
			{
				leftNakedSubset.insert(*it);
			}
			else if (rightNode && rightNode->CellsGet().count(*it))
			{
				rightNakedSubset.insert(*it);
			}
		}
		for (auto it = values.begin(), end = values.end(); it != end; ++it)
		{
			if (leftNode && leftNode->AllowedValuesGet().count(*it))
			{
				leftValues.insert(*it);
			}
			else if (rightNode && rightNode->AllowedValuesGet().count(*it))
			{
				rightValues.insert(*it);
			}
		}
		if (leftNakedSubset.size() > 0 && leftValues.size() == leftNakedSubset.size())
		{
			leftNode->UpdateManagerGet()->OnNakedSetFound(leftNakedSubset, leftValues, isHidden);
		}
		if (rightNakedSubset.size() > 0 && rightValues.size() == rightNakedSubset.size())
		{
			rightNode->UpdateManagerGet()->OnNakedSetFound(rightNakedSubset, rightValues, isHidden);
		}

		return;
	}

	for (SudokuCell* cell : mRegion->CellsGet())
	{
		// for the cells in the naked subset:
		// remove all the options other than the ones making up the naked subset
		if (nakedSubset.count(cell) > 0)
		{
			cell->RemoveAllOtherOptions(values);
		}
		// for the cells NOT in the naked subset:
		// remove all the options making up the naked subset
		else
		{
			cell->RemoveMultipleOptions(values);
		}
	}

	if (nakedSubset.size() < mRegion->CellsGet().size())
	{
		if (isHidden)
		{
			for (const auto& v : values)
			{
				mHotValues.at(TechniqueType::HiddenNakedSubset).erase(v);
			}
		}
		else
		{
			for (const auto& c : nakedSubset)
			{
				mHotCells.at(TechniqueType::NakedSubset).erase(c);
			}
		}
	}
}

void RegionUpdatesManager::OnCellOptionRemoved(SudokuCell* cell, unsigned short value)
{
	if (!mRegion->IsLeafNode()) // call OnCellOptionRemoved on the appropriate child node
	{
		auto leftNode = mRegion->LeftNodeGet();
		auto rightNode = mRegion->RightNodeGet();

		if (leftNode && leftNode->CellsGet().count(cell))
		{
			leftNode->UpdateManagerGet()->OnCellOptionRemoved(cell, value);
		}
		else if (rightNode && rightNode->CellsGet().count(cell))
		{
			rightNode->UpdateManagerGet()->OnCellOptionRemoved(cell, value);
		}
		return;
	}

	if (mRegion->CellsGet().count(cell) > 0)
	{
		auto it = mHotCells.begin();
		while (it != mHotCells.end())
		{
			it->second.insert(cell);
			++it;
		}
	}
	if(mRegion->HasConfirmedValue(value))
	{
		auto it = mHotValues.begin();
		while (it != mHotValues.end())
		{
			it->second.insert(value);
			++it;
		}
	}

	mRegion->UpdateValueMap(value, cell);
}

void RegionUpdatesManager::OnLockedCandidatesFound(const CellSet& intersection, unsigned short lockedValue)
{
	if (!mRegion->IsLeafNode()) // call OnLockedCandidatesFound on the appropriate child node
	{
		Region* childNode = nullptr;
		auto leftNode = mRegion->LeftNodeGet();
		auto rightNode = mRegion->RightNodeGet();

		if (leftNode && leftNode->AllowedValuesGet().count(lockedValue))
		{
			childNode = leftNode;
		}
		else if (rightNode && rightNode->AllowedValuesGet().count(lockedValue))
		{
			childNode = rightNode;
		}

		if (!childNode)
		{
			return;
		}

		CellSet childIntersection;
		for (auto it = intersection.begin(), end = intersection.end(); it != end; ++it)
		{
			if (childNode->CellsGet().count(*it))
			{
				childIntersection.insert(*it);
			}
		}
		if (childNode && childIntersection.size() > 0)
		{
			childNode->UpdateManagerGet()->OnLockedCandidatesFound(childIntersection, lockedValue);
		}

		return;
	}

	mRegion->AddConfirmedValue(lockedValue);
	for (SudokuCell* cell : mRegion->CellsGet())
	{
		if (intersection.count(cell) == 0)
		{
			cell->RemoveOption(lockedValue);
		}
	}
	mHotValues.at(TechniqueType::LockedCandidates).erase(lockedValue);
}

void RegionUpdatesManager::OnFishFound(const CellSet& definingSet, unsigned short fishValue)
{
	if (!mRegion->IsLeafNode()) // call OnFishFound on the appropriate child node
	{
		auto leftNode = mRegion->LeftNodeGet();
		auto rightNode = mRegion->RightNodeGet();

		if (leftNode && leftNode->AllowedValuesGet().count(fishValue))
		{
			leftNode->UpdateManagerGet()->OnFishFound(definingSet, fishValue);
		}
		else if (rightNode && rightNode->AllowedValuesGet().count(fishValue))
		{
			rightNode->UpdateManagerGet()->OnFishFound(definingSet, fishValue);
		}

		return;
	}

	for (SudokuCell* c : mRegion->CellsGet())
	{
		if (definingSet.count(c) == 0)
		{
			c->RemoveOption(fishValue);
		}
	}
}

void RegionUpdatesManager::OnTechniqueFailed(TechniqueType type)
{
	assert(mRegion->IsLeafNode());

	{
		auto it = mHotCells.find(type);
		if (it != mHotCells.end())
		{
			it->second.clear();
		}
	}
	{
		auto it = mHotValues.find(type);
		if (it != mHotValues.end())
		{
			it->second.clear();
		}
	}
}

void RegionUpdatesManager::OnTechniqueFailed(TechniqueType type, SudokuCell* cell)
{
	assert(mRegion->IsLeafNode());

	{
		auto it = mHotCells.find(type);
		if (it != mHotCells.end())
		{
			it->second.erase(cell);
		}
	}
}

void RegionUpdatesManager::OnTechniqueFailed(TechniqueType type, unsigned short value)
{
	assert(mRegion->IsLeafNode());

	{
		auto it = mHotValues.find(type);
		if (it != mHotValues.end())
		{
			it->second.erase(value);
		}
	}
}

void RegionUpdatesManager::ResetHotArrays()
{
	{
		auto it = mHotCells.begin();
		while (it != mHotCells.end())
		{
			it->second.clear();
			++it;
		}
	}
	{
		auto it = mHotValues.begin();
		while (it != mHotValues.end())
		{
			it->second.clear();
			++it;
		}
	}
}

void RegionUpdatesManager::OnRegionPartitioned()
{
	// partition the hot cells and hot values between the two newly created regions
	auto leftNode = mRegion->LeftNodeGet();
	auto rightNode = mRegion->RightNodeGet();

	RegionUpdatesManager* leftManager = leftNode ? leftNode->UpdateManagerGet() : nullptr;
	RegionUpdatesManager* rightManager = rightNode ? mRegion->UpdateManagerGet() : nullptr;

	{
		auto it = mHotCells.begin();
		while (it != mHotCells.end())
		{
			for (SudokuCell* const& c : it->second)
			{
				if (leftNode && leftNode->CellsGet().count(c) > 0)
				{
					leftManager->mHotCells.at(it->first).insert(c);
				}
				else if(rightNode && rightNode->CellsGet().count(c) > 0)
				{
					rightManager->mHotCells.at(it->first).insert(c);
				}
			}
			++it;
		}
	}
	{
		auto it = mHotValues.begin();
		while (it != mHotValues.end())
		{
			for (unsigned short const& v : it->second)
			{
				if (leftNode && leftNode->AllowedValuesGet().count(v) > 0)
				{
					leftManager->mHotValues.at(it->first).insert(v);
				}
				else if (rightNode && rightNode->AllowedValuesGet().count(v) > 0)
				{
					rightManager->mHotValues.at(it->first).insert(v);
				}
			}
			++it;
		}
	}

	// if any of the new regions has size 1,
	// clear all the hot cells and hot values
	if (leftNode && leftNode->CellsGet().size() == 1)
	{
		leftManager->ResetHotArrays();
	}
	if (rightNode && rightNode->CellsGet().size() == 1)
	{
		rightManager->ResetHotArrays();
	}
}

void RegionUpdatesManager::OnConfirmedValueAdded(unsigned short value)
{
	auto it = mHotValues.begin();
	while (it != mHotValues.end())
	{
		it->second.insert(value);
		++it;
	}
}

void RegionUpdatesManager::Reset()
{
	for (auto& cells : mHotCells)
	{
		cells.second.clear();
	}
	for (auto& values : mHotValues)
	{
		values.second.clear();
	}
}
