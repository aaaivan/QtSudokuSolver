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
    mAllowedValues(),
    mSnapshot(nullptr)
{
}

KillerConstraint::~KillerConstraint()
{
}

void KillerConstraint::Initialise(Region* region)
{
    VariantConstraint::Initialise(region);
    mSnapshot.reset();

    mAllowedValues.insert(mRegion->AllowedValuesGet().begin(), mRegion->AllowedValuesGet().end());
    FindCombinations();
    UpdateAllowedAndConfirmedValues();
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

VariantConstraint *KillerConstraint::DeepCopy() const
{
    return new KillerConstraint(mCageSum);
}

void KillerConstraint::TakeSnaphot()
{
    mSnapshot = std::make_unique<Snapshot>(mConfirmedValues, mAllowedValues, mCombinations);
}

void KillerConstraint::RestoreSnaphot()
{
    if(mSnapshot)
    {
        mConfirmedValues = std::move(mSnapshot->mConfirmedValues);
        mAllowedValues = std::move(mSnapshot->mAllowedValues);
        mCombinations = std::move(mSnapshot->mCombinations);
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

const std::set<unsigned short> &KillerConstraint::ConfirmedValuesGet() const
{
    return mConfirmedValues;
}

const std::set<unsigned short> &KillerConstraint::AllowedValuesGet() const
{
    return mAllowedValues;
}

void KillerConstraint::FindCombinations()
{
    mCombinations.clear();
    std::list<unsigned short> combination;
    std::list<unsigned short> allowedValues(mAllowedValues.begin(), mAllowedValues.end());
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
    if (mConfirmedValues.count(value) > 0)
    {
        mRegion->GridGet()->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_NoKillerSum>(this, mRegion->GridGet()));
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
        mRegion->GridGet()->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_NoKillerSum>(this, mRegion->GridGet()));
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
    std::set<unsigned short> disallowedValues;
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

            if (!isAllowed)
            {
                disallowedValues.insert(v);
            }
            else if (isConfirmed)
            {
                AddConfirmedValue(v);
            }
        }
    }
    for(const auto& v : disallowedValues)
    {
        RemoveAllowedValue(v);
    }
}
