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
    connect(this, SIGNAL(clicked(bool)), this, SLOT(OnClicked()));
}

void CellContentButton::keyReleaseEvent(QKeyEvent *event)
{
    QPushButton::keyReleaseEvent(event);
    mCell->MainWindowContentGet()->ActiveContextMenuGet()->KeyboardInput(mCell, event);
}

void CellContentButton::OnClicked()
{
    mCell->MainWindowContentGet()->ActiveContextMenuGet()->CellClicked(mCell);
}
