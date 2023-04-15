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
    void CalculationStarted();
    void CalculationFinished();
    void NumberOfSolutionsComputed(size_t count, bool stopped);
    void CellUpdated(unsigned int id, unsigned short value);

protected:
    void run() override;

private:
    std::unique_ptr<BruteForceSolver> mBruteForceSolver;
    SudokuGrid* mGrid;

    size_t mMaxSolutionsCount;
    bool mUseHints;
    bool mDisplaySolution;
    bool mAbort;
    QMutex mInputMutex;
    QMutex* mSolverMutex;

public:
    void CountSolutions(size_t maxSolutionCount, bool useHints);
    void DisplaySolution(size_t maxSolutionCount, bool useHints);
    void AbortCalculation();
    void NotifyGridChanged();

    void NotifySolutionsCountReady(size_t count, bool stopped);
    void NotifySolutionReady(const std::vector<unsigned short>& solution);
    void DisplayCandidatesForCell(CellId id);
    void ResetGridContents();
};

#endif // BRUTEFORCESOLVERTHREAD_H
