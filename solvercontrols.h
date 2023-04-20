#ifndef SOLVERCONTROLS_H
#define SOLVERCONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>

class BruteForceSolverThread;
class MainWindowContent;
class SudokuSolverThread;

class SolverControls : public QWidget
{
    Q_OBJECT
public:
    explicit SolverControls(BruteForceSolverThread* bruteForceSolver, MainWindowContent* mainWindow, QWidget *parent = nullptr);

private:
    QPushButton* mCountSolutionsBtn;
    QPushButton* mBruteForceSolveBtn;
    QSpinBox* mMaxSolutionsCount;
    QCheckBox* mUseHintsCheckbox;
    QPushButton* mAbortCalculationsBtn;
    QPushButton* mClearGridBtn;
    QPushButton* mLogicalStepBtn;

    MainWindowContent* mMainWindow;
    BruteForceSolverThread* mBruteForceSolver;
    SudokuSolverThread* mSolverThread;

private slots:
    void CountSolutionsBtn_Clicked();
    void DisplaySolutionsBtn_Clicked();
    void ClearGridBtn_Clicked();
    void AbortButton_Clicked();
    void LogicalStepBtn_Clicked();
    void CalculationStarted();
    void CalculationFinished();
};

#endif // SOLVERCONTROLS_H
