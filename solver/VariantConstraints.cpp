#include "VariantConstraints.h"
#include "Region.h"
#include "SudokuGrid.h"
#include "GridProgressManager.h"
#include "qglobal.h"
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
    mAllowedValues()
{
}

KillerConstraint::~KillerConstraint()
{
}

void KillerConstraint::Initialise(Region* region)
{
    VariantConstraint::Initialise(region);

    for (unsigned short v = 1; v <= mRegion->SizeGet(); ++v)
    {
        mAllowedValues.insert(v);
    }

    {
        std::list<unsigned short> combination;
        FindCombinations(1, mRegion->SizeGet() + 1, 0, combination);
        UpdateAllowedAndConfirmedValues();
    }

    for (const auto& v : mRegion->ConfirmedValuesGet())
    {
        RemoveCombinationsWithoutValue(v);
    }

    for (unsigned short v = 1; v <= mRegion->SizeGet(); ++v)
    {
        if (!mRegion->IsValueAllowed(v))
        {
            RemoveCombinationsWithValue(v);
        }
    }
}

unsigned short KillerConstraint::SumGet() const
{
    return mCageSum;
}

void KillerConstraint::OnAllowedValueRemoved(unsigned short value)
{
    RemoveCombinationsWithValue(value);
}

void KillerConstraint::OnConfimedValueAdded(unsigned short value)
{
    RemoveCombinationsWithoutValue(value);
}

void KillerConstraint::OnOptionRemovedFromCell(unsigned short value, SudokuCell* cell)
{
    Q_UNUSED(cell);
    Q_UNUSED(value);
    return;
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
    if (leftNode)
    {
        for (const auto& v : leftNode->ConfirmedValuesGet())
        {
            leftTotal += v;
        }
    }

    std::unique_ptr<VariantConstraint> kc = std::make_unique<KillerConstraint>(mCageSum - leftTotal);
    rightNode->AddVariantConstraint(std::move(kc));
}

RegionType KillerConstraint::TypeGet()
{
    return RegionType::KillerCage;
}

void KillerConstraint::FindCombinations(unsigned short min, unsigned short max, unsigned int runningTotal, std::list<unsigned short>& combination)
{
    if (combination.size() == mRegion->SizeGet())
    {
        if (mCageSum == runningTotal)
        {
            mCombinations.emplace_back(combination.begin(), combination.end());
        }
        return;
    }

    unsigned short n = mRegion->SizeGet() - combination.size();
    while (max - min >= n)
    {
        unsigned int minRunningTotal = runningTotal + (min * n) + (((n - 1) * n) / 2);
        if (minRunningTotal > mCageSum) break;

        combination.push_back(min);
        FindCombinations(min + 1, max, runningTotal + min, combination);
        combination.pop_back();
        ++min;
    }
}

void KillerConstraint::RemoveCombinationsWithValue(unsigned short value)
{
    if (mConfirmedValues.count(value) > 0)
    {
        mRegion->GridGet()->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_NoKillerSum>(this));
        return;
    }

    auto it = mCombinations.begin();
    bool removed = false;
    while (it != mCombinations.end())
    {
        if (it->count(value) > 0)
        {
            it = mCombinations.erase(it);
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
    if (mAllowedValues.count(value) == 0)
    {
        mRegion->GridGet()->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_NoKillerSum>(this));
        return;
    }

    auto it = mCombinations.begin();
    bool removed = false;
    while (it != mCombinations.end())
    {
        if (it->count(value) == 0)
        {
            it = mCombinations.erase(it);
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

void KillerConstraint::RemoveAllowedValue(unsigned short value)
{
    if (mAllowedValues.erase(value) > 0 && mRegion->IsValueAllowed(value))
    {
        mRegion->GridGet()->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_ValueNotInKiller>(mRegion, value));
    }
}

void KillerConstraint::UpdateAllowedAndConfirmedValues()
{
    for (const auto& v : mAllowedValues)
    {
        if (mConfirmedValues.count(v) == 0)
        {
            bool isConfirmed = true;
            bool isAllowed = false;
            for (const auto& c : mCombinations)
            {
                if (c.count(v) == 0)
                {
                    isConfirmed = false;
                }
                else
                {
                    isAllowed = true;
                }
            }
            if (isConfirmed)
            {
                AddConfirmedValue(v);
            }
            else if (!isAllowed)
            {
                RemoveAllowedValue(v);
            }
        }
    }
}
