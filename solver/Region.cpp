#include "Region.h"
#include "SudokuCell.h"
#include "SudokuGrid.h"
#include "RegionUpdatesManager.h"
#include "GridProgressManager.h"
#include <algorithm>
#include <iterator>
#include <cassert>
#include <sstream>

Region::Region(SudokuGrid* parentGrid, CellSet&& cells, bool startingRegion):
    mId(),
    mCells(cells),
    mConfirmedValues(),
    mAllowedValues(),
    mValueToCellMap(),
    mParentGrid(parentGrid),
    mIsStartingRegion(startingRegion),
    mSize(cells.size()),
    mUpdateManager(std::make_unique<RegionUpdatesManager>(this)),
    mRightNode(nullptr),
    mLeftNode(nullptr),
    mParents()
{
    // create region id
    std::vector<unsigned short> cellIds;
    std::string delimiter = ".";
    std::stringstream ss;
    for (const auto& c : mCells)
    {
        cellIds.push_back(c->IdGet());
    }
    std::sort(cellIds.begin(), cellIds.end());
    std::copy(cellIds.begin(), cellIds.end(), std::ostream_iterator<unsigned short>(ss, delimiter.c_str()));
    mId = ss.str();

    Init();
}

Region::~Region()
{
}

void Region::Init()
{
    mConfirmedValues.clear();
    mAllowedValues.clear();
    mValueToCellMap.clear();
    mUpdateManager->Reset();
    mRightNode.reset();
    mLeftNode.reset();

    // construct the value to cell map and find the allowed and confirmed values if any
    for (unsigned short v = 1; v <= mParentGrid->SizeGet(); ++v)
    {
        mValueToCellMap[v] = CellSet();
    }
    for (SudokuCell* const c : mCells)
    {
        for (const unsigned short& v : c->OptionsGet())
        {
            mAllowedValues.insert(v);
            mValueToCellMap.at(v).insert(c);
        }
    }

    if (mAllowedValues.size() == mCells.size())
    {
        for (const unsigned short& v : mAllowedValues)
        {
            AddConfirmedValue(v);
        }
    }
    else if (mAllowedValues.size() < mCells.size())
    {
        mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_TooFewValuesForRegion>(mCells, mAllowedValues, mParentGrid));
    }

    for(const auto& constraint : mAdditionalConstraints)
    {
        constraint->Initialise(this);
    }
}

std::string Region::IdGet() const
{
    return mId;
}

const CellSet& Region::CellsGet() const
{
    return mCells;
}

unsigned short Region::SizeGet() const
{
    return mSize;
}

const std::set<unsigned short>& Region::ConfirmedValuesGet() const
{
    return mConfirmedValues;
}

const std::set<unsigned short>& Region::AllowedValuesGet() const
{
    return mAllowedValues;
}

const CellSet& Region::CellsWithValueGet(unsigned short value) const
{
    return mValueToCellMap.at(value);
}

bool Region::HasConfirmedValue(unsigned short value) const
{
    if (IsLeafNode())
    {
        return mConfirmedValues.find(value) != mConfirmedValues.end();
    }
    else
    {
        return mLeftNode->HasConfirmedValue(value) || mRightNode->HasConfirmedValue(value);
    }
}

bool Region::ContainsRegion(Region *r)
{
    for(const auto& c : r->CellsGet())
    {
        if(mCells.count(c) == 0)
        {
            return false;
        }
    }
    return true;
}

bool Region::IsValueAllowed(unsigned short value) const
{
    if (IsLeafNode())
    {
        return mAllowedValues.find(value) != mAllowedValues.end();
    }
    else
    {
        return mLeftNode->IsValueAllowed(value) || mRightNode->IsValueAllowed(value);
    }
}

SudokuGrid* Region::GridGet() const
{
    return mParentGrid;
}

RegionUpdatesManager* Region::UpdateManagerGet() const
{
    return mUpdateManager.get();
}

const RegionList& Region::ParentNodesGet() const
{
    return mParents;
}

Region* Region::LeftNodeGet() const
{
    return mLeftNode.get();
}

Region* Region::RightNodeGet() const
{
    return mRightNode.get();
}

bool Region::IsLeafNode() const
{
    return mLeftNode == nullptr && mRightNode == nullptr;
}

