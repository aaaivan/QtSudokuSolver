#include "SolvingTechnique.h"
#include "Region.h"
#include "VariantConstraints.h"
#include "SudokuGrid.h"
#include "RegionsManager.h"
#include "GridProgressManager.h"
#include "solver/GhostCagesManager.h"
#include "solver/SudokuCell.h"

void InniesAndOuties::SearchInnies()
{
    mKillerCombinations.clear();
    size_t maxCombinationSize = mContainedKillers.at(*mCurrentRegion).size();

    for (size_t i = 1; i <= maxCombinationSize; ++i)
    {
        CalculateCageCombinations(i, mContainedKillers.at(*mCurrentRegion));
        if(mKillerUnions.size() == 0)
        {
            mKillerUnions.clear();
            mKillerCombinations.clear();
            break;
        }

        for (auto& comb : mKillerUnions)
        {
            SearchInniesInner(comb);
        }
    }
}

void InniesAndOuties::SearchInniesInner(KillerCage_t& unionCage)
{
    CellSet cells = (*mCurrentRegion)->CellsGet();
    for (const auto& c : unionCage.second)
    {
        cells.erase(c);
    }

    int ghostCageTotal = static_cast<int>(mCurrentRegionTotal) - static_cast<int>(unionCage.first);
    if(ghostCageTotal < 0 || (ghostCageTotal == 0 && cells.size() > 0))
    {
        mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_BrokenInnie>(std::move(cells), mGrid));
        return;
    }
    else if(cells.size() == 0)
    {
        return;
    }

    for (const auto& k : mContainedKillers.at(*mCurrentRegion))
    {
        if(k->RegionGet()->IsContainedInCells(cells))
        {
            if(k->RegionGet()->SizeGet() == cells.size()
               && k->SumGet() != (unsigned int)ghostCageTotal)
            {
                mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_ClashingGhostCages>(std::move(cells), ghostCageTotal, k->SumGet(), mGrid));
            }
            return;
        }
    }

    KillerCage_t k = std::pair(ghostCageTotal, std::move(cells));
    if(mOutieCages.count(k) == 0)
    {
        mInnieCages.insert(std::move(k));
    }
}

void InniesAndOuties::SearchOuties()
{
    mKillerCombinations.clear();
    size_t maxCombinationSize = mIntersectingKillers.at(*mCurrentRegion).size();

    for (size_t i = 1; i <= maxCombinationSize; ++i)
    {
        CalculateCageCombinations(i, mIntersectingKillers.at(*mCurrentRegion));
        if(mKillerUnions.size() == 0)
        {
            mKillerUnions.clear();
            mKillerCombinations.clear();
            break;
        }

        for (auto& comb : mKillerUnions)
        {
            SearchOutiesInner(comb);
        }
    }
}

