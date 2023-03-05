#ifndef VARIANTCLUEWIDGET_H
#define VARIANTCLUEWIDGET_H

#include <QList>

class SudokuGridWidget;
class SudokuCellWidget;
class ContextMenuWindow;

class VariantClueWidget
{
public:
    VariantClueWidget(int cellLength, SudokuGridWidget* grid, ContextMenuWindow* contextMenu);

protected:
    SudokuGridWidget* mGrid;
    ContextMenuWindow* mContextMenu;
    int mCellLength;
    QList<SudokuCellWidget*> mCells;

public:
    virtual void AddCell(SudokuCellWidget* cell) = 0;
    virtual void RemoveCell(SudokuCellWidget* cell) = 0;
    virtual void ClueDidGetActive();
    virtual void ClueDidGetInactive();
    ContextMenuWindow* ContextMenuGet() const;
};

#endif // VARIANTCLUEWIDGET_H
