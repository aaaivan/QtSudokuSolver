#include "cellcontentbutton.h"
#include "sudokucellwidget.h"
#include <QKeyEvent>
#include <QDebug>

CellContentButton::CellContentButton(unsigned short numOptions, SudokuCellWidget *parent)
    : QPushButton{parent},
      mNumOptions(numOptions),
      mCell(parent)
{
}

void CellContentButton::keyReleaseEvent(QKeyEvent *event)
{
    QPushButton::keyReleaseEvent(event);
    bool ok;
    int num = event->text().toInt(&ok);

    if(ok && num > 0 && num <= mNumOptions)
    {
        mCell->SetGivenDigit(static_cast<unsigned short>(num));
    }
    else if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        mCell->RemoveGivenDigit();
    }
}
