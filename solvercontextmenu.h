#ifndef SOLVERCONTEXTMENU_H
#define SOLVERCONTEXTMENU_H

#include "contextmenuwindow.h"
#include <QWidget>

class SolverContextMenu : public QWidget, public ContextMenuWindow
{
    Q_OBJECT
public:
    explicit SolverContextMenu(MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private:
    void hideEvent(QHideEvent* event) override;
    void showEvent(QShowEvent* event) override;

public:
    void CellGainedFocus(SudokuCellWidget* cell) override;
    void CellLostFocus(SudokuCellWidget* cell) override;
    void CellClicked(SudokuCellWidget* cell) override;
    void KeyboardInput(SudokuCellWidget* cell, QKeyEvent* event) override;
};

#endif // SOLVERCONTEXTMENU_H
