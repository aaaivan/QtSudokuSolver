#include "BruteForceSolver.h"
#include "SudokuGrid.h"
#include "SudokuCell.h"
#include "RegionsManager.h"
#include "thirdparty/dancing_links.h"
#include "bruteforcesolverthread.h"
#include <cassert>
#include <QDebug>

BruteForceSolver::BruteForceSolver(BruteForceSolverThread* bruteForceThread, SudokuGrid* grid, bool* abortFlag):
    mGrid(grid)
  , mBruteForceThread(bruteForceThread)
  , mUseHintsAsConstraints(false)
  , mIncidenceMatrixDirty(true)
  , mSolutionsDirty(true)
  , mMaxSolutionCount(0)
  , mAbort(abortFlag)
  , mDLXMatrix(nullptr)
  , mSolutions()
  , mSolutionIt(mSolutions.end())
{
}

Possibility BruteForceSolver::PossibilityFromRowIndex(size_t row)
{
    size_t size = mGrid->SizeGet();
    assert(row < size * size * size);

    CellId id = static_cast<CellId>(row/size);
    unsigned short value = static_cast<unsigned short>(row % size + 1);
    return {id, value};
}

size_t BruteForceSolver::IndexFromPossibility(CellId id, unsigned short value)
{
    assert(value > 0);

    size_t size = mGrid->SizeGet();
    return id * size + value - 1;
}

void BruteForceSolver::GenerateIncidenceMatrix(bool useHints)
{
    if(useHints != mUseHintsAsConstraints)
    {
        mUseHintsAsConstraints = useHints;
        mIncidenceMatrixDirty = true;
    }
    if(!mIncidenceMatrixDirty)
    {
        return;
    }
    mIncidenceMatrixDirty = false;
    mSolutionsDirty = true;

    size_t size = mGrid->SizeGet();
    const size_t primary_columns = size * size * size;

    size_t im_rows = primary_columns; // num of all possibilities
    size_t im_cols = 3 * size * size;    // row-col, row-num, col-num constraints

    const auto regions = mGrid->RegionsManagerGet()->StartingRegionsGet();
    // primary constraints
    im_cols += regions[(int)RegionType::House_Region].size() * size; // box-num constraint
    //secondary constraints
    im_cols += regions[(int)RegionType::Generic_region].size() * size;
    im_rows += regions[(int)RegionType::Generic_region].size() * size;
    for(const auto& reg: regions[(int)RegionType::KillerCage])
    {
        const KillerConstraint* kc = static_cast<const KillerConstraint*>(reg.get()->GetConstraintByType(RegionType::KillerCage));
        im_cols += size;;
        im_rows += kc->CombinationsGet().size();
    }

    // create the temp incidence matrix
    bool** M = new bool*[im_rows];
    for(size_t i = 0; i < im_rows; i++)
    {
        M[i] = new bool[im_cols];
    }

    // fill the incidence matrix
    FillIncidenceMatrix(M, im_rows);

    // convert matrix to 2d doubly linked list
    mDLXMatrix.reset();
    mDLXMatrix = std::make_unique<linked_matrix_GJK::LMatrix>(M,im_rows,im_cols);

    // delete the temp incidence matrix
    for(size_t i = 0; i < im_rows; i++)
    {
        delete[] M[i];
    }
    delete[] M;
}

void BruteForceSolver::SolveExactCoverProblem()
{
    if(mSolutionsDirty)
    {
        mSolutions.clear();
        mSolutionIt = mSolutions.end();
        mSolutionsDirty = false;
        std::list<std::vector<size_t>> solutions;
        dancing_links_GJK::Exact_Cover_Solver(*mDLXMatrix, solutions, mMaxSolutionCount, mAbort);

        size_t size = mGrid->SizeGet();
        const size_t primary_columns = size * size * size;
        for(const auto& sol : solutions)
        {
            mSolutions.push_back(std::vector<unsigned short>(size * size));
            for (const auto& r : sol)
            {
                if(r >= primary_columns)
                {
                    continue;
                }
                Possibility p = PossibilityFromRowIndex(r);
                mSolutions.back()[p.first] = p.second;
            }
        }
    }
}

