#ifndef SUDOKU_GRID_H
#define SUDOKU_GRID_H

// Includes
#include "Types.h"
#include <array>
#include <vector>

// Classes
class RegionsManager;
class GridProgressManager;
class VariantConstraint;

class SudokuGrid
{
	const unsigned short mSize;					// length of the grid
	std::vector<std::vector<CellUPtr>> mGrid;	// array of cells representing the grid
	RegionsManagerUPtr mRegionsManager;			// manages the subdivision of the grid into regions
	GridProgressManagerUPtr mProgressManager;	// notifies cells when some progress has been made

public:
// Constructors/Destructors

	SudokuGrid(unsigned short size);
	~SudokuGrid();

// Public getters

	unsigned short SizeGet() const;
	const SudokuCell* CellGet(unsigned short row, unsigned short col) const;
	RegionsManager* RegionsManagerGet() const;
	GridProgressManager* ProgressManagerGet() const;

// Non-constant methods

#if PRINT_LOG_MESSAGES
	void Print() const;
#endif // PRINT_LOG_MESSAGES

// Non-constant methods

	/// <summary>
	/// Define a given cell of the puzzle
	/// </summary>
	void AddGivenCell(unsigned short row, unsigned short col, unsigned short value);

	/// <summary>
	/// Set the viable options of a cell
	/// </summary>
	void SetCellOptions(unsigned short row, unsigned short col, std::set<unsigned short> options);

	/// <summary>
	/// Set the elimination hints of a cell
	/// </summary>
	void SetCellEliminationHints(unsigned short row, unsigned short mcol, std::set<unsigned short> hints);

	/// <summary>
	/// Takes a list of number couples, representing coordinates of cells in the same region.
	/// The region type (row, column, box etc.) is also specified as a parameter.
	/// </summary>
	void DefineRegion(const std::vector<std::array<unsigned short, 2> /* row, col */>& cells, RegionType regionType, std::unique_ptr<VariantConstraint> constraints = nullptr);

	/// <summary>
	/// Clear the content of the grid (does not delete the regions)
	/// </summary>
	void ResetContents();

	/// <summary>
	/// Clear the content of the grid, including the regions
	/// </summary>
	void Clear();
};
#endif // SUDOKU_GRID_H