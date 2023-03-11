// Includes
#include "SudokuCell.h"
#include "SudokuGrid.h"
#include "RegionsManager.h"
#include "GridProgressManager.h"
#include "../sudokusolverthread.h"
#include <cassert>

SudokuCell::SudokuCell(SudokuGrid* grid, unsigned short row, unsigned short col, unsigned short sudokuSize) :
    mValue(0),
    mRow(row),
    mCol(col),
    mId(col + sudokuSize * row),
    mViableOptions(),
    mEliminationHints(),
    mIsGiven( false ),
    mParentGrid(grid)
{
    Reset();
}

unsigned short SudokuCell::ValueGet() const
{
    return mValue;
}

unsigned int SudokuCell::IdGet() const
{
    return mId;
}

unsigned short SudokuCell::RowGet() const
{
    return mRow;
}

unsigned short SudokuCell::ColGet() const
{
    return mCol;
}

SudokuGrid* SudokuCell::GridGet() const
{
    return mParentGrid;
}

const std::set<unsigned short>& SudokuCell::OptionsGet() const
{
    return mViableOptions;
}

const std::set<unsigned short>& SudokuCell::HintedEliminationsGet() const
{
    return mEliminationHints;
}

const RegionSet& SudokuCell::GetRegionsWithCell() const
{
    return GridGet()->RegionsManagerGet()->RegionsWithCellGet(this);
}

bool SudokuCell::HasGuess(unsigned short value) const
{
    return mViableOptions.find(value) != mViableOptions.end();
}

bool SudokuCell::IsSolved() const
{
    return mValue != 0;
}

bool SudokuCell::IsGiven() const
{
    return mIsGiven;
}

void SudokuCell::MakeGiven(unsigned short value)
{
    if (value)
    {
        mIsGiven = true;
        mViableOptions.clear();
        mViableOptions.insert(value);
        mValue = value;
        mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_GivenCellAdded>(this, *mViableOptions.begin()));
        mParentGrid->SolverThreadManagerGet()->NotifyCellChanged(this);
    }
}

void SudokuCell::RemoveOption(unsigned short guess)
{
    if (mViableOptions.erase(guess))
    {
        if (mViableOptions.size() == 1)
        {
            Progress_OptionRemoved notification(this, guess);
            notification.ProcessProgress();
            mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_SingleOptionLeftInCell>(this, *mViableOptions.begin()));
        }
        else if (mViableOptions.size() == 0)
        {
            if (mIsGiven)
            {
                mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_ClashWithGivenCell>(this));
            }
            else if (IsSolved())
            {
                mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_ClashWithSolvedCell>(this));
            }
            else
            {
                mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_NoOptionsLeftInCell>(this));
            }
        }
        else
        {
            Progress_OptionRemoved notification(this, guess);
            notification.ProcessProgress();
        }
    }
}

void SudokuCell::RemoveOptionHint(unsigned short option)
{
    size_t viableOptCount = mViableOptions.size();
    RemoveOption(option);

    if (mViableOptions.size() < viableOptCount)
    {
        mEliminationHints.insert(option);
    }
}

void SudokuCell::RemoveAllOtherOptions(unsigned short option)
{
    std::set<unsigned int> optionsToDelete;

    auto it = mViableOptions.begin();
    while (it != mViableOptions.end())
    {
        if (*it != option)
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

void SudokuCell::Reset()
{
    mValue = 0;
    mIsGiven = false;
    mEliminationHints.clear();
    mViableOptions.clear();
    for (unsigned short i = 1; i <= mParentGrid->SizeGet(); ++i)
    {
        mViableOptions.emplace_hint(mViableOptions.end(), i);
    }
    mParentGrid->SolverThreadManagerGet()->NotifyCellChanged(this);
}

void SudokuCell::ValueSet(unsigned short value)
{
    assert( value && mViableOptions.find(value) != mViableOptions.end() );
    mValue = value;
}

#if PRINT_LOG_MESSAGES

//TODO: these print methods are temporary
std::string SudokuCell::PrintPosition() const
{
    std::string message;
    message += "r";
    message += std::to_string(RowGet() + 1);
    message += "c";
    message += std::to_string(ColGet() + 1);
    return message;
}

//TODO: these print methods are temporary
std::string SudokuCell::PrintOptions() const
{
    std::string message;
    message += "[ ";
    for (const auto& option : mViableOptions)
    {
        message += std::to_string(option) + " ";
    }
    message += "]";
    return message;
}

#endif // PRINT_LOG_MESSAGES
