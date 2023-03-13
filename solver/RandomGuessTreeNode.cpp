#include "RandomGuessTreeNode.h"
#include "SudokuGrid.h"
#include "SudokuCell.h"
#include "GridProgressManager.h"
#include <cassert>

RandomGuessTreeRoot::RandomGuessTreeRoot(SudokuGrid *grid, SudokuCell* pivot, BifurcationTechnique* technique):
    mGrid(grid),
    mPivot(pivot),
    mNextNode(0),
    mChildNodes(),
    mHotCells(),
    mFinished(false),
    mTechnique(technique)
{
    size_t maxCells = mGrid->SizeGet() * mGrid->SizeGet();
    for (size_t i = 0; i < maxCells; ++i)
    {
        unsigned short row = i / mGrid->SizeGet();
        unsigned short col = i % mGrid->SizeGet();
        SudokuCell* cell = mGrid->CellGet(row, col);
        if(mTechnique->mCellOrder.count(cell) > 0)
        {
            mHotCells.insert(i);
        }
    }

    size_t i = 0;
    mChildNodes.reserve(pivot->OptionsGet().size());
    for(const auto& v : pivot->OptionsGet())
    {
        mChildNodes.push_back(std::make_unique<RandomGuessTreeNode>(mGrid, mPivot, v, this, i, mTechnique));
        ++i;
    }
}

bool RandomGuessTreeRoot::HasFinished() const
{
    return mFinished;
}

void RandomGuessTreeRoot::NextStep()
{
    GridProgressManager* progressManager = mGrid->ProgressManagerGet();

    if(mNextNode < mChildNodes.size())
    {
        mChildNodes.at(mNextNode)->NextStep();
    }
    else
    {
        // process results of the search
        bool valid = false;
        for (const auto& child : mChildNodes)
        {
            valid |= child->IsValidGet();
        }
        if(!valid)
        {
            progressManager->RegisterProgress(std::make_shared<Impossible_NoSolutionByBifurcation>(mPivot, mGrid));
        }
        mFinished = true;
    }
}

void RandomGuessTreeRoot::NodeHasFinished(unsigned short nodeIndex)
{
    GridProgressManager* progressManager = mGrid->ProgressManagerGet();
    if(!mChildNodes.at(mNextNode)->IsValidGet())
    {
        unsigned short option = mChildNodes.at(mNextNode)->PivotValueGet();
        progressManager->RegisterProgress(std::make_shared<Progress_OptionRemovedViaGuessing>(mPivot, option));
    }
    mNextNode = nodeIndex + 1;

    std::set<unsigned int> mColdCells;
    for (const auto& id : mHotCells)
    {
        unsigned short row = id / mGrid->SizeGet();
        unsigned short col = id % mGrid->SizeGet();
        SudokuCell* cell = mGrid->CellGet(row, col);

        const std::set<unsigned short>& options = cell->OptionsGet();
        std::set<unsigned short> childrenOptions;
        for(size_t i = 0; i < mNextNode; ++i)
        {
            const auto& child = mChildNodes.at(i);
            if(child->IsValidGet())
            {
                const std::set<unsigned short>& opts = child->GridGet()->CellGet(row, col)->OptionsGet();
                childrenOptions.insert(opts.begin(), opts.end());
            }
        }
        bool isCold = true;
        for (const auto& v : options)
        {
            if(childrenOptions.count(v) == 0)
            {
                // for this cell, there is at least one option in the original node
                // that is not in any of the child nodes
                isCold = false;
                break;
            }
        }
        if(isCold)
        {
            mColdCells.insert(id);
        }
        else if(mNextNode == mChildNodes.size())
        {
            std::set<unsigned short> resultOptions = options;
            for (const auto& v : childrenOptions)
            {
                if(auto it = resultOptions.find(v); it != resultOptions.end())
                {
                    resultOptions.erase(it);
                }
            }
            progressManager->RegisterProgress(std::make_shared<Progress_ValueDisallowedByBifurcation>(mGrid->CellGet(row, col), std::move(resultOptions), mPivot));
        }
    }
    for (const auto& id  : mColdCells)
    {
        mHotCells.erase(mHotCells.find(id));
    }
    if(mHotCells.size() == 0)
    {
        mNextNode = mChildNodes.size();
    }
}

