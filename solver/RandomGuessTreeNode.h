#ifndef RANDOMGUESSTREENODE_H
#define RANDOMGUESSTREENODE_H

#include "Types.h"

class RandomGuessTreeRoot;
class RandomGuessTreeNode;
class BifurcationTechnique;

typedef unsigned int CellId;
typedef std::map<CellId, std::set<unsigned short>> Elimination_t;
typedef std::map<CellId, std::map<unsigned short, Elimination_t>> EliminationMatrix;

class RandomGuessTreeRoot
{
    SudokuGrid* mParentGrid;
    SudokuGrid* mBifurcationGrid;
    CellId mPivot;
    unsigned short mNextNode;
    std::vector<std::unique_ptr<RandomGuessTreeNode>> mChildNodes;
    std::map<CellId, std::set<unsigned short>> mEliminatedValues;
    bool mFinished;

    BifurcationTechnique* mTechnique;

public:
    RandomGuessTreeRoot(SudokuGrid* parentGrid, SudokuGrid* bifurcationGrid, CellId pivot, BifurcationTechnique* technique);

    bool HasFinished() const;
    void NextStep();
    void NodeHasFinished(unsigned short nodeIndex);
};

class RandomGuessTreeNode
{
    const unsigned short mIndex;
    SudokuGrid* mGrid;
    RandomGuessTreeRoot* const mParentNode;
    CellId mPivot;
    unsigned short mPivotValue;
    bool mIsNodeValid;

    BifurcationTechnique* mTechnique;
public:
    RandomGuessTreeNode(SudokuGrid* bifurcGrid, CellId pivot, unsigned short pivotValue, RandomGuessTreeRoot* parent, unsigned short index, BifurcationTechnique* technique);

    void Init();
    void Uninit();
    bool IsValidGet() const;
    unsigned short PivotValueGet() const;
    void NextStep();
};

#endif // RANDOMGUESSTREENODE_H
