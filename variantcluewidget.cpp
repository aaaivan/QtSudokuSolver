#include "variantcluewidget.h"

VariantClueWidget::VariantClueWidget(int cellLength, SudokuGridWidget *grid, ContextMenuWindow* contextMenu):
    mGrid(grid),
    mContextMenu(contextMenu),
    mCellLength(cellLength),
    mCells()
{
}

int VariantClueWidget::SizeGet() const
{
    return mCells.size();
}

void VariantClueWidget::ClueDidGetActive()
{
    return;
}

void VariantClueWidget::ClueDidGetInactive()
{
    return;
}

ContextMenuWindow *VariantClueWidget::ContextMenuGet() const
{
    return mContextMenu;
}
