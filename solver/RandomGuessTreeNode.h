#ifndef RANDOMGUESSTREENODE_H
#define RANDOMGUESSTREENODE_H

#include "Types.h"

class RandomGuessTreeRoot;
class RandomGuessTreeNode;
class BifurcationTechnique;

typedef std::vector<std::map<unsigned short, std::map<size_t, std::set<unsigned short>>>> EliminationMatrix;

class RandomGuessTreeRoot
{
    SudokuGrid* const mGrid;
    SudokuCell* const mPivot;
    unsigned short mNextNode;
    std::vector<std::unique_ptr<RandomGuessTreeNode>> mChildNodes;
    std::set<unsigned int> mHotCells;
    bool mFinished;

    BifurcationTechnique* mTechnique;

public:
    RandomGuessTreeRoot(SudokuGrid* grid, SudokuCell* pivot, BifurcationTechnique* technique);

    bool HasFinished() const;
    void NextStep();
    void NodeHasFinished(unsigned short nodeIndex);
};

class RandomGuessTreeNode
{
    const unsigned short mIndex;
    std::unique_ptr<SudokuGrid> mGrid;
    const SudokuGrid* mRootGrid;
    RandomGuessTreeRoot* const mParentNode;
    SudokuCell* mPivotCell;
    unsigned short mPivotValue;
    bool mIsNodeValid;

    BifurcationTechnique* mTechnique;
public:
    RandomGuessTreeNode(const SudokuGrid* grid, SudokuCell* pivotCell, unsigned short pivotValue, RandomGuessTreeRoot* parent, unsigned short index, BifurcationTechnique* technique);

    bool IsValidGet() const;
    SudokuGrid* GridGet() const;
    unsigned short PivotValueGet() const;
    void NextStep();

private:
    void UpdateOptionEliminationMatrix() const;
};

#endif // RANDOMGUESSTREENODE_H
