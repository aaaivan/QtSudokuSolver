#include "drawkillerscontextmenu.h"
#include "sudokucellwidget.h"
#include "killercagewidget.h"
#include "variantclueslayer.h"
#include "sudokugridwidget.h"
#include "mainwindowcontent.h"
#include "sudokusolverthread.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QLabel>
#include <QFormLayout>
#include <QSpinBox>
#include <QStackedLayout>

DrawKillersContextMenu::DrawKillersContextMenu(MainWindowContent* mainWindowContent, const PuzzleData* loadedGrid, QWidget *parent)
    : QWidget{parent},
      ContextMenuWindow(mainWindowContent),
      mGrid(mainWindowContent->GridGet()),
      mCurrentView(MenuView::MainView),
      mStackedLayout(new QStackedLayout()),
      mCageTotal(new QSpinBox()),
      mIdOfKillerBeingModified(-1)
{
    // build stacked layout
    this->setLayout(mStackedLayout);
    QWidget* mainView = new QWidget();
    QWidget* editView = new QWidget();
    mStackedLayout->insertWidget(MenuView::MainView, mainView);
    mStackedLayout->insertWidget(MenuView::EditKiller, editView);
    mStackedLayout->setCurrentIndex(MenuView::MainView);

    // build main view
    {
        QVBoxLayout* verticalLayout = new QVBoxLayout();
        mainView->setLayout(verticalLayout);
        QLabel* instructions = new QLabel("Select an existing killer cage to edit it\n"
                                          "or an empty cell to add a new one.");
        QPushButton* clearAllKillerCages = new QPushButton("Delete All");
        verticalLayout->addWidget(instructions);
        QFrame* hLine = new QFrame();
        hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
        verticalLayout->addWidget(hLine);
        verticalLayout->addWidget(clearAllKillerCages);
        verticalLayout->addStretch();

        // events
        connect(clearAllKillerCages, SIGNAL(clicked(bool)), this, SLOT(DeleteAllKillersBtn_Clicked()));
    }

    // build edit view
    {
        QVBoxLayout* verticalLayout = new QVBoxLayout();
        editView->setLayout(verticalLayout);
        QWidget* form = new QWidget();
        QFrame* hLine = new QFrame();
        hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
        QWidget* buttons = new QWidget();
        verticalLayout->addWidget(form);
        verticalLayout->addWidget(hLine);
        verticalLayout->addWidget(buttons);
        verticalLayout->addStretch();

        QFormLayout* formLayout = new QFormLayout();
        form->setLayout(formLayout);
        QLabel* label = new QLabel("Cage Total");
        formLayout->addRow(label, mCageTotal);

        QHBoxLayout* horizontalLayout = new QHBoxLayout();
        buttons->setLayout(horizontalLayout);
        QPushButton* doneBtn = new QPushButton("Done");
        QPushButton* deleteBtn = new QPushButton("Delete");
        horizontalLayout->addWidget(doneBtn);
        horizontalLayout->addWidget(deleteBtn);

        // events
        connect(mCageTotal, SIGNAL(valueChanged(int)), this, SLOT(CageTotal_ValueChanged(int)));
        connect(doneBtn, SIGNAL(clicked(bool)), this, SLOT(EditingDoneBtn_Clicked()));
        connect(deleteBtn, SIGNAL(clicked(bool)), this, SLOT(DeleteActiveKillerBtn_Clicked()));
    }

    // load puzzle
    if(loadedGrid)
    {
        for (const auto& k : loadedGrid->mKillerCages)
        {
            SudokuCellWidget* firstCell = mGrid->CellGet(k.first);
            KillerCageWidget* killerCageWidget = new KillerCageWidget(mGrid->SizeGet(), mGrid->CellLengthGet(), firstCell,
                                                                      1, mGrid, this, mGrid->VariantCluesLayerGet());
            for (const auto& cId : k.second.second)
            {
                SudokuCellWidget* c = mGrid->CellGet(cId);
                killerCageWidget->AddDisconnectedCell(c);
            }
            killerCageWidget->CageTotalSet(k.second.first);
            mGrid->VariantCluesLayerGet()->AddVariantClue(killerCageWidget, false);
            mGrid->SolverGet()->AddKillerCage(killerCageWidget->CageIdGet(), killerCageWidget->CageTotalGet(), k.second.second);
        }
        mGrid->SolverGet()->SubmitChangesToSolver();
    }
}