void Region::LeafNodesGet(RegionSet& regions)
{
    if (IsLeafNode())
    {
        regions.insert(this);
    }
    else
    {
        mLeftNode->LeafNodesGet(regions);
        mRightNode->LeafNodesGet(regions);
    }
}

RegionSPtr Region::RegionSharedPtrGet() const
{
    RegionSPtr result;
    if (mParents.size() > 0) // the region is not a starting region
    {
        auto it = mParents.begin();
        if ((*it)->LeftNodeGet() == this)
        {
            result = (*it)->mLeftNode;
        }
        else if ((*it)->RightNodeGet() == this)
        {
            result = (*it)->mRightNode;
        }
    }
    return result;
}

Region* Region::FindLeafNodeWithCell(const SudokuCell* cell)
{
    Region* result = nullptr;
    if (std::find(mCells.begin(), mCells.end(), cell) != mCells.end())
    {
        if (IsLeafNode())
        {
            result = this;
        }
        else
        {
            result = mLeftNode->FindLeafNodeWithCell(cell);
            if (result == nullptr)
            {
                result = mRightNode->FindLeafNodeWithCell(cell);
            }
        }
    }
    return result;
}

const VariantConstraint* Region::GetConstraintByType(RegionType type) const
{
    auto it = mAdditionalConstraints.begin();
    while (it != mAdditionalConstraints.end())
    {
        if (it->get()->TypeGet() == type)
        {
            return it->get();
        }
        ++it;
    }
    return nullptr;
}

bool Region::IsStartingRegion() const
{
    return mIsStartingRegion;
}

bool Region::IsClosed() const
{
    return mCells.size() == mConfirmedValues.size() && mCells.size() == mAllowedValues.size();
}

bool Region::PartitionRegion(const RegionSPtr& leftNode, Region*& outRightNode)
{
    // Assumption: the cells in the left and right node have no viable option in common.
    // This function must be called AFTER the allowed values in the left node have been removed from the right node!
    bool result = false;
    outRightNode = nullptr;
    if (leftNode.get() != this && this->IsLeafNode())
    {
        CellSet rightRegionCells;
        for (SudokuCell* cell : mCells)
        {
            if (!leftNode || leftNode->CellsGet().count(cell) == 0)
            {
                rightRegionCells.emplace(cell);
            }
        }

        // check that every cell ends up in either of the two child nodes
        size_t leftSize = leftNode ? leftNode->mCells.size() : 0;
        if (rightRegionCells.size() + leftSize == mCells.size())
        {
            result = true;
            if (rightRegionCells.size() > 0 && leftSize > 0) // the left node does not already include all the cells in this region
            {
                mLeftNode = leftNode;
                mRightNode = std::make_shared<Region>(mParentGrid, std::move(rightRegionCells), false);
                mLeftNode->mParents.push_back(this);
                mRightNode->mParents.push_back(this);

                // partition the confirmed values between the two child regions
                for (const unsigned short& v : mConfirmedValues)
                {
                    if (mLeftNode->AllowedValuesGet().count(v) > 0)
                    {
                        mLeftNode->AddConfirmedValue(v);
                    }
                    else if (mRightNode->AllowedValuesGet().count(v) > 0)
                    {
                        mRightNode->AddConfirmedValue(v);
                    }
                }

                mUpdateManager->OnRegionPartitioned();

                // Notify the additonal constraints of the partition
                for (const auto& constraint : mAdditionalConstraints)
                {
                    constraint->OnRegionPartitioned(mLeftNode.get(), mRightNode.get());
                }

                outRightNode = mRightNode.get();
            }
            else if (leftSize > 0)
            {
                mLeftNode = leftNode;
                mLeftNode->mParents.push_back(this);

                // partition the confirmed values between the two child regions
                for (const unsigned short& v : mConfirmedValues)
                {
                    if (mLeftNode->AllowedValuesGet().count(v) > 0)
                    {
                        mLeftNode->AddConfirmedValue(v);
                    }
                }
                mUpdateManager->OnRegionPartitioned();

                // Notify the additonal constraints of the partition
                for (const auto& constraint : mAdditionalConstraints)
                {
                    constraint->OnRegionPartitioned(mLeftNode.get(), mRightNode.get());
                }
            }
            else
            {
                auto rightNode = std::make_shared<Region>(mParentGrid, std::move(rightRegionCells), false);
                MergeRegions(rightNode);
                outRightNode = mRightNode.get();
            }
        }
    }
    return result;
}

