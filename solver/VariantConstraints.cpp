#include "VariantConstraints.h"
#include "Region.h"
#include "SudokuGrid.h"
#include "GridProgressManager.h"
#include "qglobal.h"
#include "SudokuCell.h"
#include "thirdparty/dancing_links.h"
#include <cassert>

VariantConstraint::VariantConstraint() :
    mRegion(nullptr)
{
}

Region* VariantConstraint::RegionGet() const
{
    return mRegion;
}

VariantConstraint::~VariantConstraint()
{
}

void VariantConstraint::Initialise(Region* region)
{
    mRegion = region;
}

KillerConstraint::KillerConstraint(unsigned int sum) :
    VariantConstraint(),
    mCageSum(sum),
    mCombinations(),
    mConfirmedValues(),
    mAllowedValues(),
    mCellToOrder(),
    mOrderToCell(),
    mDLXSolutions(),
    mValidSolution(),
    mSnapshot(nullptr)
{
}

void KillerConstraint::Initialise(Region* region)
{
    VariantConstraint::Initialise(region);
    mSnapshot.reset();

    mCombinations.clear();
    mConfirmedValues.clear();
    mAllowedValues.clear();
    mCellToOrder.clear();
    mOrderToCell.clear();
    mDLXSolutions.clear();
    mValidSolution.clear();

    unsigned short x = 0;
    for (const auto& c : region->CellsGet())
    {
        mCellToOrder[c->IdGet()] = x;
        mOrderToCell[x] = c->IdGet();
        mAllowedValues.push_back(c->OptionsGet());
        ++x;
    }

    FindCombinations(std::list<unsigned short>(mRegion->AllowedValuesGet().begin(),
                     mRegion->AllowedValuesGet().end()));


    size_t gridSize = region->GridGet()->SizeGet();
    size_t colsCount = region->SizeGet() + gridSize;
    size_t mainRowsCount = region->SizeGet() * gridSize;
    size_t rowsCount = mainRowsCount + mCombinations.size();
    bool** incidenceMatrix = new bool*[rowsCount];
    for (size_t i = 0; i < rowsCount; ++i)
    {
        incidenceMatrix[i] = new bool[colsCount];
    }
    FillIncidenceMatrix(incidenceMatrix, mainRowsCount, rowsCount, colsCount);

    linked_matrix_GJK::LMatrix DLXMatrix(incidenceMatrix, rowsCount, colsCount);
    std::list<std::vector<size_t>> solutions;
    dancing_links_GJK::Exact_Cover_Solver(DLXMatrix, solutions);

    for (size_t i = 0; i < rowsCount; ++i)
    {
        delete[] incidenceMatrix[i];
    }
    delete[] incidenceMatrix;

    size_t size = mRegion->SizeGet();
    mDLXSolutions.reserve(solutions.size());
    mValidSolution.reserve(solutions.size());
    for(const auto& sol : solutions)
    {
        mDLXSolutions.push_back(std::vector<unsigned short>(size));
        mValidSolution.push_back(true);
        for (const auto& r : sol)
        {
            if(r >= mainRowsCount)
            {
                continue;
            }
            auto p = PossibilityFromRow(r);
            unsigned short index = mCellToOrder[p.first];
            mDLXSolutions.back()[index] = p.second;
        }
    }

    UpdateAllowedAndConfirmedValues();
}

unsigned int KillerConstraint::SumGet() const
{
    return mCageSum;
}

void KillerConstraint::OnAllowedValueRemoved(unsigned short value)
{
    Q_UNUSED(value);
}

void KillerConstraint::OnConfimedValueAdded(unsigned short value)
{
    if(mConfirmedValues.count(value) == 0)
    {
        RemoveCombinationsWithoutValue(value);
    }
}

void KillerConstraint::OnOptionRemovedFromCell(unsigned short value, SudokuCell* cell)
{
    unsigned short index = mCellToOrder[cell->IdGet()];
    bool removed = false;
    for (size_t i = 0; i < mDLXSolutions.size(); ++i)
    {
        if(mValidSolution.at(i) && mDLXSolutions.at(i).at(index) == value)
        {
            mValidSolution.at(i) = false;
            removed = true;
        }
    }
    mAllowedValues.at(index).erase(value);

    if(removed)
    {
        UpdateAllowedAndConfirmedValues();
    }
}

