#ifndef SOLVERCONTEXTMENU_H
#define SOLVERCONTEXTMENU_H

#include "contextmenuwindow.h"
#include <QWidget>
#include <QPlainTextEdit>
#include <QLabel>

class SudokuSolverThread;
class BruteForceSolverThread;

class SolverContextMenu : public QWidget, public ContextMenuWindow
{
    Q_OBJECT
public:
    explicit SolverContextMenu(MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private:
    SudokuSolverThread* mLogicalSolver;
    BruteForceSolverThread* mBruteForceSolver;

    QLabel* mStatusLabel;
    QPlainTextEdit* mSolverOutput;
    bool mFirstMessage;

    void hideEvent(QHideEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:
    void OnCalculationStarted();
    void OnCalculationFinished();
    void OnSolutionsCounted(size_t count, bool stopped);
    void OnNewSolverMessage(QString message);
    void OnSolverReset();
    void SolverOutput_TextChanged();

public:
    void CellGainedFocus(SudokuCellWidget* cell) override;
    void CellLostFocus(SudokuCellWidget* cell) override;
    void CellClicked(SudokuCellWidget* cell) override;
    void KeyboardInput(SudokuCellWidget* cell, QKeyEvent* event) override;
};

#endif // SOLVERCONTEXTMENU_H
