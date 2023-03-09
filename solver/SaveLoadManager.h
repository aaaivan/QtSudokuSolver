#ifndef SAVE_LOAD_MANAGER_H
#define SAVE_LOAD_MANAGER_H

#include "Types.h"
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <functional>

class SaveLoadManager
{
	enum class ValueType
	{
		GivenCell,
		HintedElimination,

		None
	};
	static const std::map<ValueType, char> identifiersMap;		// store the character that identifies the value type

	struct ParsingInfo
	{
		char id;															// character that identifies the type of region (row, column, killer cage, etc.)
		std::function<std::string(const Region*)> serialisationFunc;		// function to serialize the region
		std::function<bool(std::string, SudokuGrid*)> deserialisationFunc;	// function to deserialize the region
	};
	std::map<RegionType, ParsingInfo> mParsingInfoMap;						// map that associates each region type to their parsing info

	// Special Memeber Funcions
	SaveLoadManager();
	~SaveLoadManager() = default;
	SaveLoadManager(const SaveLoadManager&) = delete;
	SaveLoadManager& operator=(const SaveLoadManager&) = delete;

	// De-serialization methods
	bool ParseGrid(std::ifstream& theFile, std::vector<std::vector<std::string>>& outGrid, unsigned short size) const;
	bool ParseCell(std::string token, unsigned short row, unsigned short col, SudokuGrid* grid) const;
	bool ParseConstraints(std::ifstream& theFile, SudokuGrid* grid) const;
	bool ParseRegion(RegionType type, std::string line, SudokuGrid* grid) const;
	bool ParseKillerCage(std::string line, SudokuGrid* grid) const;

	// Serialization methods
	std::string SerializeGrid(const SudokuGrid* grid) const;
	std::string SerializeCell(const SudokuCell* cell) const;
	std::string SerializeConstraints(const SudokuGrid* grid) const;
	std::string SerializeRegion(char id, const Region* region) const;
	std::string SerializeKillerCage(char id, const Region* region) const;
public:
	static const SaveLoadManager* Get();
	/// <summary>
	/// Parse the file at the specified path and return a SudokuGridfilled  with the parsed values
	/// </summary>
	std::unique_ptr<SudokuGrid> LoadSudoku(std::string filepath) const;
	/// <summary>
	/// Save the board to a file at the specified location
	/// </summary>
	bool SaveSudoku(std::string filepath,const SudokuGrid* grid) const;
};

#endif // !SAVE_LOAD_MANAGER_H
