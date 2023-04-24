#include "SudokuGrid.h"
#include "Region.h"
#include "RegionsManager.h"
#include "GridProgressManager.h"
#include "SudokuCell.h"
#include "../sudokusolverthread.h"
#include "GhostCagesManager.h"
#include <cassert>

SudokuGrid::SudokuGrid(unsigned short size, SudokuSolverThread* solverThread) :
    mSize(size),
    mParentNode(nullptr), // needs to be initialized before the progress manager
    mGrid(),
    mRegionsManager(std::make_unique<RegionsManager>(this)),
    mGhostRegionsManager(std::make_unique<GhostCagesManager>(this)),
    mProgressManager(std::make_unique<GridProgressManager>(this)),
    mHasSnapshort(false),
    mSolverThread(solverThread)
{
    // populate the grid
    mGrid.reserve(mSize);
    for (size_t i = 0; i < mSize; i++)
    {
        mGrid.push_back(std::vector<CellUPtr>(mSize));
        for (size_t j = 0; j < mSize; j++)
        {
            mGrid.at(i).at(j) = std::make_unique<SudokuCell>(this, i, j, mSize);
        }
    }

    DefineRowsAndCols();
}

SudokuGrid::SudokuGrid(const SudokuGrid *grid) :
    mSize(grid->SizeGet()),
    mParentNode(grid),
    mGrid(),
    mRegionsManager(std::make_unique<RegionsManager>(this)),
    mGhostRegionsManager(std::make_unique<GhostCagesManager>(this)),
    mProgressManager(std::make_unique<GridProgressManager>(this)),
    mHasSnapshort(false),
    mSolverThread(nullptr)
{
    // populate the grid with the cells
    mGrid.reserve(mSize);
    for (size_t i = 0; i < mSize; i++)
    {
        mGrid.push_back(std::vector<CellUPtr>(mSize));
        for (size_t j = 0; j < mSize; j++)
        {
            mGrid.at(i).at(j) = CellUPtr(grid->mGrid.at(i).at(j)->DeepCopy(this));
        }
    }

    // create regions from the leaf regions of the existing grid
    for(const auto& r : grid->mRegionsManager->RegionsGet())
    {
        std::vector<VariantConstraint*> constraints;
        for (const auto& c : r->VariantConstraintsGet())
        {
            constraints.emplace_back(c->DeepCopy());
        }
        DefineRegion(r->CellCoordsGet(), RegionType::Generic_region, constraints);
    }
}

SudokuGrid::~SudokuGrid()
{
}


void SudokuGrid::DefineRowsAndCols()
{
    std::vector<std::array<unsigned short, 2>> house;
    // define rows
    for (unsigned short row = 0; row < mSize; ++row)
    {
        for (unsigned short col = 0; col < mSize; ++col)
        {
            house.push_back({ row, col });
        }
        DefineRegion(house, RegionType::House_Row, nullptr, "row " + std::to_string(row+1));
        house.clear();
    }

    // define columns
    for (unsigned short col = 0; col < mSize; ++col)
    {
        for (unsigned short row = 0; row < mSize; ++row)
        {
            house.push_back({ row, col });
        }
        DefineRegion(house, RegionType::House_Column, nullptr, "column " + std::to_string(col+1));
        house.clear();
    }
}

unsigned short SudokuGrid::SizeGet() const
{
    return mSize;
}

SudokuCell* SudokuGrid::CellGet(unsigned short row, unsigned short col) const
{
    return mGrid.at(row).at(col).get();
}

SudokuCell *SudokuGrid::CellGet(unsigned int id) const
{
    return CellGet(id / mSize, id % mSize);
}

RegionsManager* SudokuGrid::RegionsManagerGet() const
{
    return mRegionsManager.get();
}

GhostCagesManager *SudokuGrid::GhostRegionsManagerGet() const
{
    return mGhostRegionsManager.get();
}

GridProgressManager* SudokuGrid::ProgressManagerGet() const
{
    return mProgressManager.get();
}

bool SudokuGrid::IsSolved() const
{
    for (size_t i = 0; i < mSize; i++)
    {
        for (size_t j = 0; j < mSize; j++)
        {
            if(!mGrid.at(i).at(j)->IsSolved())
            {
                return false;
            }
        }
    }
    return true;
}