RandomGuessTreeNode::RandomGuessTreeNode(const SudokuGrid* grid, SudokuCell* pivotCell, unsigned short pivotValue, RandomGuessTreeRoot* parent, unsigned short index, BifurcationTechnique* technique):
    mIndex(index),
    mGrid(std::make_unique<SudokuGrid>(grid)),
    mRootGrid(grid),
    mParentNode(parent),
    mPivotCell(pivotCell),
    mPivotValue(pivotValue),
    mIsNodeValid(true),
    mTechnique(technique)
{
    unsigned short row = pivotCell->RowGet();
    unsigned short col = pivotCell->ColGet();
    SudokuCell* pivotCellCopy = mGrid->CellGet(row, col);
    pivotCellCopy->RemoveAllOtherOptions(pivotValue);

    size_t cellIndex = mTechnique->mCellOrder[pivotCell];
    const auto& optionsMap = mTechnique->mOptionEliminationMatrix.at(cellIndex);

    if(optionsMap.count(pivotValue) == 0)
    {
        return;
    }

    const auto& eliminationMap = optionsMap.at(pivotValue);
    for (const auto& elimMapEntry : eliminationMap)
    {
        SudokuCell* cell = mTechnique->mCells.at(elimMapEntry.first);
        const auto& options = elimMapEntry.second;
        mGrid->CellGet(cell->RowGet(), cell->ColGet())->RemoveMultipleOptions(options);
    }
}

bool RandomGuessTreeNode::IsValidGet() const
{
    return mIsNodeValid;
}

SudokuGrid *RandomGuessTreeNode::GridGet() const
{
    return mGrid.get();
}

unsigned short RandomGuessTreeNode::PivotValueGet() const
{
    return mPivotValue;
}

void RandomGuessTreeNode::NextStep()
{
    GridProgressManager* progressManager = mGrid->ProgressManagerGet();
    assert(mParentNode != nullptr);

    if(progressManager->HasFinished())
    {
        if(progressManager->HasAborted())
        {
            mIsNodeValid = false;
            mParentNode->NodeHasFinished(mIndex);
        }
        else
        {
            mIsNodeValid = true;
            mParentNode->NodeHasFinished(mIndex);
        }
    }
    else
    {
        progressManager->NextStep();
    }

}

void RandomGuessTreeNode::UpdateOptionEliminationMatrix() const
{
    size_t pivotCellIndex = mTechnique->mCellOrder.at(mPivotCell);
    unsigned short gridSize = mRootGrid->SizeGet();
    for (size_t i = 0; i < gridSize; i++)
    {
        for (size_t j = 0; j < gridSize; j++)
        {
            SudokuCell* rootCell = mRootGrid->CellGet(i, j);
            SudokuCell* cell = mGrid->CellGet(i, j);
            for (const auto& v : rootCell->OptionsGet())
            {
                if(!cell->HasGuess(v))
                {
                    size_t rootCellIndex = mTechnique->mCellOrder.at(rootCell);
                    auto& optionsMap = mTechnique->mOptionEliminationMatrix.at(rootCellIndex);
                    if(optionsMap.count(v) == 0)
                    {
                        optionsMap[v] = {};
                    }
                    auto& eliminationMap = optionsMap.at(v);
                    if(eliminationMap.count(pivotCellIndex) == 0)
                    {
                        eliminationMap[pivotCellIndex] = {};
                    }
                    eliminationMap.at(pivotCellIndex).insert(mPivotValue);
                }
            }
        }
    }
}
