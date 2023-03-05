#ifndef ADDDIGITSCONTROLS_H
#define ADDDIGITSCONTROLS_H

#include "contextmenuwindow.h"
#include <QWidget>

class SudokuGridWidget;

class AddDigitsControls : public QWidget, public ContextMenuWindow
{
    Q_OBJECT
public:
    explicit AddDigitsControls(MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

    void CellClicked(SudokuCellWidget* cell) override;
    void KeyboardInput(SudokuCellWidget* cell, QKeyEvent* event) override;

private:
    SudokuGridWidget* mGrid;
};

#endif // ADDDIGITSCONTROLS_H
