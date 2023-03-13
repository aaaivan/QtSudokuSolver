#ifndef VARIANT_CONSTRAINTS_H
#define VARIANT_CONSTRAINTS_H

// Includes
#include "Types.h"

/// <summary>
/// Constraint other than standard Sudoku rules
/// </summary>
class VariantConstraint
{
protected:
    Region* mRegion;

public:
// Special member function

    VariantConstraint();
    virtual ~VariantConstraint();

// Public getters

    Region* RegionGet() const;
    virtual RegionType TypeGet() = 0;

// Non-constant methods

    /// <summary>
    /// Called as soon as the constraint is added to a region
    /// </summary>
    virtual void Initialise(Region* region);
    /// <summary>
    /// Called when an allowed value is removed from the region
    /// </summary>
    virtual void OnAllowedValueRemoved(unsigned short value) = 0;
    /// <summary>
    /// Called when a confirmed value is added from the region
    /// </summary>
    virtual void OnConfimedValueAdded(unsigned short value) = 0;
    /// <summary>
    /// Called when an option is removed from within a cell in the region
    /// </summary>
    virtual void OnOptionRemovedFromCell(unsigned short value, SudokuCell* cell) = 0;
    /// <summary>
    /// Called when the region is partitioned
    /// </summary>
    virtual void OnRegionPartitioned(Region* leftNode, Region* rightNode) = 0;
    /// <summary>
    /// Creates a deep copy of the class
    /// </summary>
    virtual VariantConstraint* DeepCopy() const = 0;
};

/// <summary>
/// Killer cage
/// </summary>
class KillerConstraint : public VariantConstraint
{
private:
    unsigned int mCageSum;								// sum of the digits in the cage
    std::list<std::set<unsigned short>> mCombinations;	// sets of numbers whose sum equals mCageSum and whose size euqals the size of the region
    std::set<unsigned short> mConfirmedValues;			// values that must be in the sum
    std::set<unsigned short> mAllowedValues;			// values that could be in the sum

public:
// Special member function

    KillerConstraint(unsigned int sum);
    ~KillerConstraint();

// Public getters

    unsigned short SumGet() const;
    RegionType TypeGet() override;

// Non-constant methods

    void Initialise(Region* region) override;
    void OnAllowedValueRemoved(unsigned short value) override;
    void OnConfimedValueAdded(unsigned short value) override;
    void OnOptionRemovedFromCell(unsigned short value, SudokuCell* cell) override;
    void OnRegionPartitioned(Region* leftNode, Region* rightNode) override;
    VariantConstraint* DeepCopy() const override;

private:
    /// <summary>
    /// Recursive function to find the sets of digits with size equal to the size of the region
    /// and whose memebers sum to mCageSum
    /// </summary>
    void FindCombinations();
    void FindCombinationsInner(std::list<unsigned short>::const_iterator it, const std::list<unsigned short> &allowedValues, unsigned int runningTotal, std::list<unsigned short>& combination);
    /// <summary>
    /// Remove all combinations where one of the digits is value
    /// </summary>
    void RemoveCombinationsWithValue(unsigned short value);
    /// <summary>
    /// Remove all combinations where none of the digits is value
    /// </summary>
    void RemoveCombinationsWithoutValue(unsigned short value);
    /// <summary>
    /// Specify a value that must be in the sum (e.g., 3 cells summing to 8 must contain a 1)
    /// </summary>
    void AddConfirmedValue(unsigned value);
    /// <summary>
    /// Specify a value that may not be in the sum (e.g., 3 cells summing to 19 cannot contain a 1)
    /// </summary>
    void RemoveAllowedValue(unsigned short value);
    /// <summary>
    /// Check whether we can add a confirmed value/remove an allowed value in the sum
    /// </summary>
    void UpdateAllowedAndConfirmedValues();
};

#endif // !VARIANT_CONSTRAINTS_H
