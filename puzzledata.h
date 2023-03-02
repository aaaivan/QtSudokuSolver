#ifndef PUZZLEDATA_H
#define PUZZLEDATA_H

#include <vector>
#include <set>

typedef std::pair<unsigned short, unsigned short> CellCoord;
typedef std::set<CellCoord> CellsInRegion;

class PuzzleData
{
public:
    PuzzleData(unsigned short size);

private:
    unsigned short mSize;
    std::vector<CellsInRegion> mRegions;
    std::vector<std::pair<int, CellsInRegion>> mKillerCages;
    std::vector<std::pair<unsigned short, CellCoord>> mGivens;

public:
    unsigned short CellCountInRegion(unsigned short regionId) const;

    void AddCellToRegion(unsigned short regionId, unsigned short x, unsigned short y);
    void RemoveCellFromRegion(unsigned short regionId, unsigned short x, unsigned short y);
};

#endif // PUZZLEDATA_H
