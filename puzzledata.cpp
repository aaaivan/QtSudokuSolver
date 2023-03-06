#include "puzzledata.h"
#include <algorithm>

PuzzleData::PuzzleData(unsigned short size):
    mSize(size),
    mRegions(size),
    mKillerCages(),
    mGivens(),
    mHints(),
    mPositiveDiagonal(false),
    mNegativeDiagonal(false)
{
}

unsigned short PuzzleData::CellCountInRegion(unsigned short regionId) const
{
    unsigned short index = regionId - 1;
    if(index < mSize)
    {
        return mRegions[index].size();
    }
    return 0;
}

bool PuzzleData::HasPositiveDiagonalConstraint() const
{
    return mPositiveDiagonal;
}

bool PuzzleData::HasNegativeDiagonalConstraint() const
{
    return mNegativeDiagonal;
}

void PuzzleData::KillerCageGet(CellCoord id, std::pair<unsigned int, CellsInRegion> outCage) const
{
    if(mKillerCages.count(id))
    {
        outCage = mKillerCages.at(id);
    }
}

void PuzzleData::HintsGet(CellCoord id, std::set<unsigned short>& outHints) const
{
    if(mHints.count(id))
    {
        outHints = mHints.at(id);
    }
}

void PuzzleData::AddCellToRegion(unsigned short regionId, CellCoord cellId)
{
    unsigned short index = regionId - 1;
    if(index < mSize)
    {
        mRegions[index].insert(cellId);
    }
}

void PuzzleData::RemoveCellFromRegion(unsigned short regionId, CellCoord  cellId)
{
    unsigned short index = regionId - 1;
    if(index < mSize)
    {
        if(auto it = mRegions[index].find(cellId); it != mRegions[index].end())
        {
            mRegions[index].erase(it);
        }
    }
}

void PuzzleData::PositiveDiagonalConstraintSet(bool set)
{
    mPositiveDiagonal = set;
}

void PuzzleData::NegativeDiagonalConstraintSet(bool set)
{
    mNegativeDiagonal = set;
}

void PuzzleData::AddGiven(unsigned short value, CellCoord  cellId)
{
    if(value > 0 && value <= mSize)
    {
        mGivens[cellId] = value;
    }
}

void PuzzleData::RemoveGiven(CellCoord  cellId)
{
    auto it = mGivens.find(cellId);
    if(it != mGivens.end())
    {
        mGivens.erase(it);
    }
}

void PuzzleData::AddCellToKillerCage(CellCoord cageId, CellCoord  cellId)
{
    auto it = mKillerCages.find(cageId);
    if(it != mKillerCages.end())
    {
        it->second.second.insert(cellId);
    }
    else
    {
        mKillerCages[cageId] = {0, {cellId}};
    }
}

void PuzzleData::RemoveCellFromKillerCage(CellCoord cageId, CellCoord  cellId)
{
    auto mapIt = mKillerCages.find(cageId);
    if(mapIt != mKillerCages.end())
    {
        auto setIt = mapIt->second.second.find(cellId);
        if(setIt != mapIt->second.second.end())
        {
            mapIt->second.second.erase(setIt);
            if(mapIt->second.second.size() == 0)
            {
                mKillerCages.erase(mapIt);
            }
        }
    }
}

void PuzzleData::AddKillerCage(CellCoord cageId, const std::pair<unsigned int, CellsInRegion> &cage)
{
    mKillerCages[cageId] = cage;
}

void PuzzleData::RemoveKillerCage(CellCoord cageId)
{

    auto it = mKillerCages.find(cageId);
    if(it != mKillerCages.end())
    {
        mKillerCages.erase(it);
    }
}

void PuzzleData::KillerCageTotalSet(CellCoord cageId, unsigned int total)
{
    auto it = mKillerCages.find(cageId);
    if(it != mKillerCages.end())
    {
        it->second.first = total;
    }
    else
    {
        mKillerCages[cageId] = {total, {}};
    }
}

void PuzzleData::AddHint(CellCoord cageId, unsigned short value)
{
    auto it = mHints.find(cageId);
    if(it != mHints.end())
    {
        it->second.insert(value);
    }
    else
    {
        mHints[cageId] = {value};
    }
}

void PuzzleData::RemoveHint(CellCoord cageId, unsigned short value)
{
    auto mapIt = mHints.find(cageId);
    if(mapIt != mHints.end())
    {
        auto setIt = mapIt->second.find(value);
        if(setIt != mapIt->second.end())
        {
            mapIt->second.erase(setIt);
        }
    }
}

void PuzzleData::RemoveAllHints()
{
    mHints.clear();
}