void Region::MergeRegions(const RegionSPtr& rightNode)
{
    if (rightNode.get() == this)
        return;

    // this assumes that this and right node are made up of the same cells.
    // this needs to be checked beforehand!
    mRightNode = rightNode;
    mRightNode->mParents.push_back(this);

    // partition the confirmed values between the two child regions
    for (const unsigned short& v : mConfirmedValues)
    {
        if (mRightNode->AllowedValuesGet().count(v) > 0)
        {
            mRightNode->AddConfirmedValue(v);
        }
    }
    mUpdateManager->OnRegionPartitioned();

    // Notify the additonal constraints of the partition
    for (const auto& constraint : mAdditionalConstraints)
    {
        constraint->OnRegionPartitioned(mLeftNode.get(), mRightNode.get());
    }
}

void Region::AddConfirmedValue(unsigned value)
{
    if (mAllowedValues.count(value) == 0) // the value we are trying to add must be in the allowed values
    {
        mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_NoCellLeftForValueInRegion>(this, value, mParentGrid));
        return;
    }

    if (mConfirmedValues.insert(value).second)
    {
        // Notify the region update manager of the new confirmed value
        mUpdateManager->OnConfirmedValueAdded(value);

        // have we found a hidden single?
        if (mValueToCellMap.at(value).size() == 1)
        {
            SudokuCell* nakedSingle = *(mValueToCellMap.at(value).begin());
            if(!nakedSingle->IsSolved())
            {
                mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_SingleCellForOption>(nakedSingle, value));
            }
        }

        // Notify the additonal constraints of the new confirmed value
        for (const auto& constraint : mAdditionalConstraints)
        {
            constraint->OnConfimedValueAdded(value);
        }

        if (mConfirmedValues.size() == mCells.size())
        {
            for (unsigned short v = 1; v <= mSize; ++v)
            {
                if (mConfirmedValues.count(v) == 0)
                {
                    RemoveAllowedValue(v);
                }
            }
            mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared <Progress_RegionBecameClosed>(this));
        }
        else if (mConfirmedValues.size() > mCells.size())
        {
            mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_TooManyValuesForRegion>(mCells, mConfirmedValues, mParentGrid));
        }
    }
}

void Region::UpdateValueMap(unsigned short removedValue, SudokuCell* removedFrom)
{
    if (mValueToCellMap.at(removedValue).erase(removedFrom) > 0)
    {
        // have we found a hidden single?
        if (HasConfirmedValue(removedValue) && mValueToCellMap.at(removedValue).size() == 1)
        {
            SudokuCell* nakedSingle = *(mValueToCellMap.at(removedValue).begin());
            if(!nakedSingle->IsSolved())
            {
                mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_SingleCellForOption>(nakedSingle, removedValue));
            }
        }
        // Notify the additonal constraints of the removed option
        for (const auto& constraint : mAdditionalConstraints)
        {
            constraint->OnOptionRemovedFromCell(removedValue, removedFrom);
        }
    }
    if (mValueToCellMap.at(removedValue).empty())
    {
        RemoveAllowedValue(removedValue);
    }
}

void Region::AddVariantConstraint(std::unique_ptr<VariantConstraint> constraint)
{
    if (constraint)
    {
        mAdditionalConstraints.push_back(std::move(constraint));
        mAdditionalConstraints.rbegin()->get()->Initialise(this);
    }
}

void Region::Reset()
{
    Init();
}

void Region::RemoveAllowedValue(unsigned short value)
{
    if (mConfirmedValues.count(value) > 0) // the value we are trying to remove must NOT be in the confirmed values
    {
        mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_NoCellLeftForValueInRegion>(this, value, mParentGrid));
        return;
    }

    if (mAllowedValues.erase(value) > 0)
    {
        // Notify the additonal constraints of the removed option
        for (const auto& constraint : mAdditionalConstraints)
        {
            constraint->OnAllowedValueRemoved(value);
        }

        // the number of allowed values matches the size of the region
        if (mAllowedValues.size() == mCells.size())
        {
            for (const unsigned short& v : mAllowedValues)
            {
                AddConfirmedValue(v);
            }
        }
    }
}
