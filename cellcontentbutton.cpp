#include "cellcontentbutton.h"
#include "sudokucellwidget.h"
#include "contextmenuwindow.h"
#include "mainwindowcontent.h"
#include <QKeyEvent>

CellContentButton::CellContentButton(unsigned short numOptions, SudokuCellWidget *parent)
    : QLabel{parent},
      mNumOptions(numOptions),
      mCell(parent)
{
    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    this->setTextFormat(Qt::RichText);
    this->setAlignment(Qt::AlignCenter);
    this->setWordWrap(true);
    this->setContentsMargins(5, 5, 5, 5);
}

void CellContentButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    if(this->hasFocus())
    {
        mCell->MainWindowContentGet()->ActiveContextMenuGet()->CellClicked(mCell);
    }
}

void CellContentButton::keyReleaseEvent(QKeyEvent *event)
{
    QLabel::keyReleaseEvent(event);
    mCell->MainWindowContentGet()->ActiveContextMenuGet()->KeyboardInput(mCell, event);
}

void CellContentButton::focusInEvent(QFocusEvent *event)
{
    QLabel::focusInEvent(event);
    mCell->MainWindowContentGet()->ActiveContextMenuGet()->CellGainedFocus(mCell);
}

void CellContentButton::focusOutEvent(QFocusEvent *event)
{
    QLabel::focusOutEvent(event);
    mCell->MainWindowContentGet()->ActiveContextMenuGet()->CellLostFocus(mCell);
}
