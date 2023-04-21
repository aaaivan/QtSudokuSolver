#include "RandomGuessTreeNode.h"
#include "SudokuGrid.h"
#include "SudokuCell.h"
#include "GridProgressManager.h"
#include <cassert>

RandomGuessTreeRoot::RandomGuessTreeRoot(SudokuGrid* parentGrid, SudokuGrid* bifurcationGrid, CellId pivot, BifurcationTechnique* technique):
    mParentGrid(parentGrid),
    mBifurcationGrid(bifurcationGrid),
    mPivot(pivot),
    mNextNode(0),
    mChildNodes(),
    mEliminatedValues(),
    mFinished(false),
    mTechnique(technique)
{
    size_t maxCells = mParentGrid->SizeGet() * mParentGrid->SizeGet();
    for (size_t i = 0; i < maxCells; ++i)
    {
        SudokuCell* cell = mParentGrid->CellGet(i);
        mEliminatedValues[i] = (cell->OptionsGet());
    }

    size_t i = 0;
    mChildNodes.reserve(mParentGrid->CellGet(mPivot)->OptionsGet().size());
    for(const auto& v : mParentGrid->CellGet(mPivot)->OptionsGet())
    {
        mChildNodes.push_back(std::make_unique<RandomGuessTreeNode>(mBifurcationGrid, mPivot, v, this, i, mTechnique));
        ++i;
    }
    mChildNodes.begin()->get()->Init();
}

bool RandomGuessTreeRoot::HasFinished() const
{
    return mFinished;
}

void RandomGuessTreeRoot::NextStep()
{
    GridProgressManager* progressManager = mParentGrid->ProgressManagerGet();

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
            progressManager->RegisterProgress(std::make_shared<Impossible_NoSolutionByBifurcation>(mParentGrid->CellGet(mPivot), mParentGrid));
        }
        mFinished = true;
    }
}

void RandomGuessTreeRoot::NodeHasFinished(unsigned short nodeIndex)
{
    GridProgressManager* progressManager = mParentGrid->ProgressManagerGet();
    bool isValid = mChildNodes.at(nodeIndex)->IsValidGet();
    mNextNode = nodeIndex + 1;

    if(isValid)
    {
        auto it = mEliminatedValues.begin();
        auto endIt = mEliminatedValues.end();
        while (it != endIt)
        {
            SudokuCell* cell = mBifurcationGrid->CellGet(it->first);
            for (const auto& v : cell->OptionsGet())
            {
                it->second.erase(v);
            }

            if(it->second.size() == 0)
            {
                it = mEliminatedValues.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    else
    {
        unsigned short option = mChildNodes.at(nodeIndex)->PivotValueGet();
        progressManager->RegisterProgress(std::make_shared<Progress_OptionRemovedViaGuessing>(mParentGrid->CellGet(mPivot), option));
    }

    mChildNodes[nodeIndex]->Uninit();
    if(mNextNode == mChildNodes.size())
    {
        for (auto& eliminations : mEliminatedValues)
        {
            SudokuCell* cell = mParentGrid->CellGet(eliminations.first);
            progressManager->RegisterProgress(std::make_shared<Progress_ValueDisallowedByBifurcation>(cell, std::move(eliminations.second), mParentGrid->CellGet(mPivot)));
        }
    }
    else
    {
        mChildNodes[mNextNode]->Init();
    }


//    if(mEliminatedValues.size() == 0)
//    {
//        mNextNode == mChildNodes.size();
//    }
}

RandomGuessTreeNode::RandomGuessTreeNode(SudokuGrid* bifurcGrid, CellId pivot, unsigned short pivotValue, RandomGuessTreeRoot* parent, unsigned short index, BifurcationTechnique* technique):
    mIndex(index),
    mGrid(bifurcGrid),
    mParentNode(parent),
    mPivot(pivot),
    mPivotValue(pivotValue),
    mIsNodeValid(true),
    mTechnique(technique)
{
}

void RandomGuessTreeNode::Init()
{
    mGrid->TakeSnapshot();
    mGrid->CellGet(mPivot)->RemoveAllOtherOptions(mPivotValue);
}

void RandomGuessTreeNode::Uninit()
{
    mGrid->RestoreSnapshot();
}

bool RandomGuessTreeNode::IsValidGet() const
{
    return mIsNodeValid;
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
