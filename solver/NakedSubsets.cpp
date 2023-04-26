#include "SolvingTechnique.h"
#include "SudokuCell.h"
#include "Region.h"
#include "GridProgressManager.h"
#include "SudokuGrid.h"
#include <algorithm>
#include <iterator>
#include <list>
#include <map>
#include <string>

typedef CellList::iterator CellListIter;

bool NakedSubsetTechnique::SearchNakedSubsetInner(const CellList& cells, CellList::iterator it, const CellList::iterator endIt, const CellSet& excludeCells, std::set<unsigned short> &candidates, CellList &outNakedSubset, const size_t targetSize, bool& impossible)
{
    if (impossible)
    {
        return false;
    }
    else if (outNakedSubset.size() == targetSize && candidates.size() == targetSize) // naked subset found
    {
        return true;
    }
    else if (outNakedSubset.size() == targetSize || candidates.size() > targetSize) // naked subset cannot exist with this combination of cells
    {
        return false;
    }
    else if (outNakedSubset.size() > candidates.size()) // We need to place N values in M cells with N < M. That's not going to work wothout repeating a number.
    {
        impossible = true;
        return false;
    }
    else
    {
        while (std::distance(it, endIt) >= static_cast<int>(targetSize) - static_cast<int>(outNakedSubset.size())) // make sure the iterator is followed by enough elemnts to reach the desired subset size
        {
            if (excludeCells.find(*it) != excludeCells.end())
            {
                ++it;
            }
            else
            {
                std::set<unsigned short> newCandidates;
                std::merge((*it)->OptionsGet().begin(), (*it)->OptionsGet().end(),
                    candidates.begin(), candidates.end(),
                    std::inserter(newCandidates, newCandidates.begin()));
                // add the current cell to the naked subset
                outNakedSubset.push_back(*it);

                if (SearchNakedSubsetInner(cells, ++it, endIt, excludeCells, newCandidates, outNakedSubset, targetSize, impossible))
                {
                    candidates = newCandidates;
                    return true;
                }

                // backtrack
                outNakedSubset.pop_back();
            }
        }
        return false;
    }
}

void NakedSubsetTechnique::SearchNakedSubsets(const CellSet& cellsToSearch)
{
    static constexpr size_t s_minSubsetSize = 1;

    CellList cells(mCurrentRegion->CellsGet().begin(), mCurrentRegion->CellsGet().end());

    // sort the cells from lowest to largest number of viable options. This allows to stop
    // the search early if a cell with more candidates than the subset size is reached.
    cells.sort([](SudokuCell* const &a, SudokuCell* const &b) { return a->OptionsGet().size() < b->OptionsGet().size(); });

    auto endIt = cells.begin();

    // the size of the subset is at least as great as the number of viable options for the
    // first cell in the list (rember the list is sorted lowest to largest number of viable options)
    size_t minSubsetSize = std::max(s_minSubsetSize, (*endIt)->OptionsGet().size());

    // if the region has size N we only need to check for naked subset up to a size of N-1,
    // since a set of size N would obviously be naked but would not allow us to make any progress
    // on the puzzle
    unsigned short maxSubsetSize = cells.size() - 1;
    if (mCurrentRegion->IsClosed())
    {
        maxSubsetSize = mCurrentRegion->CellsGet().size() / 2;
    }
    for (size_t subsetSize = minSubsetSize; subsetSize <= maxSubsetSize; ++subsetSize)
    {
        // we will end the iteration on the first cell that has too many options for the subset size we are aiming for
        while (endIt != cells.end() && (*endIt)->OptionsGet().size() <= subsetSize)
        {
            ++endIt;
        }

        CellSet excludeCells;

        for (SudokuCell* c : cellsToSearch)
        {
            bool impossible = false;
            CellList nakedSubset;
            std::set<unsigned short> candidateValues(c->OptionsGet().begin(), c->OptionsGet().end());
            nakedSubset.push_back(c);
            excludeCells.insert(c);

            if (SearchNakedSubsetInner(cells, cells.begin(), endIt, excludeCells, candidateValues, nakedSubset, subsetSize, impossible))
            {
                mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_NakedSubset>(std::move(nakedSubset), std::move(candidateValues)));
                return;
            }
            else if (impossible)
            {
                mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_TooFewValuesForRegion>(std::move(nakedSubset), std::move(candidateValues), mGrid));
            }
        }
    }
    for (SudokuCell* c : cellsToSearch)
    {
        mGrid->ProgressManagerGet()->RegisterFailure(TechniqueType::NakedSubset, mCurrentRegion, c);
    }
}


