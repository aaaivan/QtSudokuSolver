#include "SudokuGrid.h"
#include "Region.h"
#include "RegionsManager.h"
#include "GridProgressManager.h"
#include "SudokuCell.h"
#include "../sudokusolverthread.h"
#include <cassert>

SudokuGrid::SudokuGrid(unsigned short size, SudokuSolverThread* solverThread) :
    mSize(size),
    mParentNode(nullptr), // needs to be initialized before the progress manager
    mGrid(),
    mRegionsManager(std::make_unique<RegionsManager>(this)),
    mProgressManager(std::make_unique<GridProgressManager>(this)),
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
    mProgressManager(std::make_unique<GridProgressManager>(this)),
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
        DefineRegion(house, RegionType::House_Row);
        house.clear();
    }

    // define columns
    for (unsigned short col = 0; col < mSize; ++col)
    {
        for (unsigned short row = 0; row < mSize; ++row)
        {
            house.push_back({ row, col });
        }
        DefineRegion(house, RegionType::House_Column);
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

void SudokuGrid::AddGivenCell(unsigned short row, unsigned short col, unsigned short value)
{
        assert(	row < mSize &&
                col < mSize &&
                value <= mSize &&
                "Index out of bound for a Given Cell");
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

void SudokuGrid::DefineRegion(const std::vector<std::array<unsigned short, 2>>& cells, RegionType regionType, VariantConstraint* constraint)
{
    std::vector<VariantConstraint*> constraints;
    if(constraint)
    {
        constraints.emplace_back(constraint);
    }
    DefineRegion(cells, regionType, constraints);
}

void SudokuGrid::DefineRegion(const std::vector<std::array<unsigned short, 2> > &cells, RegionType regionType, std::vector<VariantConstraint*> &constraints)
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

    RegionSPtr regionSPtr = std::make_shared<Region>(this, std::move(cellList), true);
    for (auto& constraint: constraints)
    {
        regionSPtr->AddVariantConstraint(std::unique_ptr<VariantConstraint>(constraint));
    }
    mRegionsManager->RegisterRegion(regionSPtr, regionType);
}

void SudokuGrid::ResetContents()
{
    for (size_t i = 0; i < mSize; i++)
    {
        for (size_t j = 0; j < mSize; j++)
        {
            mGrid.at(i).at(j)->Reset();
        }
    }
    mProgressManager->Clear();
    mRegionsManager->Reset();
}

void SudokuGrid::Clear()
{
    for (size_t i = 0; i < mSize; i++)
    {
        for (size_t j = 0; j < mSize; j++)
        {
            mGrid.at(i).at(j)->Reset();
        }
    }
    mProgressManager->Clear();
    mRegionsManager->Clear();
    DefineRowsAndCols();
}

void SudokuGrid::NotifyCellChanged(SudokuCell *cell)
{
    if(mSolverThread)
    {
        mSolverThread->NotifyCellChanged(cell);
    }
}

#if PRINT_LOG_MESSAGES
//TODO: this print methos is temporary.
void SudokuGrid::Print() const
{

    for (size_t row = 0; row < mGrid.size(); row++)
    {
        for (size_t col = 0; col < mGrid.size(); col++)
        {
            printf("%-22s", mGrid[row][col]->PrintOptions().c_str());
            if (col % 3 == 2)
            {
                printf("|  ");
            }
        }
        printf("\n");
        if (row % 3 == 2)
        {
            printf("------------------------------------------------------------------ ");
            printf("-------------------------------------------------------------------- ");
            printf("--------------------------------------------------------------------\n");
        }
    }
    printf("\n\n");

}
#endif // PRINT_LOG_MESSAGES
