#ifndef SCANNING_H
#define SCANNING_H

// includes
#include "Types.h"

/// <summary>
/// Given a naked subset, remove its values from the options of all cell that "see" it.
/// </summary>
void ScanNaked(const CellSet& nakedSubset, const std::set<unsigned short>& cellValues, const SudokuGrid* grid, bool isHidden);

/// <summary>
/// Given a subset of locked candidates in a region, remove its values from the other cells not in the subset.
/// </summary>
void ScanLocked(const CellSet& intersection, const Region* intersectingRegions, unsigned short lockedValue);

#endif // SCANNING_H