void KillerConstraint::OnRegionPartitioned(Region* leftNode, Region* rightNode)
{
    assert(!leftNode || leftNode->IsClosed());

    // If the right region is closed, knowing its sum would be redundant
    if (!rightNode || rightNode->IsClosed())
    {
        return;
    }

    int leftTotal = 0;
    if(leftNode)
    {
        leftTotal = leftNode->SumGet();
    }
    std::unique_ptr<VariantConstraint> kc = std::make_unique<KillerConstraint>(mCageSum - leftTotal);
    rightNode->AddVariantConstraint(std::move(kc));
}

VariantConstraint *KillerConstraint::DeepCopy() const
{
    return new KillerConstraint(mCageSum);
}

void KillerConstraint::TakeSnaphot()
{
    mSnapshot = std::make_unique<Snapshot>(mValidSolution, mConfirmedValues, mAllowedValues);
}

void KillerConstraint::RestoreSnaphot()
{
    if(mSnapshot)
    {
        mValidSolution = std::move(mSnapshot->mValidSolution);
        mConfirmedValues = std::move(mSnapshot->mConfirmedValues);
        mAllowedValues = std::move(mSnapshot->mAllowedValues);
        mSnapshot.reset();
    }
}

RegionType KillerConstraint::TypeGet()
{
    return RegionType::KillerCage;
}

const std::list<std::set<unsigned short> > &KillerConstraint::CombinationsGet() const
{
    return mCombinations;
}

void KillerConstraint::FindCombinations(std::list<unsigned short> allowedValues)
{
    mCombinations.clear();
    std::list<unsigned short> combination;
    unsigned int total = 0;
    for(const auto& v : mRegion->ConfirmedValuesGet())
    {
        total += v;
        combination.push_back(v);
        allowedValues.remove(v);
    }
    allowedValues.sort();
    FindCombinationsInner(allowedValues.begin(), allowedValues, total, combination);
}

void KillerConstraint::FindCombinationsInner(std::list<unsigned short>::const_iterator it, const std::list<unsigned short> &allowedValues, unsigned int runningTotal, std::list<unsigned short>& combination)
{
    if (combination.size() == mRegion->SizeGet())
    {
        if (mCageSum == runningTotal)
        {
            mCombinations.emplace_back(combination.begin(), combination.end());
        }
        return;
    }
    else
    {
        int missingValues = mRegion->SizeGet() - combination.size();
        while(std::distance(it, allowedValues.end()) >= missingValues)
        {
            unsigned int v = *it;
            unsigned int minPossibleTotal = runningTotal + v + (missingValues - 1) * missingValues / 2;
            if(minPossibleTotal > mCageSum) return;

            combination.push_back(v);
            FindCombinationsInner(++it, allowedValues, runningTotal + v, combination);
            combination.pop_back();
        }
    }

}

void KillerConstraint::RemoveCombinationsWithoutValue(unsigned short value)
{
    bool removed = false;
    for (size_t i = 0; i < mDLXSolutions.size(); ++i)
    {
        if(mValidSolution.at(i) &&
           std::find(mDLXSolutions.at(i).begin(), mDLXSolutions.at(i).end(), value) == mDLXSolutions.at(i).end())
        {
            mValidSolution.at(i) = false;
            removed = true;
        }
    }

    if (removed)
    {
        UpdateAllowedAndConfirmedValues();
    }
}

void KillerConstraint::AddConfirmedValue(unsigned value)
{
    if (mConfirmedValues.insert(value).second && !mRegion->HasConfirmedValue(value))
    {
        mRegion->GridGet()->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_ValueForcedInKiller>(mRegion, value));
    }
}