void BruteForceSolver::FillIncidenceMatrix(bool** M, const size_t rows)
{
    size_t c = 0;
    size_t size = mGrid->SizeGet();
    size_t primaryRows = size * size * size;
    size_t sec_r = primaryRows;
    // row-col constraint
    // |r1c1 r1c2 r1c3 ... r2c1 r2c2 ... r9c8 r9c9|
    for(size_t i = 0; i < size * size; ++i)
    {
        for(size_t r = 0; r < rows; ++r)
        {
            if(r >= primaryRows)
            {
                M[r][c] = 0;
                continue;
            }
            const Possibility p = PossibilityFromRowIndex(r);
            SudokuCell* cell = mGrid->CellGet(p.first);
            unsigned short value = p.second;
            if(cell->IsGiven())
            {
                M[r][c] = (cell->IdGet() == i && value == cell->ValueGet());
            }
            else if(mUseHintsAsConstraints)
            {
                M[r][c] = (cell->IdGet() == i && cell->HintedEliminationsGet().count(value) == 0);
            }
            else
            {
                M[r][c] = (cell->IdGet() == i);
            }
        }
        ++c;
    }

    // houses
    // |Ha1 Ha2 Ha3 ... Hb1 Hb2 Hb3 ... |
    constexpr int housesTypes = 3;
    RegionType houses[housesTypes] = {
        RegionType::House_Row,
        RegionType::House_Column,
        RegionType::House_Region
    };
    for (int regionType = 0; regionType < housesTypes; ++regionType)
    {
        const auto& regions = mGrid->RegionsManagerGet()->StartingRegionsGet()[static_cast<int>(houses[regionType])];
        auto it = regions.begin();
        while(it != regions.end())
        {
            for(size_t i = 1; i <= size; ++i)
            {
                for(size_t r = 0; r < rows; ++r)
                {
                    if(r >= primaryRows)
                    {
                        M[r][c] = 0;
                        continue;
                    }
                    const Possibility p = PossibilityFromRowIndex(r);
                    SudokuCell* cell = mGrid->CellGet(p.first);
                    unsigned short value = p.second;
                    M[r][c] = ((*it)->CellsGet().count(cell) && value == i);
                }
                ++c;
            }
            ++it;
        }
    }

    // non-house regions
    // |Ra1 Ra2 Ra3 ... Rb1 Rb2 Rb3 ... |
    {
        const auto& regions = mGrid->RegionsManagerGet()->StartingRegionsGet()[static_cast<int>(RegionType::Generic_region)];
        auto it = regions.begin();
        while(it != regions.end())
        {
            for(size_t i = 1; i <= size; ++i)
            {
                for(size_t r = 0; r < rows; ++r)
                {
                    if(r >= primaryRows)
                    {
                        M[r][c] = 0;
                        continue;
                    }
                    const Possibility p = PossibilityFromRowIndex(r);
                    SudokuCell* cell = mGrid->CellGet(p.first);
                    unsigned short value = p.second;
                    M[r][c] = ((*it)->CellsGet().count(cell) && value == i);
                }
                M[sec_r][c] = 1;
                ++sec_r;
                ++c;
            }
            ++it;
        }
    }

    // killer cages
    // |Ka1 Ka2 Ka3 ... Kb1 Kb2 Kb3 ... |
    {
        const auto& regions = mGrid->RegionsManagerGet()->StartingRegionsGet()[static_cast<int>(RegionType::KillerCage)];
        auto it = regions.begin();
        while(it != regions.end())
        {
            int start_c = c;
            for(size_t i = 1; i <= size; ++i)
            {
                for(size_t r = 0; r < rows; ++r)
                {
                    if(r >= primaryRows)
                    {
                        M[r][c] = 0;
                        continue;
                    }
                    const Possibility p = PossibilityFromRowIndex(r);
                    SudokuCell* cell = mGrid->CellGet(p.first);
                    unsigned short value = p.second;
                    M[r][c] = ((*it)->CellsGet().count(cell) && value == i);
                }
                ++c;
            }
            const KillerConstraint* kc = static_cast<const KillerConstraint*>((*it)->GetConstraintByType(RegionType::KillerCage));
            for(const auto& combination: kc->CombinationsGet())
            {
                for(size_t i = 1; i <= size; ++i)
                {
                    M[sec_r][start_c + i - 1] = (combination.count(i) == 0);
                }
                ++sec_r;
            }
            ++it;
        }
    }
}

void BruteForceSolver::DirtySolutions()
{
    mSolutionsDirty = true;
    mIncidenceMatrixDirty = true;
    mMaxSolutionCount = 0;
}

void BruteForceSolver::CountSolutions(size_t maxSolutionsCount)
{
    assert(!mIncidenceMatrixDirty);
    if(maxSolutionsCount > mMaxSolutionCount)
    {
        mMaxSolutionCount = maxSolutionsCount;
        mSolutionsDirty = true;
    }
    SolveExactCoverProblem();
    bool stopped = mSolutions.size() >= mMaxSolutionCount || *mAbort == true;
    mBruteForceThread->NotifySolutionsCountReady(mSolutions.size(), stopped);
}

void BruteForceSolver::FindSolution(size_t maxSolutionsCount)
{
    CountSolutions(maxSolutionsCount);
    std::vector<unsigned short> solution;

    if(mSolutions.size() > 0)
    {
        if(mSolutionIt == mSolutions.end())
        {
            mSolutionIt = mSolutions.begin();
        }
        solution = *mSolutionIt;
        mSolutionIt++;

        mBruteForceThread->NotifySolutionReady(solution);
    }
}
