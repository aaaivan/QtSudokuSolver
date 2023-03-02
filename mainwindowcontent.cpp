#include "mainwindowcontent.h"
#include "sudokugridwidget.h"
#include "editgridcontrols.h"
#include "drawregionscontrols.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QTabWidget>

MainWindowContent::MainWindowContent(unsigned short size, QWidget *parent)
    : QWidget{parent},
      mContextMenu(new QStackedLayout()),
      mGrid(new SudokuGridWidget(size, this)),
      mCurrentView(ViewType::EnterDigits)
{
    // build widget layout
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    this->setLayout(horizontalLayout);
    QTabWidget* tabsLeft = new QTabWidget();
    QFrame* contextMenuFrame = new QFrame();
    horizontalLayout->addWidget(tabsLeft);
    horizontalLayout->addWidget(mGrid);
    horizontalLayout->addWidget(contextMenuFrame);

    // build left-hand side tabs
    EditGridControls* edit = new EditGridControls(this);
    tabsLeft->addTab(edit, "Create");

    // build context menu stacked layout
    mContextMenu->setStackingMode(QStackedLayout::StackingMode::StackOne);
    contextMenuFrame->setLayout(mContextMenu);
    QWidget* emptyView = new QLabel("To Be Implemented...");
    QWidget* emptyView2 = new QLabel("To Be Implemented...");
    DrawRegionsControls* regionsEditControls = new DrawRegionsControls(mGrid);
    mContextMenu->insertWidget(ViewType::EnterDigits, emptyView);
    mContextMenu->insertWidget(ViewType::DrawRegions, regionsEditControls);
    mContextMenu->insertWidget(ViewType::DrawKiller, emptyView2);
    mContextMenu->setCurrentIndex(ViewType::EnterDigits);

    // context menu styling
    contextMenuFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    contextMenuFrame->setObjectName("context_menu_frame");
    contextMenuFrame->setStyleSheet("#context_menu_frame{background-color: white;}");
}

unsigned short MainWindowContent::SelectedRegionIdGet() const
{
    return dynamic_cast<DrawRegionsControls*>(mContextMenu->widget(ViewType::DrawRegions))->SelectedRegionIdGet();
}

void MainWindowContent::ChangeView(ViewType view)
{
    if(view != mCurrentView)
    {
        mCurrentView = view;
        mContextMenu->setCurrentIndex(view);
        mGrid->SwitchView(view);
    }
}
