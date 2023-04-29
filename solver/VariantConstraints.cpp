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
    mCellToOrder(),
    mOrderToCell(),
    mDLXSolutions(),
    mSnapshot(nullptr)
{
}

void KillerConstraint::Initialise(Region* region)
{
    VariantConstraint::Initialise(region);
    mSnapshot.reset();

    mCellToOrder.clear();
    mOrderToCell.clear();
    mDLXSolutions.clear();

    unsigned short x = 0;
    for (const auto& c : region->CellsGet())
    {
        mCellToOrder[c->IdGet()] = x;
        mOrderToCell[x] = c->IdGet();
        ++x;
    }

    FindCombinations(std::list<unsigned short>(mRegion->AllowedValuesGet().begin(),
                     mRegion->AllowedValuesGet().end()));


    size_t gridSize = region->GridGet()->SizeGet();
    size_t mColsCount = region->SizeGet() + gridSize;
    size_t mMainRowsCount = region->SizeGet() * gridSize;
    size_t mRowsCount = mMainRowsCount + mCombinations.size();
    bool** mIncidenceMatrix = new bool*[mRowsCount];
    for (size_t i = 0; i < mRowsCount; ++i)
    {
        mIncidenceMatrix[i] = new bool[mColsCount];
    }
    FillIncidenceMatrix(mIncidenceMatrix, mMainRowsCount, mRowsCount, mColsCount);

    linked_matrix_GJK::LMatrix DLXMatrix(mIncidenceMatrix, mRowsCount, mColsCount);
    std::list<std::vector<size_t>> solutions;
    dancing_links_GJK::Exact_Cover_Solver(DLXMatrix, solutions);

    for (size_t i = 0; i < mRowsCount; ++i)
    {
        delete[] mIncidenceMatrix[i];
    }
    delete[] mIncidenceMatrix;

    size_t size = mRegion->SizeGet();
    for(const auto& sol : solutions)
    {
        mDLXSolutions.push_back(std::vector<unsigned short>(size));
        for (const auto& r : sol)
        {
            if(r >= mMainRowsCount)
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
    auto it = mDLXSolutions.begin();
    bool removed = false;
    while (it != mDLXSolutions.end())
    {
        if(it->at(index) == value)
        {
            it = mDLXSolutions.erase(it);
            removed = true;
        }
        else
        {
            ++it;
        }
    }
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
    mSnapshot = std::make_unique<Snapshot>(mDLXSolutions, mConfirmedValues);
}

void KillerConstraint::RestoreSnaphot()
{
    if(mSnapshot)
    {
        mDLXSolutions = std::move(mSnapshot->mDLXSolutions);
        mConfirmedValues = std::move(mSnapshot->mConfirmedValues);
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

void KillerConstraint::RemoveCombinationsWithValue(unsigned short value)
{
    bool removed = false;
    auto it = mDLXSolutions.begin();
    while (it != mDLXSolutions.end())
    {
        if(std::find(it->begin(), it->end(), value) != it->end())
        {
            it = mDLXSolutions.erase(it);
            removed = true;
        }
        else
        {
            ++it;
        }
    }

    if (removed)
    {
        UpdateAllowedAndConfirmedValues();
    }
}

void KillerConstraint::RemoveCombinationsWithoutValue(unsigned short value)
{
    bool removed = false;
    auto it = mDLXSolutions.begin();
    while (it != mDLXSolutions.end())
    {
        if(std::find(it->begin(), it->end(), value) == it->end())
        {
            it = mDLXSolutions.erase(it);
            removed = true;
        }
        else
        {
            ++it;
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
    std::vector<std::set<unsigned short>> optionsInCell(mCellToOrder.size(), std::set<unsigned short>());
    std::vector<size_t> valuesCount(grid->SizeGet(), 0);

    for (const auto& s : mDLXSolutions)
    {
        for (size_t i = 0; i < s.size(); ++i)
        {
            unsigned short candidate = s.at(i);
            optionsInCell.at(i).insert(candidate);
            valuesCount[candidate - 1]++;
        }
    }

    for (size_t i = 0; i < optionsInCell.size(); ++i)
    {
        SudokuCell* cell = grid->CellGet(mOrderToCell[i]);
        if(optionsInCell.at(i).size() < cell->OptionsGet().size())
        {
            std::set<unsigned short> optionsToRemove;
            for (const auto& v : cell->OptionsGet())
            {
                if(optionsInCell.at(i).count(v) == 0)
                {
                    optionsToRemove.insert(v);
                }
            }
            if(optionsToRemove.size() > 0)
            {
                grid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_ValueNotInKiller>(cell, mRegion, std::move(optionsToRemove)));
            }
        }
    }

    for (size_t i = 0; i < valuesCount.size(); ++i)
    {
        if(valuesCount.at(i) == mDLXSolutions.size())
        {
            AddConfirmedValue(i + 1);
        }
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
