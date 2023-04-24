#include "SolvingTechnique.h"
#include "Region.h"
#include "VariantConstraints.h"
#include "SudokuGrid.h"
#include "RegionsManager.h"
#include "GridProgressManager.h"

void InniesAndOuties::SearchInnies()
{
    mKillerCombinations.clear();
    size_t maxCombinationSize = mContainedKillers.at(*mCurrentRegion).size();

    for (size_t i = 1; i <= maxCombinationSize; ++i)
    {
        CalculateContainedCageCombinations(i);

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

    for (const auto& k : mContainedKillers.at(*mCurrentRegion))
    {
        if(k->RegionGet()->IsContainedInCells(cells))
        {
            if(k->SumGet() > ghostCageTotal)
            {
                mGrid->ProgressManagerGet()->RegisterProgress(std::make_shared<Impossible_ClashingInnies>(std::move(cells), ghostCageTotal, k->SumGet() > ghostCageTotal, mGrid));
            }
            return;
        }
    }

    if(ghostCageTotal > 0 && cells.size() > 0)
    {
        mGhostCages.insert(std::pair(ghostCageTotal, std::move(cells)));
    }
}

void InniesAndOuties::CalculateContainedCageCombinations(unsigned int size)
{
    mKillerUnions.clear();
    if(size == 1)
    {
        mKillerCombinations.clear();
        auto it = mContainedKillers.at(*mCurrentRegion).begin();
        auto end = mContainedKillers.at(*mCurrentRegion).end();
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
            // Recursively get all possible defining sets
            CalculateContainedCageCombinationsInner(newCombinations, ++startIt, mContainedKillers.at(*mCurrentRegion).end(), oldComb, size);
        }
        mKillerCombinations.clear();
        mKillerCombinations = std::move(newCombinations);
    }
}

void InniesAndOuties::CalculateContainedCageCombinationsInner(std::list<KillerCombination>& outCombinations, KillerSetIt kIt, const KillerSetIt& end, KillerCombination& nextComb, unsigned int size)
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
            CalculateContainedCageCombinationsInner(outCombinations, kIt, end, nextComb, size);
            nextComb.pop_back();
        }
    }
}

