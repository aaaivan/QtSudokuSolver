#ifndef PUZZLEDATA_H
#define PUZZLEDATA_H

#include <vector>
#include <map>
#include <set>

typedef unsigned int CellCoord;
typedef std::set<CellCoord> CellsInRegion;

class PuzzleData
{
public:
    PuzzleData(unsigned short size);

private:
    unsigned short mSize;
    std::vector<CellsInRegion> mRegions;
    std::map<CellCoord, std::pair<unsigned int, CellsInRegion>> mKillerCages;
    std::map<CellCoord, unsigned short> mGivens;
    bool mPositiveDiagonal;
    bool mNegativeDiagonal;

public:
    unsigned short CellCountInRegion(unsigned short regionId) const;
    bool HasPositiveDiagonalConstraint() const;
    bool HasNegativeDiagonalConstraint() const;
    const std::pair<unsigned int, CellsInRegion> KillerCageGet(CellCoord id) const;

    void AddCellToRegion(unsigned short regionId, CellCoord cellId);
    void RemoveCellFromRegion(unsigned short regionId, CellCoord cellId);
    void PositiveDiagonalConstraintSet(bool set);
    void NegativeDiagonalConstraintSet(bool set);
    void AddGiven(unsigned short value, CellCoord cellId);
    void RemoveGiven(CellCoord cellId);
    void AddCellToKillerCage(CellCoord cageId, CellCoord cellId);
    void RemoveCellFromKillerCage(CellCoord cageId, CellCoord cellId);
    void AddKillerCage(CellCoord cageId, const std::pair<unsigned int, CellsInRegion>& cageContent);
    void RemoveKillerCage(CellCoord cageId);
    void KillerCageTotalSet(CellCoord cageId, CellCoord cellId);
};

#endif // PUZZLEDATA_H
