#ifndef SUDOKU_TYPES_H
#define SUDOKU_TYPES_H

#define PRINT_LOG_MESSAGES 0

// Includes
#include "ProgressTypes.h"
#include <memory>
#include <list>
#include <set>
#include <map>
#include <vector>

// Classes
class SudokuCell;
class Region;
class SudokuGrid;
class RegionsManager;
class GridProgressManager;

//Typedefs
typedef std::unique_ptr<SudokuCell> CellUPtr;
typedef std::unique_ptr<RegionsManager> RegionsManagerUPtr;
typedef std::unique_ptr<GridProgressManager> GridProgressManagerUPtr;
//typedef std::unique_ptr<Region> RegionUPtr;
typedef std::shared_ptr<Region> RegionSPtr;
typedef std::set<Region*> RegionSet;
typedef std::list<Region*> RegionList;
typedef std::set<SudokuCell*> CellSet;
typedef std::list<SudokuCell*> CellList;
typedef std::pair<unsigned short, CellSet> ValueMapEntry;

// enums
enum class RegionType : int
{
    House_Row,
    House_Column,
    House_Region,
    Generic_region,

    KillerCage,

    MAX_TYPES
};
#endif // SUDOKU_TYPES_H
