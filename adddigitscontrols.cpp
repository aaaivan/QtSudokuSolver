#include "adddigitscontrols.h"
#include "mainwindowcontent.h"
#include "sudokucellwidget.h"
#include "sudokugridwidget.h"
#include <QLabel>
#include <QKeyEvent>

AddDigitsControls::AddDigitsControls(MainWindowContent* mainWindowContent, QWidget *parent)
    : QWidget{parent},
      ContextMenuWindow(mainWindowContent),
      mGrid(mainWindowContent->GridGet())
{
    QLabel* label = new QLabel("To be implemented", this);
}

void AddDigitsControls::CellClicked(SudokuCellWidget *cell)
{
    return;
}

void AddDigitsControls::KeyboardInput(SudokuCellWidget *cell, QKeyEvent *event)
{
    bool ok;
    int num = event->text().toInt(&ok);

    if(ok && num > 0 && num <= mGrid->SizeGet())
    {
        cell->SetGivenDigit(static_cast<unsigned short>(num));
    }
    else if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        cell->RemoveGivenDigit();
    }
}