void InniesAndOuties::SearchOutiesInner(KillerCage_t &unionCage)
{
    const CellSet& regionCells = (*mCurrentRegion)->CellsGet();
    for (const auto& c : regionCells)
    {
        if(unionCage.second.erase(c) == 0)
        {
            return;
        }
    }
    CellSet cells = unionCage.second;
    int ghostCageTotal = static_cast<int>(unionCage.first) - static_cast<int>(mCurrentRegionTotal);
    if(ghostCageTotal < 0 || (ghostCageTotal == 0 && cells.size() > 0))
    {
        mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_BrokenOutie>(std::move(cells), mGrid));
        return;
    }
    else if(cells.size() == 0)
    {
        return;
    }

    RegionSet regions;
    mGrid->RegionsManagerGet()->RegionsWithCellsGet(regions, cells);
    if(regions.size() == 0)
    {
        int gridSize = mGrid->SizeGet();
        int minCandidate = ghostCageTotal - ((cells.size() - 1) * gridSize);
        minCandidate = std::max(minCandidate, 1);
        int maxCandidate = ghostCageTotal - (cells.size() - 1);
        maxCandidate = std::min(maxCandidate, gridSize);
        for (const auto& c : cells)
        {
            for (int i = minCandidate - 1; i > 0; --i)
            {
                if(c->HasGuess(i))
                {
                    mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_SplitOutie>(std::move(cells),ghostCageTotal, minCandidate, maxCandidate, mGrid));
                    return;
                }
            }
            for (int i = maxCandidate + 1; i < mGrid->SizeGet(); ++i)
            {
                if(c->HasGuess(i))
                {
                    mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Progress_SplitOutie>(std::move(cells),ghostCageTotal, minCandidate, maxCandidate, mGrid));
                    return;
                }
            }
        }

        return;
    }
    else
    {
        for (const auto& r : regions)
        {
            if(r->SizeGet() == cells.size())
            {
                if(r->SumGet() != (unsigned int)ghostCageTotal)
                {
                    mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_ClashingGhostCages>(std::move(cells), ghostCageTotal, r->SumGet(), mGrid));
                }
                return;
            }
        }
    }

    regions.clear();
    mGrid->GhostRegionsManagerGet()->RegionsWithCellsGet(regions, cells);
    for (const auto& r : regions)
    {
        if(r->SizeGet() == cells.size())
        {
            if(r->SumGet() != (unsigned int)ghostCageTotal)
            {
                mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_ClashingGhostCages>(std::move(cells), ghostCageTotal, r->SumGet(), mGrid));
            }
            return;
        }
    }

    KillerCage_t k = std::pair(ghostCageTotal, std::move(cells));
    if(mInnieCages.count(k) == 0)
    {
        mOutieCages.insert(std::move(k));
    }
}

void InniesAndOuties::CalculateCageCombinations(unsigned int size, const std::set<const KillerConstraint*>& availableKillers)
{
    mKillerUnions.clear();
    if(size == 1)
    {
        mKillerCombinations.clear();
        auto it = availableKillers.begin();
        auto end = availableKillers.end();
        while(it != end)
        {
            mKillerCombinations.push_back({it});
            mKillerUnions.emplace_back((*it)->SumGet(), (*it)->RegionGet()->CellsGet());
            ++it;
        }
    }
    else
    {
        std::list<KillerCombination> newCombinations;
        for (auto& oldComb : mKillerCombinations)
        {
            KillerSetIt startIt = oldComb.back();
            // Recursively get all possible killer unions
            CalculateCageCombinationsInner(newCombinations, ++startIt, availableKillers.end(), oldComb, size);
        }
        mKillerCombinations.clear();
        mKillerCombinations = std::move(newCombinations);
    }
}

void InniesAndOuties::CalculateCageCombinationsInner(std::list<KillerCombination>& outCombinations, KillerSetIt kIt, const KillerSetIt& end, KillerCombination& nextComb, unsigned int size)
{
    std::set<SudokuCell*> cells;
    unsigned int total = 0;
    for (const auto& k : nextComb)
    {
        total += (*k)->SumGet();
        const auto& c = (*k)->RegionGet()->CellsGet();
        cells.insert(c.begin(), c.end());
    }

    if (nextComb.size() == size) // the defining set has the desired size. We are done
    {
        outCombinations.emplace_back(nextComb.begin(), nextComb.end());
        mKillerUnions.emplace_back(total, cells);
    }
    else
    {
        // make sure the number of regions that are yet to be processed is at least as big
        // as the number of regions missing to reach the desired set size
        while (std::distance(kIt, end) >= (int)size - (int)nextComb.size())
        {
            bool prune = false;
            // check that the new region does not intersect one already in the set
            for (const auto& c : (*kIt)->RegionGet()->CellsGet())
            {
                if(cells.count(c) > 0)
                {
                    prune = true;
                    break;
                }
            }
            if(prune)
            {
                ++kIt;
                continue;
            }
            nextComb.push_back(kIt);
            ++kIt;
            CalculateCageCombinationsInner(outCombinations, kIt, end, nextComb, size);
            nextComb.pop_back();
        }
    }
}
