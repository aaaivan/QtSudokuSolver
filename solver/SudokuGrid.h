#ifndef SUDOKU_GRID_H
#define SUDOKU_GRID_H

// Includes
#include "Types.h"
#include <array>
#include <vector>
#include <string>

// Classes
class RegionsManager;
class GridProgressManager;
class VariantConstraint;
class SudokuSolverThread;

class SudokuGrid
{
    const unsigned short mSize;					// length of the grid
    const SudokuGrid* mParentNode;
    std::vector<std::vector<CellUPtr>> mGrid;	// array of cells representing the grid
    RegionsManagerUPtr mRegionsManager;			// manages the subdivision of the grid into regions
    GridProgressManagerUPtr mProgressManager;	// notifies cells when some progress has been made
    bool mHasSnapshort;

    SudokuSolverThread* mSolverThread;

// Private methods
    void DefineRowsAndCols();

public:
// Constructors/Destructors

    SudokuGrid(unsigned short size, SudokuSolverThread* solverThread);
    SudokuGrid(const SudokuGrid* grid);
    ~SudokuGrid();

// Public getters

    unsigned short SizeGet() const;
    SudokuCell* CellGet(unsigned short row, unsigned short col) const;
    SudokuCell* CellGet(unsigned int id) const;
    RegionsManager* RegionsManagerGet() const;
    GridProgressManager* ProgressManagerGet() const;
    bool IsSolved() const;
    const SudokuGrid* ParentNodeGet();
    SudokuSolverThread* SolverThreadGet() const;

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
    void DefineRegion(const std::vector<std::array<unsigned short, 2> /* row, col */>& cells,
                      RegionType regionType, VariantConstraint* constraint = nullptr, std::string name="");
    void DefineRegion(const std::vector<std::array<unsigned short, 2> /* row, col */>& cells,
                      RegionType regionType, std::vector<VariantConstraint*> &constraints, std::string name="");

    /// <summary>
    /// Clear the content of the grid (does not delete the regions)
    /// </summary>
    void ResetContents();

    /// <summary>
    /// Clear the content of the grid, including the regions
    /// </summary>
    void Clear();

    /// <summary>
    /// Notify the user interface that the content of the cell has changed
    /// so that it can be displayed
    /// </summary>
    void NotifyCellChanged(SudokuCell* cell) const;
    void NotifyCellChanged(unsigned int cellId) const;

    void TakeSnapshot();
    void RestoreSnapshot();

};
#endif // SUDOKU_GRID_H
