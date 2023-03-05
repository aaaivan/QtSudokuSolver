#include "drawkillerscontrols.h"
#include "gridgraphicaloverlay.h"
#include "killercagewidget.h"
#include "sudokucellwidget.h"
#include "sudokugridwidget.h"
#include "variantcluewidget.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QLabel>

DrawKillersControls::DrawKillersControls(MainWindowContent* mainWindowContent, QWidget *parent)
    : QWidget{parent},
      ContextMenuWindow(mainWindowContent),
      mGrid(mainWindowContent->GridGet()),
      mCurrentView(MenuView::MainView),
      mStackedLayout(new QStackedLayout())
{
    // build stacked layout
    this->setLayout(mStackedLayout);
    QWidget* mainView = new QWidget();
    QPushButton* v2 = new QPushButton("Needs implementation");
    mStackedLayout->insertWidget(MenuView::MainView, mainView);
    mStackedLayout->insertWidget(MenuView::EditKiller, v2);
    mStackedLayout->setCurrentIndex(MenuView::MainView);

    // build buttons view
    QVBoxLayout* horizontalLayout = new QVBoxLayout();
    mainView->setLayout(horizontalLayout);
    QLabel* instructions = new QLabel("Select an existing killer cage to edit it or an empty cell to add a new one.");
    QPushButton* clearAllKillerCages = new QPushButton("Clear All Killer Cages");
    horizontalLayout->addWidget(instructions);
    QFrame* hLine = new QFrame();
    hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    horizontalLayout->addWidget(hLine);
    horizontalLayout->addWidget(clearAllKillerCages);
    horizontalLayout->addStretch();

    // events
    connect(clearAllKillerCages, SIGNAL(clicked(bool)), this, SLOT(ClearKillersBtn_Clicked()));
}

void DrawKillersControls::hideEvent(QHideEvent *event)
{
    mGrid->GraphicalOverlayGet()->ClearActiveComponent();
}

void DrawKillersControls::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    SwitchView(MenuView::MainView);
}

void DrawKillersControls::SwitchView(MenuView newView)
{
    if(newView != mCurrentView)
    {
        mCurrentView = newView;
        mStackedLayout->setCurrentIndex(mCurrentView);
    }
}

void DrawKillersControls::CellClickedInMainView(SudokuCellWidget *cell)
{
    bool handled = false;
    for (const auto& clue : cell->VariantCluesGet()) {
        if(clue->ContextMenuGet() == this)
        {
            if(mGrid->GraphicalOverlayGet()->ActiveComponentSet(dynamic_cast<KillerCageWidget*>(clue)))
            {
                SwitchView(MenuView::EditKiller);
            }
            handled = true;
            break;
        }
    }

    if(!handled)
    {
        CreateNewCageFromCell(cell);
    }
}

void DrawKillersControls::CellClickedInEditView(SudokuCellWidget *cell)
{
    KillerCageWidget* activeCage = dynamic_cast<KillerCageWidget*>(mGrid->GraphicalOverlayGet()->ActiveComponentGet());
    if(activeCage)
    {

        KillerCageWidget* selectedCage = nullptr;
        for (const auto& clue : cell->VariantCluesGet()) {
            if(clue->ContextMenuGet() == this)
            {
                selectedCage = dynamic_cast<KillerCageWidget*>(clue);
            }
        }

        if(selectedCage && selectedCage == activeCage)
        {
            activeCage->RemoveCell(cell);
        }
        else if(!selectedCage)
        {
            activeCage->AddCell(cell);
        }
    }
}

void DrawKillersControls::CreateNewCageFromCell(SudokuCellWidget *cell)
{
    KillerCageWidget* killerCageWidget = new KillerCageWidget(mGrid->SizeGet(), mGrid->CellLengthGet(),
                                                              cell, mGrid, this, mGrid->GraphicalOverlayGet());
    mGrid->GraphicalOverlayGet()->AddOverlayComponent(killerCageWidget);
}

void DrawKillersControls::CellClicked(SudokuCellWidget *cell)
{
    switch (mCurrentView)
    {
    case MenuView::MainView:
        CellClickedInMainView(cell);
        break;
    case MenuView::EditKiller:
        CellClickedInEditView(cell);
        break;
    default:
        break;
    }
}

void DrawKillersControls::KeyboardInput(SudokuCellWidget *cell, QKeyEvent *event)
{
    switch (mCurrentView)
    {
    case MenuView::MainView:
        break;
    case MenuView::EditKiller:
        break;
    default:
        break;
    }
}

void DrawKillersControls::ClueAdded(QWidget *clue)
{
    if(mGrid->GraphicalOverlayGet()->ActiveComponentSet(clue))
    {
        SwitchView(MenuView::EditKiller);
    }
}

void DrawKillersControls::ClueRemoved(QWidget *clue)
{
    SwitchView(MenuView::MainView);
}

void DrawKillersControls::ClearKillersBtn_Clicked()
{
    auto btn = static_cast<QMessageBox::StandardButton>(QMessageBox::warning(this, "Clear Killer Cages", "All the killer cages in the grid will be cleared.\nContinue?",
                                                        QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel));

    if(btn == QMessageBox::StandardButton::Ok)
    {
        for(const auto& clue : mGrid->GraphicalOverlayGet()->OverlayComponentsGet())
        {
            if(dynamic_cast<KillerCageWidget*>(clue))
            {
                mGrid->GraphicalOverlayGet()->RemoveOverlayComponent(clue);
            }
        }
    }
}
