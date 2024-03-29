#ifndef CONTEXTMENUWINDOW_H
#define CONTEXTMENUWINDOW_H

class SudokuCellWidget;
class MainWindowContent;
class QKeyEvent;
class QWidget;

class ContextMenuWindow
{
protected:
    MainWindowContent* mMainWindowContent;

public:
    ContextMenuWindow(MainWindowContent* mainWindowContent);

    virtual void CellGainedFocus(SudokuCellWidget* cell) = 0;
    virtual void CellLostFocus(SudokuCellWidget* cell) = 0;
    virtual void CellClicked(SudokuCellWidget* cell) = 0;
    virtual void KeyboardInput(SudokuCellWidget* cell, QKeyEvent* event) = 0;
    virtual void ClueAdded(QWidget* clue);
    virtual void ClueRemoved(QWidget* clue);
    virtual void ClueDidGetActive(QWidget* clue);
    virtual void ClueDidGetInactive(QWidget* clue, bool willBeDeleted);
};

#endif // CONTEXTMENUWINDOW_H
