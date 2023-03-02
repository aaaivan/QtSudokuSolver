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
    bool mPositiveDiagonal;
    bool mNegativeDiagonal;

public:
    unsigned short CellCountInRegion(unsigned short regionId) const;
    bool HasPositiveDiagonalConstraint() const;
    bool HasNegativeDiagonalConstraint() const;

    void AddCellToRegion(unsigned short regionId, unsigned short x, unsigned short y);
    void RemoveCellFromRegion(unsigned short regionId, unsigned short x, unsigned short y);
    void PositiveDiagonalConstraintSet(bool set);
    void NegativeDiagonalConstraintSet(bool set);
};

#endif // PUZZLEDATA_H
