#include "solvercontextmenu.h"
#include "mainwindowcontent.h"
#include "sudokugridwidget.h"

SolverContextMenu::SolverContextMenu(MainWindowContent* mainWindowContent, QWidget *parent)
    : QWidget{parent}
    , ContextMenuWindow(mainWindowContent)
{

}

void SolverContextMenu::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    mMainWindowContent->GridGet()->SolverGet()->BruteSolverGet()->AbortCalculation();
    mMainWindowContent->GridGet()->SolverGet()->SetLogicalSolverPaused(false);
}

void SolverContextMenu::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    mMainWindowContent->GridGet()->SolverGet()->SetLogicalSolverPaused(true);
}

void SolverContextMenu::CellGainedFocus(SudokuCellWidget *cell)
{

}

void SolverContextMenu::CellLostFocus(SudokuCellWidget *cell)
{

}

void SolverContextMenu::CellClicked(SudokuCellWidget *cell)
{

}

void SolverContextMenu::KeyboardInput(SudokuCellWidget *cell, QKeyEvent *event)
{

}
