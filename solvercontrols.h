#ifndef SOLVERCONTROLS_H
#define SOLVERCONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>

class MainWindowContent;

class SolverControls : public QWidget
{
    Q_OBJECT
public:
    explicit SolverControls(MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private:
    QPushButton* mCountSolutionsButton;
    QPushButton* mBruteForceSolve;
    QCheckBox* mUseHintsCheckbox;

    MainWindowContent* mMainWindowContent;

private slots:
    void CountSolutionsBtn_Clicked();
};

#endif // SOLVERCONTROLS_H