void KillerConstraint::UpdateAllowedAndConfirmedValues()
{
    SudokuGrid* grid = mRegion->GridGet();
    std::vector<std::set<unsigned short>> optionsForbiddenInCell = mAllowedValues;
    std::set<unsigned short> newConfirmed;

    for (unsigned short i = 1; i <= grid->SizeGet(); ++i)
    {
        if(mConfirmedValues.count(i) == 0)
        {
            newConfirmed.insert(i);
        }
    }

    for (size_t s = 0; s < mDLXSolutions.size(); ++s)
    {
        if(!mValidSolution.at(s)) continue;

        bool canEliminate = false;
        const auto& solution = mDLXSolutions.at(s);
        for (size_t i = 0; i < optionsForbiddenInCell.size(); ++i)
        {
            if(!optionsForbiddenInCell.at(i).empty())
            {
                canEliminate = true;
                optionsForbiddenInCell.at(i).erase(solution.at(i));
            }
        }

        auto cIt = newConfirmed.begin();
        while ( cIt!=newConfirmed.end() )
        {
            if(std::find(solution.begin(), solution.end(), *cIt) == solution.end())
            {
                cIt = newConfirmed.erase(cIt);
            }
            else
            {
                ++cIt;
            }
        }

        if(newConfirmed.empty() && !canEliminate)
        {
            break;
        }
    }

    for (size_t i = 0; i < optionsForbiddenInCell.size(); ++i)
    {
        if(!optionsForbiddenInCell.at(i).empty())
        {
            SudokuCell* cell = grid->CellGet(mOrderToCell[i]);
            for (const auto v : optionsForbiddenInCell.at(i))
            {
                mAllowedValues.at(i).erase(v);
            }
            grid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_ValueNotInKiller>(cell, mRegion, std::move(optionsForbiddenInCell.at(i))));
        }
    }

    for (const auto& v : newConfirmed)
    {
        AddConfirmedValue(v);
    }
}

size_t KillerConstraint::RowFromPossibility(CellId cell, unsigned short value) const
{
    assert(value > 0 && mCellToOrder.count(cell) > 0);

    size_t size = mRegion->GridGet()->SizeGet();
    return mCellToOrder.at(cell) * size + value - 1;
}

std::pair<CellId, unsigned short> KillerConstraint::PossibilityFromRow(size_t row) const
{
    size_t size = mRegion->GridGet()->SizeGet();

    size_t pos = static_cast<CellId>(row/size);
    CellId id = mOrderToCell.at(pos);
    unsigned short value = static_cast<unsigned short>(row % size + 1);

    return {id, value};
}

void KillerConstraint::FillIncidenceMatrix(bool** mIncidenceMatrix, size_t mMainRowsCount, size_t mRowsCount, size_t column)
{
    unsigned short size = mRegion->GridGet()->SizeGet();
    size_t c = 0;
    size_t sec_r = mMainRowsCount;

    // single value per cell constraint
    for(size_t i = 0; i < mRegion->SizeGet(); ++i)
    {
        for(size_t r = 0; r < mRowsCount; ++r)
        {
            if(r >= mMainRowsCount)
            {
                mIncidenceMatrix[r][c] = 0;
                continue;
            }
            const std::pair<CellId, unsigned short> p = PossibilityFromRow(r);
            unsigned short pos = mCellToOrder.at(p.first);
            mIncidenceMatrix[r][c] = (pos == i);
        }
        ++c;
    }


    // killer cages
    // |Ka1 Ka2 Ka3 ... |
    {
        size_t start_c = c;
        for(size_t i = 1; i <= size; ++i)
        {
            for(size_t r = 0; r < mRowsCount; ++r)
            {
                if(r >= mMainRowsCount)
                {
                    mIncidenceMatrix[r][c] = 0;
                    continue;
                }
                const std::pair<CellId, unsigned short> p = PossibilityFromRow(r);
                unsigned short value = p.second;
                mIncidenceMatrix[r][c] = (value == i);
            }
            ++c;
        }
        for(const auto& combination: mCombinations)
        {
            for(size_t i = 1; i <= size; ++i)
            {
                mIncidenceMatrix[sec_r][start_c + i - 1] = (combination.count(i) == 0);
            }
            ++sec_r;
        }
    }

    for (const auto& c : mRegion->CellsGet())
    {
        for(size_t i = 1; i <= size; ++i)
        {
            if(!c->HasGuess(i))
            {
                size_t r = RowFromPossibility(c->IdGet(), i);
                ClearIncidenceMatrixRow(r, mIncidenceMatrix, column);
            }
        }
    }
}

bool KillerConstraint::ClearIncidenceMatrixRow(size_t row, bool** mIncidenceMatrix, size_t mColsCount)
{
    bool result = false;
    for(size_t col = 0; col < mColsCount; ++col)
    {
        result = mIncidenceMatrix[row][col] || result;
        mIncidenceMatrix[row][col] = 0;
    }

    return result;
}