const SudokuGrid *SudokuGrid::ParentNodeGet()
{
    return mParentNode;
}

SudokuSolverThread *SudokuGrid::SolverThreadGet() const
{
    return mSolverThread;
}

void SudokuGrid::AddGivenCell(unsigned short row, unsigned short col, unsigned short value)
{
        assert(	row < mSize &&
                col < mSize &&
                value <= mSize &&
                "Index out of bound for a Given Cell");

        mProgressManager->Reset();
        mGrid.at(row).at(col)->MakeGiven(value);
}

void SudokuGrid::SetCellOptions(unsigned short row, unsigned short col, std::set<unsigned short> options)
{
    mGrid.at(row).at(col)->RemoveAllOtherOptions(options);
}

void SudokuGrid::SetCellEliminationHints(unsigned short row, unsigned short col, std::set<unsigned short> hints)
{
    for (const auto& v : hints)
    {
        mGrid.at(row).at(col)->RemoveOptionHint(v);
    }
}

void SudokuGrid::DefineRegion(const std::vector<std::array<unsigned short, 2>>& cells,
                              RegionType regionType, VariantConstraint* constraint, std::string name)
{
    std::vector<VariantConstraint*> constraints;
    if(constraint)
    {
        constraints.emplace_back(constraint);
    }
    DefineRegion(cells, regionType, constraints, name);
}

void SudokuGrid::DefineRegion(const std::vector<std::array<unsigned short, 2> > &cells,
                              RegionType regionType, std::vector<VariantConstraint*> &constraints, std::string name)
{
    assert(cells.size() <= mSize && "Index out of bound for a Given Cell");

    CellSet cellList;
    for (const auto& rowCol : cells)
    {
        assert(	rowCol.at(0) < mSize &&
                rowCol.at(1) < mSize &&
                "Index out of bound for a Given Cell");
        cellList.emplace(mGrid.at(rowCol.at(0)).at(rowCol.at(1)).get());
    }

    mProgressManager->Reset();
    RegionSPtr regionSPtr = std::make_shared<Region>(this, std::move(cellList), true);
    for (auto& constraint: constraints)
    {
        regionSPtr->AddVariantConstraint(std::unique_ptr<VariantConstraint>(constraint));
    }
    regionSPtr->RegionNameSet(name);
    mRegionsManager->RegisterRegion(regionSPtr, regionType);
}

void SudokuGrid::ResetContents()
{
    mHasSnapshort = false;

    for (size_t i = 0; i < mSize; i++)
    {
        for (size_t j = 0; j < mSize; j++)
        {
            mGrid.at(i).at(j)->Reset();
        }
    }
    mProgressManager->Clear();
    mRegionsManager->Reset();
    mGhostRegionsManager->Clear();
}

void SudokuGrid::Clear()
{
    mHasSnapshort = false;

    for (size_t i = 0; i < mSize; i++)
    {
        for (size_t j = 0; j < mSize; j++)
        {
            mGrid.at(i).at(j)->Reset();
        }
    }
    mProgressManager->Clear();
    mRegionsManager->Clear();
    mGhostRegionsManager->Clear();
    DefineRowsAndCols();
}

void SudokuGrid::NotifyCellChanged(SudokuCell *cell) const
{
    if(mSolverThread)
    {
        mSolverThread->NotifyCellChanged(cell, cell->IsSolved());
    }
}

void SudokuGrid::NotifyCellChanged(unsigned int cellId) const
{
    NotifyCellChanged(CellGet(cellId));
}

void SudokuGrid::TakeSnapshot()
{
    mHasSnapshort = true;
    mRegionsManager->TakeSnapshot();
    mGhostRegionsManager->TakeSnapshot();
    for (size_t i = 0; i < mSize; i++)
    {
        for (size_t j = 0; j < mSize; j++)
        {
            mGrid.at(i).at(j)->TakeSnapshot();
        }
    }
}

void SudokuGrid::RestoreSnapshot()
{
    if(mHasSnapshort)
    {
        mHasSnapshort = false;
        for (size_t i = 0; i < mSize; i++)
        {
            for (size_t j = 0; j < mSize; j++)
            {
                mGrid.at(i).at(j)->RestoreSnapshot();
            }
        }
        mRegionsManager->RestoreSnapshot();
        mGhostRegionsManager->RestoreSnapshot();
    }
}
