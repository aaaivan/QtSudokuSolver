#include "puzzledata.h"
#include <algorithm>

PuzzleData::PuzzleData(unsigned short size):
    mSize(size),
    mRegions(size),
    mKillerCages(),
    mGivens(),
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

const std::pair<unsigned int, CellsInRegion> PuzzleData::KillerCageGet(CellCoord id) const
{
    if(mKillerCages.count(id))
    {
        return mKillerCages.at(id);
    }
    return {};
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
