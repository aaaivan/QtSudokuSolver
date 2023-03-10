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
    enum Diagonal
    {
        Diagonal_Negative,
        Diagonal_Positive
    };

    PuzzleData(unsigned short size);

    unsigned short mSize;
    std::vector<CellsInRegion> mRegions;
    std::map<CellCoord, std::pair<unsigned int, CellsInRegion>> mKillerCages;
    std::map<CellCoord, unsigned short> mGivens;
    std::map<CellCoord, std::set<unsigned short>> mHints;
    bool mPositiveDiagonal;
    bool mNegativeDiagonal;
};

#endif // PUZZLEDATA_H
