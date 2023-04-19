#ifndef SAVE_LOAD_MANAGER_H
#define SAVE_LOAD_MANAGER_H

#include "puzzledata.h"
#include <fstream>
#include <memory>
#include <sstream>
#include <map>
#include <vector>
#include <functional>

class PuzzleData;

class SaveLoadManager
{
    enum class ValueType
    {
        Option,
        GivenCell,
        HintedElimination
    };
    static const std::map<ValueType, std::string> sCandidateIdentifiersMap;		// store the character that identifies the value type

    enum class RegionType
    {
        Region,
        Killer,
        PositiveDiagonal,
        NegativeDiagonal
    };
    static const std::map<RegionType, std::string> sConstraintIdentifiersMap;	// store the character that identifies the region type

    static const std::string sSectionEndString;
    static const std::string sGridContentsString;
    static const std::string sConstraintsString;


    // Special Memeber Funcions
    SaveLoadManager();
    ~SaveLoadManager() = default;
    SaveLoadManager(const SaveLoadManager&) = delete;
    SaveLoadManager& operator=(const SaveLoadManager&) = delete;

    // De-serialization methods
    bool ParseGrid(std::ifstream& theFile, PuzzleData* puzzleData) const;
    bool ParseGiven(std::istringstream& ss, PuzzleData* puzzleData) const;
    bool ParseHints(std::istringstream& ss, PuzzleData* puzzleData) const;
    bool ParseConstraints(std::ifstream& theFile, PuzzleData* puzzleData) const;
    bool ParseRegion(std::istringstream& ss, PuzzleData* puzzleData) const;
    bool ParseKillerCage(std::istringstream& ss, PuzzleData* puzzleData) const;

    // Serialization methods
    std::string SerializeGrid(const PuzzleData& puzzleData) const;
    std::string SerializeGiven(CellCoord cell, unsigned short value) const;
    std::string SerializeHints(CellCoord cell, std::set<unsigned short> hints) const;
    std::string SerializeConstraints(const PuzzleData& puzzleData) const;
    std::string SerializeRegion(unsigned short id, const CellsInRegion& region) const;
    std::string SerializeKillerCage(unsigned int id, unsigned int sum, const CellsInRegion& cells) const;
public:
    static const SaveLoadManager* Get();
    /// <summary>
    /// Parse the file at the specified path and return a SudokuGridfilled  with the parsed values
    /// </summary>
    bool LoadSudoku(std::string filepath, std::unique_ptr<PuzzleData>& puzzleData) const;
    /// <summary>
    /// Save the board to a file at the specified location
    /// </summary>
    bool SaveSudoku(std::string filepath, const PuzzleData& puzzleData) const;
};

#endif // !SAVE_LOAD_MANAGER_H
