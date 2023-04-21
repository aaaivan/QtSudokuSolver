#ifndef SUDOKU_CELL_H
#define SUDOKU_CELL_H

// Includes
#include "Types.h"
#include <string>

//classes
class SudokuGrid;

class SudokuCell
{
private:
    unsigned short mValue;						// the digit contained by the cell
    const unsigned short mRow;					// row the cell is in
    const unsigned short mCol;					// column the cell is in
    const unsigned int mId;						// unique id for the cell
    std::set<unsigned short> mViableOptions;	// vector of digits are still viable
    std::set<unsigned short> mEliminationHints;	// options that have been ruled out manually by the user
    bool mIsGiven;								// whether th ecell is a given clue
    SudokuGrid* mParentGrid;					// pointer to the grid the cell belongs to
    std::string mName;

    friend class Progress_GivenCellAdded;
    friend class Progress_SingleOptionLeftInCell;
    friend class Progress_SingleCellForOption;

    struct Snapshot
    {
        std::set<unsigned short> mViableOptions;
        unsigned short mValue;
        Snapshot(const std::set<unsigned short>& opt, const unsigned short& val):
            mViableOptions(opt),
            mValue(val)
        {}
    };
    std::unique_ptr<Snapshot> mSnapshot;

public:
// Constructors

    SudokuCell(SudokuGrid* grid, unsigned short  row, unsigned short  col, unsigned short  sudoku_size);

// Public getters

    unsigned short ValueGet() const;
    unsigned int IdGet() const;
    unsigned short RowGet() const;
    unsigned short ColGet() const;
    SudokuGrid* GridGet() const;
    const std::set<unsigned short>& OptionsGet() const;
    const std::set<unsigned short>& HintedEliminationsGet() const;
    const RegionSet& GetRegionsWithCell() const;
    std::string CellNameGet() const;

// Constant methods

    bool HasGuess(unsigned short  guess) const;
    bool IsSolved() const;
    bool IsGiven() const;

// Non-constant methods

    /// <summary>
    /// Make the cell a given of the puuzzle with the value passed as the parameter
    /// </summary>
    void MakeGiven(unsigned short value);

    /// <summary>
    /// Remove the specified value from the list viable options
    /// </summary>
    void RemoveOption(unsigned short option);

    /// <summary>
    /// Remove the specified value from the list viable options
    /// and add it to the list of elimination hints.
    /// This allows the user to remove options that the solving algorithm
    /// is not able to remove.
    /// </summary>
    void RemoveOptionHint(unsigned short option);

    /// <summary>
    /// Remove the specified list of values from the list viable options
    /// </summary>
    template<class T>
    void RemoveMultipleOptions(const T& options);

    /// <summary>
    /// Remove all but the specified value from the list viable options
    /// </summary>
    void RemoveAllOtherOptions(unsigned short option);

    /// <summary>
    /// Remove all but the specified values from the list viable options
    /// </summary>
    template<class T>
    void RemoveAllOtherOptions(const T &options);

    /// <summary>
    /// Clear all the options (including hints) and remove the given digits if there is one
    /// </summary>
    void Reset();

    /// <summary>
    /// Creates a deep copy of the object
    /// </summary>
    SudokuCell* DeepCopy(SudokuGrid* parentGrid) const;
    void TakeSnapshot();
    void RestoreSnapshot();

private:
    void ValueSet(unsigned short value);
};

template<class T>
void SudokuCell::RemoveMultipleOptions(const T& options)
{
    for (const unsigned short& option : options)
    {
        RemoveOption(option);
    }
}

template<class T>
void SudokuCell::RemoveAllOtherOptions(const T& options)
{
    if (options.size() == 0)
    {
        return;
    }

    std::set<unsigned int> optionsToDelete;

    auto it = mViableOptions.begin();
    while (it != mViableOptions.end())
    {
        if (options.find(*it) == options.end())
        {
            optionsToDelete.emplace(*it);
        }
        ++it;
    }
    for (const auto& option : optionsToDelete)
    {
        RemoveOption(option);
    }
}
#endif // SUDOKU_CELL_H
