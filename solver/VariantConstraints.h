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
    virtual void TakeSnaphot() = 0;
    virtual void RestoreSnaphot() = 0;
};

/// <summary>
/// Killer cage
/// </summary>
class KillerConstraint : public VariantConstraint
{
private:
    unsigned int mCageSum;								// sum of the digits in the cage
    std::list<std::set<unsigned short>> mCombinations;	// sets of numbers whose sum equals mCageSum and whose size euqals the size of the region
    std::set<unsigned short> mConfirmedValues;

    std::map<CellId, unsigned short> mCellToOrder;
    std::map<unsigned short, CellId> mOrderToCell;
    bool** mIncidenceMatrix;
    size_t mRowsCount;
    size_t mColsCount;
    size_t mMainRowsCount;

    struct Snapshot
    {
        bool** mIncidenceMatrix;
        size_t mRowsCount;
        size_t mColsCount;
        std::set<unsigned short> mConfirmedValues;

        Snapshot(bool** incidenceMatrix, size_t rows, size_t cols, std::set<unsigned short> confirmedValues):
            mRowsCount(rows),
            mColsCount(cols),
            mConfirmedValues(confirmedValues)
        {
            mIncidenceMatrix = new bool*[mRowsCount];
            for (size_t i = 0; i < mRowsCount; ++i)
            {
                mIncidenceMatrix[i] = new bool[mColsCount];
            }
            for (size_t r = 0; r < mRowsCount; ++r)
            {
                for (size_t c = 0; c < mColsCount; ++c)
                {
                    mIncidenceMatrix[r][c] = incidenceMatrix[r][c];
                }
            }
        }
        ~Snapshot()
        {
            for (size_t i = 0; i < mRowsCount; ++i)
            {
                delete[] mIncidenceMatrix[i];
            }
            delete[] mIncidenceMatrix;
        }
    };
    std::unique_ptr<Snapshot> mSnapshot;

public:
// Special member function

    KillerConstraint(unsigned int sum);
    ~KillerConstraint();

// Public getters

    unsigned short SumGet() const;
    RegionType TypeGet() override;
    const std::list<std::set<unsigned short>>& CombinationsGet() const;
    const std::set<unsigned short>& ConfirmedValuesGet() const;

// Non-constant methods

    void Initialise(Region* region) override;
    void OnAllowedValueRemoved(unsigned short value) override;
    void OnConfimedValueAdded(unsigned short value) override;
    void OnOptionRemovedFromCell(unsigned short value, SudokuCell* cell) override;
    void OnRegionPartitioned(Region* leftNode, Region* rightNode) override;
    VariantConstraint* DeepCopy() const override;
    void TakeSnaphot() override;
    void RestoreSnaphot() override;

private:
    /// <summary>
    /// Recursive function to find the sets of digits with size equal to the size of the region
    /// and whose memebers sum to mCageSum
    /// </summary>
    void FindCombinations(std::list<unsigned short> allowedValues);
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
    /// Check whether we can add a confirmed value/remove an allowed value in the sum
    /// </summary>
    void UpdateAllowedAndConfirmedValues();

    size_t RowFromPossibility(CellId cell, unsigned short value) const;
    std::pair<CellId, unsigned short> PossibilityFromRow(size_t row) const;
    void FillIncidenceMatrix();
    bool ClearIncidenceMatrixRow(size_t row);
};

#endif // !VARIANT_CONSTRAINTS_H