bool HiddenSubsetTechnique::SearchHiddenSubsetInner(const std::list<ValueMapEntry>& values, std::list<ValueMapEntry>::iterator it, const std::list<ValueMapEntry>::iterator endIt, const std::set<unsigned short>& excludeValues, std::list<unsigned short>& candidates, CellSet& outNakedSubset, const size_t subsetFinalSize, bool& impossible)
{
    if (impossible)
    {
        return false;
    }
    if (outNakedSubset.size() == subsetFinalSize && candidates.size() == subsetFinalSize) // naked subset found
    {
        return true;
    }
    else if (candidates.size() == subsetFinalSize || outNakedSubset.size() > subsetFinalSize) // naked subset cannot exist with this combination of values
    {
        return false;
    }
    else if (candidates.size() > outNakedSubset.size()) // We need to place N values in M cells with N > M. That's not going to work as some numbers will be left out.
    {
        impossible = true;
        return false;
    }
    else
    {
        while (std::distance(it, endIt) >= static_cast<int>(subsetFinalSize) - static_cast<int>(candidates.size())) // make sure the iterator is followed by enough elemnts to reach the desired subset size
        {
            if (excludeValues.find(it->first) != excludeValues.end())
            {
                ++it;
            }
            else
            {
                CellSet newSubset;
                std::merge(it->second.begin(), it->second.end(),
                    outNakedSubset.begin(), outNakedSubset.end(),
                    std::inserter(newSubset, newSubset.begin()));
                // add the current cell to the naked subset
                candidates.push_back(it->first);

                if (SearchHiddenSubsetInner(values, ++it, endIt, excludeValues, candidates, newSubset, subsetFinalSize, impossible))
                {
                    outNakedSubset = newSubset;
                    return true;
                }

                // backtrack
                candidates.pop_back();
            }
        }
        return false;
    }
}

void HiddenSubsetTechnique::SearchHiddenSubsets(const std::set<unsigned short>& valuesToSearch)
{
    static constexpr size_t s_minSubsetSize = 1;
    const std::set<unsigned short>& confirmedValues = mCurrentRegion->ConfirmedValuesGet();

    std::list<ValueMapEntry> valueToCellMap; // maps each value to the cells than can host it

    for (const unsigned short& v : confirmedValues)
    {
        valueToCellMap.push_back(ValueMapEntry(v, mCurrentRegion->CellsWithValueGet(v)));
    }

    std::set<unsigned short> values;
    for (const unsigned short& v : valuesToSearch)
    {
        if (mCurrentRegion->HasConfirmedValue(v))
        {
            values.insert(v);
        }
        else
        {
            mGrid->ProgressManagerGet()->RegisterFailure(TechniqueType::HiddenSubset, mCurrentRegion, nullptr, v);
        }
    }

    // sort the values from lowest to largest number of viable hosting cells. This allows to stop
    // the search early if a value with more hosts than the subset size is reached.
    valueToCellMap.sort([](const ValueMapEntry &a, const ValueMapEntry &b) { return a.second.size() < b.second.size(); });

    auto endIt = valueToCellMap.begin();

    // the size of the subset is at least as great as the number of viable cells for the first
    // value in the list (rember the list is sorted lowest to largest number of viable cells)
    size_t minSubsetSize = std::max(s_minSubsetSize, endIt->second.size());

    // if the region has size N we only neet to check for naked subset up to a size of N-1,
    // since a set of size N would obviously be naked but would not allow us to make any progress
    // on the puzzle
    unsigned short maxSubsetSize = std::min(valueToCellMap.size(), mCurrentRegion->CellsGet().size() - 1);
    if (mCurrentRegion->IsClosed())
    {
        maxSubsetSize = mCurrentRegion->CellsGet().size() / 2;
    }
    for (size_t subsetSize = minSubsetSize; subsetSize <= maxSubsetSize; ++subsetSize)
    {
        // we will end the iteration on the first value that can go in too many cells
        // for the subset size we are aiming for
        while (endIt != valueToCellMap.end() && endIt->second.size() <= subsetSize)
        {
            ++endIt;
        }

        std::set<unsigned short> excludeValues;

        for (const unsigned short& v : values)
        {
            bool impossible = false;
            std::list<ValueMapEntry>::iterator startingCells = std::find_if(valueToCellMap.begin(), valueToCellMap.end(), [=](ValueMapEntry a) { return a.first == v; });
            CellSet nakedSubset(startingCells->second.begin(), startingCells->second.end());
            std::list<unsigned short> candidateValues;
            candidateValues.push_back(v);
            excludeValues.insert(v);

            if (SearchHiddenSubsetInner(valueToCellMap, valueToCellMap.begin(), endIt, excludeValues, candidateValues, nakedSubset, subsetSize, impossible))
            {
                mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_HiddenSubset>(std::move(nakedSubset), mCurrentRegion, std::move(candidateValues)));
                return;
            }
            else if (impossible)
            {
                mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_TooManyValuesForRegion>(std::move(nakedSubset), std::move(candidateValues), mGrid));
                return;
            }
        }
    }
    for (const unsigned short& v : valuesToSearch)
    {
        mGrid->ProgressManagerGet()->RegisterFailure(TechniqueType::HiddenSubset, mCurrentRegion, nullptr, v);
    }
}
