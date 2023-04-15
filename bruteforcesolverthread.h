#ifndef BRUTEFORCESOLVERTHREAD_H
#define BRUTEFORCESOLVERTHREAD_H

#include <QThread>
#include <QMutex>
#include "solver/SudokuGrid.h"
#include "solver/BruteForceSolver.h"

class BruteForceSolverThread : public QThread
{
    Q_OBJECT
public:
    explicit BruteForceSolverThread(QObject *parent = nullptr);
    ~BruteForceSolverThread();

    void Init(SudokuGrid* grid, QMutex* solverMutex);

signals:
    void NumberOfSolutionsComputed(size_t count);

protected:
    void run() override;

private:
    std::unique_ptr<BruteForceSolver> mBruteForceSolver;

    size_t mMaxSolutionsCount;
    bool mUseHints;
    bool mAbort;
    QMutex mInputMutex;
    QMutex* mSolverMutex;

public:
    void CountSolutions(size_t maxSolutionCount, bool useHints);
    void AbortCalculation();
    void NotifyGridChanged();
};

#endif // BRUTEFORCESOLVERTHREAD_H
