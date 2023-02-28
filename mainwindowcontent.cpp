#include "mainwindowcontent.h"
#include "sudokugridwidget.h"
#include "editgridcontrols.h"
#include "regionseditingcontrols.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QStackedLayout>

MainWindowContent::MainWindowContent(QWidget *parent)
    : QWidget{parent}
{
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    this->setLayout(horizontalLayout);

    SudokuGridWidget* grid = new SudokuGridWidget(9);
    QTabWidget* tabsLeft = new QTabWidget();
    QFrame* contextMenu = new QFrame();

    horizontalLayout->addWidget(tabsLeft);
    horizontalLayout->addWidget(grid);
    horizontalLayout->addWidget(contextMenu);

    QStackedLayout* contextMenuStack = new QStackedLayout();
    contextMenuStack->setStackingMode(QStackedLayout::StackingMode::StackOne);
    contextMenu->setLayout(contextMenuStack);

    EditGridControls* edit = new EditGridControls();
    tabsLeft->addTab(edit, "Create");

    RegionsEditingControls* regionsEditControls = new RegionsEditingControls();
    contextMenuStack->addWidget(regionsEditControls);
}
