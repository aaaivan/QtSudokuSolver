#include "mainwindowcontent.h"
#include "mainwindow.h"
#include "adddigitscontrols.h"
#include "drawkillerscontrols.h"
#include "sudokugridwidget.h"
#include "editgridcontrols.h"
#include "drawregionscontrols.h"
#include "solvercontrols.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QStackedLayout>

MainWindowContent::MainWindowContent(unsigned short size, MainWindow *parent)
    : QWidget{parent},
      mMainWindow(parent),
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
    EditGridControls* editMenu = new EditGridControls(this);
    SolverControls* solverMenu = new SolverControls(this);
    tabsLeft->addTab(editMenu, "Create");
    tabsLeft->addTab(solverMenu, "Solver");

    // build context menu stacked layout
    mContextMenu->setStackingMode(QStackedLayout::StackingMode::StackOne);
    contextMenuFrame->setLayout(mContextMenu);
    AddDigitsControls* enterDigitsControls = new AddDigitsControls(this);
    DrawKillersControls* drawKillerControls = new DrawKillersControls(this);
    DrawRegionsControls* regionsEditControls = new DrawRegionsControls(this);

    mContextMenu->insertWidget(ViewType::EnterDigits, enterDigitsControls);
    mContextMenu->insertWidget(ViewType::DrawRegions, regionsEditControls);
    mContextMenu->insertWidget(ViewType::DrawKiller, drawKillerControls);

    mContextMenu->setCurrentIndex(ViewType::EnterDigits);

    // context menu styling
    contextMenuFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    contextMenuFrame->setObjectName("context_menu_frame");
    contextMenuFrame->setStyleSheet("#context_menu_frame{background-color: white;}");

    // focus policy
    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

MainWindow *MainWindowContent::MainWindowGet() const
{
    return mMainWindow;
}

ContextMenuWindow *MainWindowContent::ActiveContextMenuGet() const
{
    return dynamic_cast<ContextMenuWindow*>(mContextMenu->currentWidget());
}

ContextMenuWindow *MainWindowContent::ContextMenuGet(ViewType menu) const
{
    return dynamic_cast<ContextMenuWindow*>(mContextMenu->widget(menu));
}

SudokuGridWidget *MainWindowContent::GridGet() const
{
    return mGrid;
}

MainWindowContent::ViewType MainWindowContent::CurrentViewGet() const
{
    return mCurrentView;
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
