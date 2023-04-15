#ifndef SOLVERCONTROLS_H
#define SOLVERCONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>

class BruteForceSolverThread;

class SolverControls : public QWidget
{
    Q_OBJECT
public:
    explicit SolverControls(BruteForceSolverThread* bruteForceSolver, QWidget *parent = nullptr);

private:
    QPushButton* mCountSolutionsBtn;
    QPushButton* mBruteForceSolveBtn;
    QSpinBox* mMaxSolutionsCount;
    QCheckBox* mUseHintsCheckbox;
    QPushButton* mAbortCalculationsBtn;

    BruteForceSolverThread* mBruteForceSolver;

private slots:
    void CountSolutionsBtn_Clicked();
    void DisplaySolutionsBtn_Clicked();
    void AbortButton_Clicked();
    void CalculationStarted();
    void CalculationFinished();
};

#endif // SOLVERCONTROLS_H
