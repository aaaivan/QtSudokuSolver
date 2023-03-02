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

void PuzzleData::AddCellToRegion(unsigned short regionId, unsigned short x, unsigned short y)
{
    unsigned short index = regionId - 1;
    if(index < mSize)
    {
        CellCoord cell{x, y};
        mRegions[index].insert(std::move(cell));
    }
}

void PuzzleData::RemoveCellFromRegion(unsigned short regionId, unsigned short x, unsigned short y)
{
    unsigned short index = regionId - 1;
    if(index < mSize)
    {
        const CellCoord cell{x, y};
        if(auto it = mRegions[index].find(cell); it != mRegions[index].end())
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
