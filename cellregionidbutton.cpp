#include "cellregionidbutton.h"
#include "sudokucellwidget.h"
#include <QKeyEvent>

CellRegionIdButton::CellRegionIdButton(unsigned short numOptions, SudokuCellWidget *parent)
    : QPushButton{parent},
      mNumRegions(numOptions),
      mCell(parent)
{
}

void CellRegionIdButton::keyReleaseEvent(QKeyEvent *event)
{
    QPushButton::keyReleaseEvent(event);
    bool ok;
    int num = event->text().toInt(&ok);

    if(ok && num >= 0 && num <= mNumRegions)
    {
        mCell->UpdateRegionId(num);
    }
    else if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        mCell->ResetRegionId();
    }
}
