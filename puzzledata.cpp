#include "puzzledata.h"
#include <algorithm>

PuzzleData::PuzzleData(unsigned short size):
    mSize(size),
    mRegions(size),
    mKillerCages(),
    mGivens()
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
