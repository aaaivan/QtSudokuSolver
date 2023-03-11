#include "sudokusolverthread.h"
#include <algorithm>

PuzzleData::PuzzleData(unsigned short size):
    mSize(size),
    mRegions(size),
    mKillerCages(),
    mGivens(),
    mHints(),
    mPositiveDiagonal(false),
    mNegativeDiagonal(false)
{
}

bool SudokuSolverThread::HasPositiveDiagonalConstraint() const
{
    return mPuzzleData.mPositiveDiagonal;
}

bool SudokuSolverThread::HasNegativeDiagonalConstraint() const
{
    return mPuzzleData.mNegativeDiagonal;
}

bool SudokuSolverThread::HasKillerCage(CellCoord id) const
{
    return mPuzzleData.mKillerCages.count(id) > 0;
}

bool SudokuSolverThread::IsSameKillerCage(CellCoord id, unsigned int otherTotal, const CellsInRegion &cells) const
{
    bool matching = true;
    auto it = mPuzzleData.mKillerCages.find(id);
    if(it != mPuzzleData.mKillerCages.end())
    {
        const auto& cage = it->second;
        matching = matching && (cage.first == otherTotal);
        matching = matching && (cage.second == cells);
    }
    else
    {
        matching = false;
    }
    return matching;
}

std::set<unsigned short> SudokuSolverThread::HintsGet(CellCoord id) const
{
    std::set<unsigned short> result = {};
    if(mPuzzleData.mHints.count(id))
    {
        result = mPuzzleData.mHints.at(id);
    }
    return result;
}

void SudokuSolverThread::SetRegion(unsigned short regionId, const std::set<CellCoord> &cells)
{
    QMutexLocker locker(&mMutex);

    unsigned short index = regionId - 1;
    if(index < mPuzzleData.mSize && mPuzzleData.mRegions.at(index) != cells)
    {
        if(mPuzzleData.mRegions.at(index).empty())
        {
            AddRegionToSubmissionQueue(index);
        }
        else
        {
            ReloadGrid();
        }
        mPuzzleData.mRegions.at(index) = cells;
    }
}

void SudokuSolverThread::PositiveDiagonalConstraintSet(bool set)
{
    QMutexLocker locker(&mMutex);

    mPuzzleData.mPositiveDiagonal = set;
    if(set)
    {
        AddDiagonalToSubmissionQueue(PuzzleData::Diagonal_Positive);
    }
    else
    {
        ReloadGrid();
    }
}

void SudokuSolverThread::NegativeDiagonalConstraintSet(bool set)
{
    QMutexLocker locker(&mMutex);

    mPuzzleData.mNegativeDiagonal = set;
    if(set)
    {
        AddDiagonalToSubmissionQueue(PuzzleData::Diagonal_Negative);
    }
    else
    {
        ReloadGrid();
    }
}

void SudokuSolverThread::AddGiven(unsigned short value, CellCoord  cellId)
{
    QMutexLocker locker(&mMutex);

    if(value > 0 && value <= mPuzzleData.mSize)
    {
        if(mPuzzleData.mGivens.count(cellId) == 0)
        {
            AddGivenValueToSubmissionQueue(cellId);
        }
        else if(mPuzzleData.mGivens[cellId] != value)
        {
            ReloadCells();
        }
        mPuzzleData.mGivens[cellId] = value;
    }
}

void SudokuSolverThread::RemoveGiven(CellCoord  cellId)
{
    QMutexLocker locker(&mMutex);

    auto it = mPuzzleData.mGivens.find(cellId);
    if(it != mPuzzleData.mGivens.end())
    {
        mPuzzleData.mGivens.erase(it);
        ReloadCells();
    }
}

void SudokuSolverThread::AddKillerCage(CellCoord cageId, unsigned int total, const CellsInRegion &cells)
{
    QMutexLocker locker(&mMutex);

    auto it =  mPuzzleData.mKillerCages.find(cageId);
    if(it == mPuzzleData.mKillerCages.end())
    {
        if(cells.size() > 0)
        {
            mPuzzleData.mKillerCages[cageId] = {total, cells};
            AddKillerToSubmissionQueue(cageId);
        }
    }
}

void SudokuSolverThread::RemoveKillerCage(CellCoord cageId)
{
    QMutexLocker locker(&mMutex);

    auto it =  mPuzzleData.mKillerCages.find(cageId);
    if(it !=  mPuzzleData.mKillerCages.end())
    {
        mPuzzleData.mKillerCages.erase(it);
        ReloadGrid();
    }
}

void SudokuSolverThread::AddHint(CellCoord cellId, unsigned short value)
{
    QMutexLocker locker(&mMutex);

    auto it = mPuzzleData.mHints.find(cellId);
    if(it != mPuzzleData.mHints.end())
    {
        if(it->second.insert(value).second)
        {
            AddHintToSubmissionQueue(cellId);
        }
    }
    else
    {
        mPuzzleData.mHints[cellId] = {value};
        AddHintToSubmissionQueue(cellId);
    }
}

void SudokuSolverThread::RemoveHint(CellCoord cageId, unsigned short value)
{
    QMutexLocker locker(&mMutex);

    auto mapIt = mPuzzleData.mHints.find(cageId);
    if(mapIt != mPuzzleData.mHints.end())
    {
        auto setIt = mapIt->second.find(value);
        if(setIt != mapIt->second.end())
        {
            mapIt->second.erase(setIt);
            ReloadCells();
        }
    }
}

void SudokuSolverThread::RemoveAllHints()
{
    QMutexLocker locker(&mMutex);

    if(!mPuzzleData.mHints.empty())
    {
        mPuzzleData.mHints.clear();
        ReloadCells();
    }
}
