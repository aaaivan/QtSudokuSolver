#include "mainwindowcontent.h"
#include "mainwindow.h"
#include "adddigitscontrols.h"
#include "drawkillerscontrols.h"
#include "sudokugridwidget.h"
#include "editgridcontrols.h"
#include "drawregionscontrols.h"
#include "solvercontrols.h"
#include "solvercontextmenu.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QStackedLayout>

MainWindowContent::MainWindowContent(unsigned short size, MainWindow *parent, std::unique_ptr<PuzzleData> loadedGrid)
    : QWidget{parent},
      mMainWindow(parent),
      mControlsMenu(new QTabWidget()),
      mContextMenu(new QStackedLayout()),
      mGrid(new SudokuGridWidget(size, this)),
      mCurrentView(ContextMenuType::EnterDigits_Context)
{
    // build widget layout
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    this->setLayout(horizontalLayout);
    QFrame* contextMenuFrame = new QFrame();
    horizontalLayout->addWidget(mControlsMenu);
    horizontalLayout->addWidget(mGrid);
    horizontalLayout->addWidget(contextMenuFrame);

    // build left-hand side tabs
    EditGridControls* editMenu = new EditGridControls(this, loadedGrid.get());
    SolverControls* solverMenu = new SolverControls(mGrid->SolverGet()->BruteSolverGet(), this);
    mControlsMenu->addTab(editMenu, "Create");
    mControlsMenu->addTab(solverMenu, "Solver");

    // build context menu stacked layout
    mContextMenu->setStackingMode(QStackedLayout::StackingMode::StackOne);
    contextMenuFrame->setLayout(mContextMenu);
    AddDigitsControls* enterDigitsControls = new AddDigitsControls(this, loadedGrid.get());
    DrawKillersControls* drawKillerControls = new DrawKillersControls(this, loadedGrid.get());
    DrawRegionsControls* regionsEditControls = new DrawRegionsControls(this, loadedGrid.get());
    SolverContextMenu* solverContextMenu = new SolverContextMenu(this);

    mContextMenu->insertWidget(ContextMenuType::EnterDigits_Context, enterDigitsControls);
    mContextMenu->insertWidget(ContextMenuType::DrawRegions_Context, regionsEditControls);
    mContextMenu->insertWidget(ContextMenuType::DrawKiller_Context, drawKillerControls);
    mContextMenu->insertWidget(ContextMenuType::Solver_Context, solverContextMenu);

    mContextMenu->setCurrentIndex(ContextMenuType::EnterDigits_Context);

    // context menu styling
    contextMenuFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    contextMenuFrame->setObjectName("context_menu_frame");
    contextMenuFrame->setStyleSheet("#context_menu_frame{background-color: white;}");

    // focus policy
    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    // events
    connect(mControlsMenu, SIGNAL(currentChanged(int)), this, SLOT(OnTabChanged_ControlsMenu(int)));
}

void MainWindowContent::OnTabChanged_ControlsMenu(int tab)
{
    if(tab == static_cast<int>(ControlsMenuType::CreateTab))
    {
        int selectedBtn = static_cast<EditGridControls*>(mControlsMenu->widget(ControlsMenuType::CreateTab))->SelectedButtonIdGet();
        ChangeView(static_cast<ContextMenuType>(selectedBtn));
    }
    else if(tab == static_cast<int>(ControlsMenuType::SolverTab))
    {
        ChangeView(ContextMenuType::Solver_Context);
    }
}

MainWindow *MainWindowContent::MainWindowGet() const
{
    return mMainWindow;
}

ContextMenuWindow *MainWindowContent::ActiveContextMenuGet() const
{
    return dynamic_cast<ContextMenuWindow*>(mContextMenu->currentWidget());
}

ContextMenuWindow *MainWindowContent::ContextMenuGet(ContextMenuType menu) const
{
    return dynamic_cast<ContextMenuWindow*>(mContextMenu->widget(menu));
}

SudokuGridWidget *MainWindowContent::GridGet() const
{
    return mGrid;
}

MainWindowContent::ContextMenuType MainWindowContent::CurrentViewGet() const
{
    return mCurrentView;
}

void MainWindowContent::ChangeView(ContextMenuType view)
{
    if(view != mCurrentView)
    {
        mCurrentView = view;
        mContextMenu->setCurrentIndex(view);
        mGrid->SwitchView(view);
    }
}
