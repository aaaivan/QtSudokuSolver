#ifndef SOLVERCONTROLS_H
#define SOLVERCONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>

class BruteForceSolverThread;

class SolverControls : public QWidget
{
    Q_OBJECT
public:
    explicit SolverControls(BruteForceSolverThread* bruteForceSolver, QWidget *parent = nullptr);

private:
    QPushButton* mCountSolutionsBtn;
    QPushButton* mBruteForceSolveBtn;
    QCheckBox* mUseHintsCheckbox;
    QPushButton* mAbortCalculationsBtn;

    BruteForceSolverThread* mBruteForceSolver;

private slots:
    void CountSolutionsBtn_Clicked();
    void DisplaySolutionsBtn_Clicked();
    void AbortButton_Clicked();
    void CalculationFinished();
};

#endif // SOLVERCONTROLS_H
