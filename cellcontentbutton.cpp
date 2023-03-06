#include "cellcontentbutton.h"
#include "contextmenuwindow.h"
#include "sudokucellwidget.h"
#include "mainwindowcontent.h"
#include <QKeyEvent>

CellContentButton::CellContentButton(unsigned short numOptions, SudokuCellWidget *parent)
    : QPushButton{parent},
      mNumOptions(numOptions),
      mCell(parent)
{
    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

void CellContentButton::keyReleaseEvent(QKeyEvent *event)
{
    QPushButton::keyReleaseEvent(event);
    mCell->MainWindowContentGet()->ActiveContextMenuGet()->KeyboardInput(mCell, event);
}

void CellContentButton::focusInEvent(QFocusEvent *event)
{
    QPushButton::focusInEvent(event);
    mCell->MainWindowContentGet()->ActiveContextMenuGet()->CellGainedFocus(mCell);
}

void CellContentButton::focusOutEvent(QFocusEvent *event)
{
    QPushButton::focusOutEvent(event);
    mCell->MainWindowContentGet()->ActiveContextMenuGet()->CellLostFocus(mCell);
}
