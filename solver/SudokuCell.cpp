// Includes
#include "SudokuCell.h"
#include "SudokuGrid.h"
#include "RegionsManager.h"
#include "GridProgressManager.h"
#include <cassert>

SudokuCell::SudokuCell(SudokuGrid* grid, unsigned short row, unsigned short col, unsigned short sudokuSize) :
    mValue(0),
    mRow(row),
    mCol(col),
    mId(col + sudokuSize * row),
    mViableOptions(),
    mEliminationHints(),
    mIsGiven( false ),
    mParentGrid(grid),
    mName("r" + std::to_string(row + 1) + "c" + std::to_string(col + 1)),
    mSnapshot(nullptr)
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

std::string SudokuCell::CellNameGet() const
{
    return mName;
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
        mValue = value;
        mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_GivenCellAdded>(this, value));
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
            if(!mIsGiven)
            {
                mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_SingleOptionLeftInCell>(this, *mViableOptions.begin()));
            }
        }
        else if (mViableOptions.size() == 0)
        {
            if (mIsGiven)
            {
                mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_ClashWithGivenCell>(this, mParentGrid));
            }
            else if (IsSolved())
            {
                mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_ClashWithSolvedCell>(this, mParentGrid));
            }
            else
            {
                mParentGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_NoOptionsLeftInCell>(this, mParentGrid));
            }
        }
        else
        {
            Progress_OptionRemoved notification(this, guess);
            notification.ProcessProgress();
        }
        if(!mParentGrid->ParentNodeGet())
        {
            mParentGrid->NotifyCellChanged(this);
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
    mSnapshot.reset();
    for (unsigned short i = 1; i <= mParentGrid->SizeGet(); ++i)
    {
        mViableOptions.emplace_hint(mViableOptions.end(), i);
    }
    if(!mParentGrid->ParentNodeGet())
    {
        mParentGrid->NotifyCellChanged(this);
    }
}

SudokuCell *SudokuCell::DeepCopy(SudokuGrid* parentGrid) const
{
    assert(parentGrid->SizeGet() == mParentGrid->SizeGet());

    SudokuCell* cell = new SudokuCell(parentGrid, mRow, mCol, parentGrid->SizeGet());
    cell->mValue = this->mValue;
    cell->mViableOptions = this->mViableOptions;
    cell->mEliminationHints = this->mEliminationHints;
    cell->mIsGiven = this->mIsGiven;

    return cell;
}

void SudokuCell::TakeSnapshot()
{
    mSnapshot = std::make_unique<Snapshot>(mViableOptions, mValue);
}

void SudokuCell::RestoreSnapshot()
{
    if(mSnapshot)
    {
        mViableOptions = std::move(mSnapshot->mViableOptions);
        mValue = std::move(mSnapshot->mValue);
        mSnapshot.reset();
    }
}

void SudokuCell::ValueSet(unsigned short value)
{
    if( value != mValue && mViableOptions.count(value) > 0 )
    {
        mValue = value;
        if(!mParentGrid->ParentNodeGet())
        {
            mParentGrid->NotifyCellChanged(this);
        }
    }
}
