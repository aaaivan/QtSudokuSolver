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

std::pair<unsigned int, CellsInRegion> SudokuSolverThread::KillerCageGet(CellCoord id) const
{
    std::pair<unsigned int, CellsInRegion> result = {0, {}};
    if(mPuzzleData.mKillerCages.count(id))
    {
        result = mPuzzleData.mKillerCages.at(id);
    }
    return result;
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

void SudokuSolverThread::AddCellToKillerCage(CellCoord cageId, CellCoord  cellId)
{
    QMutexLocker locker(&mMutex);

    auto it = mPuzzleData.mKillerCages.find(cageId);
    if(it != mPuzzleData.mKillerCages.end())
    {
        if(it->second.second.insert(cellId).second)
        {
            ReloadGrid();
        }
    }
    else
    {
        mPuzzleData.mKillerCages[cageId] = {0, {cellId}};
        ReloadGrid();
    }
}

void SudokuSolverThread::RemoveCellFromKillerCage(CellCoord cageId, CellCoord  cellId)
{
    QMutexLocker locker(&mMutex);

    auto mapIt = mPuzzleData.mKillerCages.find(cageId);
    if(mapIt != mPuzzleData.mKillerCages.end())
    {
        auto setIt = mapIt->second.second.find(cellId);
        if(setIt != mapIt->second.second.end())
        {
            mapIt->second.second.erase(setIt);
            if(mapIt->second.second.size() == 0)
            {
                mPuzzleData.mKillerCages.erase(mapIt);
            }
            ReloadGrid();
        }
    }
}

void SudokuSolverThread::AddKillerCage(CellCoord cageId, const std::pair<unsigned int, CellsInRegion> &cage)
{
    QMutexLocker locker(&mMutex);

    mPuzzleData.mKillerCages[cageId] = cage;
    ReloadGrid();
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

void SudokuSolverThread::KillerCageTotalSet(CellCoord cageId, unsigned int total)
{
    QMutexLocker locker(&mMutex);

    auto it = mPuzzleData.mKillerCages.find(cageId);
    if(it != mPuzzleData.mKillerCages.end())
    {
        if(it->second.first != total)
        {
            it->second.first = total;
            ReloadGrid();
        }
    }
    else
    {
        mPuzzleData.mKillerCages[cageId] = {total, {}};
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