void DrawKillersContextMenu::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    mGrid->VariantCluesLayerGet()->ClearActiveClue();
}

void DrawKillersContextMenu::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    SwitchView(MenuView::MainView);
}

void DrawKillersContextMenu::UpdateCageTotalLimits(KillerCageWidget *activeCage)
{
    if(activeCage)
    {
        int n = activeCage->SizeGet();
        int s = mGrid->SizeGet();
        int minSum = n * (n + 1) / 2;
        int maxSum = n * (s + 1) - minSum;
        mCageTotal->setRange(minSum, maxSum);
    }
    else
    {
        mCageTotal->setRange(0, 0);
    }
}

void DrawKillersContextMenu::SwitchView(MenuView newView)
{
    if(newView != mCurrentView)
    {
        mCurrentView = newView;
        mStackedLayout->setCurrentIndex(mCurrentView);
    }
}

void DrawKillersContextMenu::CellClickedInMainView(SudokuCellWidget *cell)
{
    bool handled = false;
    for (const auto& clue : cell->VariantCluesGet()) {
        if(clue->ContextMenuGet() == this)
        {
            if(mGrid->VariantCluesLayerGet()->ActiveClueSet(dynamic_cast<KillerCageWidget*>(clue)))
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

void DrawKillersContextMenu::CellClickedInEditView(SudokuCellWidget *cell)
{
    KillerCageWidget* activeCage = dynamic_cast<KillerCageWidget*>(mGrid->VariantCluesLayerGet()->ActiveClueGet());
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

        KillerCageWidget* activeCage = dynamic_cast<KillerCageWidget*>(mGrid->VariantCluesLayerGet()->ActiveClueGet());
        UpdateCageTotalLimits(activeCage);
    }
}

void DrawKillersContextMenu::CreateNewCageFromCell(SudokuCellWidget *cell)
{
    KillerCageWidget* killerCageWidget = new KillerCageWidget(mGrid->SizeGet(), mGrid->CellLengthGet(), cell,
                                                              1, mGrid, this, mGrid->VariantCluesLayerGet());
    mGrid->VariantCluesLayerGet()->AddVariantClue(killerCageWidget, true);
}

void DrawKillersContextMenu::CellGainedFocus(SudokuCellWidget *cell)
{
    Q_UNUSED(cell)
}

void DrawKillersContextMenu::CellLostFocus(SudokuCellWidget *cell)
{
    Q_UNUSED(cell)
}

void DrawKillersContextMenu::CellClicked(SudokuCellWidget *cell)
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

void DrawKillersContextMenu::KeyboardInput(SudokuCellWidget *cell, QKeyEvent *event)
{
    Q_UNUSED(cell)
    Q_UNUSED(event)
}

void DrawKillersContextMenu::ClueDidGetActive(QWidget *clue)
{
    SwitchView(MenuView::EditKiller);
    KillerCageWidget* cage = dynamic_cast<KillerCageWidget*>(clue);
    unsigned int cageTotal = cage->CageTotalGet();
    UpdateCageTotalLimits(cage);
    if(cage)
    {
        mCageTotal->setValue(cageTotal);
        CellCoord killerId = cage->CageIdGet();
        mIdOfKillerBeingModified = mGrid->SolverGet()->HasKillerCage(killerId) ? killerId : -1;
    }
}

void DrawKillersContextMenu::ClueDidGetInactive(QWidget *clue, bool willBeDeleted)
{
    mCageTotal->setRange(0, 0);
    SwitchView(MenuView::MainView);

    KillerCageWidget* cage = dynamic_cast<KillerCageWidget*>(clue);
    CellCoord killerId = mIdOfKillerBeingModified;
    mIdOfKillerBeingModified = -1;
    if(!cage)
    {
        return;
    }

    bool newKiller = (killerId == CellCoord(-1));
    if(newKiller && willBeDeleted)
    {
        return;
    }
    else if(willBeDeleted)
    {
        mGrid->SolverGet()->RemoveKillerCage(killerId);
        mGrid->SolverGet()->SubmitChangesToSolver();
        return;
    }

    CellsInRegion cells;
    std::transform(cage->CellsGet().begin(),
                   cage->CellsGet().end(),
                   std::inserter(cells, cells.begin()),
                   [&](SudokuCellWidget* const &c){ return c->CellIdGet(); });
    bool wasModified = true;
    if(!newKiller)
    {
        wasModified = !mGrid->SolverGet()->IsSameKillerCage(killerId, cage->CageTotalGet(), cells);
    }

    if(newKiller && cells.size() > 0)
    {
        mGrid->SolverGet()->AddKillerCage(cage->CageIdGet(), cage->CageTotalGet(), cells);
        mGrid->SolverGet()->SubmitChangesToSolver();
    }
    else if(!newKiller && wasModified)
    {
        mGrid->SolverGet()->RemoveKillerCage(killerId);
        if(cells.size() > 0)
        {
            mGrid->SolverGet()->AddKillerCage(cage->CageIdGet(), cage->CageTotalGet(), cells);
        }
        mGrid->SolverGet()->SubmitChangesToSolver();
    }
}

void DrawKillersContextMenu::DeleteAllKillersBtn_Clicked()
{
    auto btn = static_cast<QMessageBox::StandardButton>(QMessageBox::warning(this, "Delete All Killer Cages", "All the killer cages in the grid will be deleted.\nContinue?",
                                                        QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel));

    if(btn == QMessageBox::StandardButton::Ok)
    {
        QVector<KillerCageWidget*> killerCages;
        for(const auto& clue : mGrid->VariantCluesLayerGet()->VariantCluesGet())
        {
            if(KillerCageWidget* cage = dynamic_cast<KillerCageWidget*>(clue); cage)
            {
                killerCages.push_back(cage);
            }
        }
        for(const auto& cage : killerCages)
        {
            mGrid->VariantCluesLayerGet()->RemoveVariantClue(cage);
            mGrid->SolverGet()->RemoveKillerCage(cage->CageIdGet());
        }
        if(killerCages.size() > 0)
        {
            mGrid->SolverGet()->SubmitChangesToSolver();
        }
    }
}

void DrawKillersContextMenu::EditingDoneBtn_Clicked()
{
    mGrid->VariantCluesLayerGet()->ClearActiveClue();
    SwitchView(MenuView::MainView);
}

void DrawKillersContextMenu::DeleteActiveKillerBtn_Clicked()
{
    auto btn = static_cast<QMessageBox::StandardButton>(QMessageBox::warning(this, "Delete Killer Cage", "The selected killer cage will be deleted.\nContinue?",
                                                        QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel));

    if(btn == QMessageBox::StandardButton::Ok)
    {
        KillerCageWidget* cage = dynamic_cast<KillerCageWidget*>(mGrid->VariantCluesLayerGet()->ActiveClueGet());
        mGrid->VariantCluesLayerGet()->RemoveVariantClue(cage);
    }
}

void DrawKillersContextMenu::CageTotal_ValueChanged(int value)
{
    KillerCageWidget* cage = dynamic_cast<KillerCageWidget*>(mGrid->VariantCluesLayerGet()->ActiveClueGet());
    if(cage && mMainWindowContent->ActiveContextMenuGet() == this)
    {
        cage->CageTotalSet(value);
    }
}
