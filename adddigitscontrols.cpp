#include "adddigitscontrols.h"
#include "mainwindowcontent.h"
#include "puzzledata.h"
#include "qapplication.h"
#include "sudokucellwidget.h"
#include "sudokugridwidget.h"
#include <QLabel>
#include <QKeyEvent>
#include <QMessageBox>
#include <QButtonGroup>
#include <set>

AddDigitsControls::AddDigitsControls(MainWindowContent* mainWindowContent, QWidget *parent)
    : QWidget{parent},
      ContextMenuWindow(mainWindowContent),
      mGrid(mainWindowContent->GridGet()),
      mStackedLayout(new QStackedLayout(this)),
      mCheckBoxes(),
      mCurrentView(MenuView::MainView),
      mFocusedCell(nullptr)
{
    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    // build stacked layout
    this->setLayout(mStackedLayout);
    QWidget* mainView = new QWidget();
    QWidget* detailedView = new QWidget();
    mStackedLayout->insertWidget(MenuView::MainView, mainView);
    mStackedLayout->insertWidget(MenuView::DetailedView, detailedView);

    // build main view
    {
        mainView->setLayout(new QVBoxLayout());
        QLabel* instructions = new QLabel("Select a cell for more details.");
        QFrame* hLine = new QFrame();
        hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
        QPushButton* deleteAllBtn = new QPushButton("Delete All");
        mainView->layout()->addWidget(instructions);
        mainView->layout()->addWidget(hLine);
        mainView->layout()->addWidget(deleteAllBtn);
        static_cast<QVBoxLayout*>(mainView->layout())->addStretch();
        // events
        connect(deleteAllBtn, SIGNAL(clicked(bool)), this, SLOT(DeleteAllBtn_Clicked()));
    }

    // build detailed view
    {
        detailedView->setLayout(new QVBoxLayout());
        QLabel* instructions = new QLabel("Give hints to the solver by checking options that\n"
                                          "should not be allowed in the selected cell.");
        QButtonGroup* checkBoxesGroup = new QButtonGroup(detailedView);
        checkBoxesGroup->setExclusive(false);
        detailedView->layout()->addWidget(instructions);
        mCheckBoxes.reserve(mGrid->SizeGet());
        for (int i = 1; i <= mGrid->SizeGet(); ++i)
        {
            QCheckBox* checkbox = new QCheckBox(QString::number(i));
            detailedView->layout()->addWidget(checkbox);
            checkBoxesGroup->addButton(checkbox, i);
            mCheckBoxes.emplace_back(checkbox);
        }
        QFrame* hLine = new QFrame();
        hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
        QWidget* buttonsWrapper = new QWidget();
        detailedView->layout()->addWidget(hLine);
        detailedView->layout()->addWidget(buttonsWrapper);

        buttonsWrapper->setLayout(new QHBoxLayout());
        QPushButton* doneBtn = new QPushButton("Done");
        QPushButton* deleteBtn = new QPushButton("Clear Hints");
        buttonsWrapper->layout()->addWidget(doneBtn);
        buttonsWrapper->layout()->addWidget(deleteBtn);
        static_cast<QVBoxLayout*>(detailedView->layout())->addStretch();
        // events
        connect(checkBoxesGroup, SIGNAL(idToggled(int,bool)), this, SLOT(HintCheckbox_Toggled(int,bool)));
        connect(doneBtn, SIGNAL(clicked(bool)), this, SLOT(DoneBtn_Clicked()));
        connect(deleteBtn, SIGNAL(clicked(bool)), this, SLOT(DeleteBtn_Clicked()));
    }
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(App_FocusChanged(QWidget*,QWidget*)));
}

void AddDigitsControls::CellGainedFocus(SudokuCellWidget *cell)
{
    if(cell != mFocusedCell)
    {
        FocusedCellSet(cell);
        SwitchView(MenuView::DetailedView);

        std::set<unsigned short> hints;
        mGrid->PuzzleDataGet()->HintsGet(cell->CellIdGet(), hints);
        for (int i = 0; i < mGrid->SizeGet(); ++i)
        {
            mCheckBoxes[i]->setChecked(hints.count(i+1) > 0);
        }
    }
}

void AddDigitsControls::CellLostFocus(SudokuCellWidget *cell)
{
    if(cell == mFocusedCell)
    {
        QWidget* focusWidget =  QApplication::focusWidget();
        while (focusWidget)
        {
            if(focusWidget == this) { break; }
            focusWidget = focusWidget->parentWidget();
        }

        if(!focusWidget)
        {
            FocusedCellSet(nullptr);
            for (const auto& btn : mCheckBoxes)
            {
                btn->setChecked(false);
            }
            SwitchView(MenuView::MainView);
        }
    }
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

void AddDigitsControls::DeleteAllBtn_Clicked()
{
    auto btn = static_cast<QMessageBox::StandardButton>(QMessageBox::warning(this, "Delete All Digits", "All the given digits in the grid will be deleted.\nContinue?",
                                                        QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel));

    if(btn == QMessageBox::StandardButton::Ok)
    {
        const auto& cells = mGrid->CellsGet();
        for (int i = 0; i < cells.size(); ++i) {
            for (int j = 0; j < cells.size(); ++j) {
                cells[i][j]->RemoveGivenDigit();
            }
        }
        mGrid->PuzzleDataGet()->RemoveAllHints();
    }
}

void AddDigitsControls::HintCheckbox_Toggled(int id, bool checked)
{
    if(!mFocusedCell || mMainWindowContent->ActiveContextMenuGet() != this)
    {
        return;
    }

    if(checked)
    {
        mGrid->PuzzleDataGet()->AddHint(mFocusedCell->CellIdGet(), id);
    }
    else
    {
        mGrid->PuzzleDataGet()->RemoveHint(mFocusedCell->CellIdGet(), id);
    }
}

void AddDigitsControls::DoneBtn_Clicked()
{
    FocusedCellSet(nullptr);
    for (const auto& btn : mCheckBoxes)
    {
        btn->setChecked(false);
    }
    SwitchView(MenuView::MainView);
}

void AddDigitsControls::DeleteBtn_Clicked()
{
    for (const auto& btn : mCheckBoxes)
    {
        btn->setChecked(false);
    }
}

void AddDigitsControls::App_FocusChanged(QWidget *old, QWidget *now)
{
    if(mMainWindowContent->ActiveContextMenuGet() != this)
    {
        return;
    }

    while (old)
    {
        if(old == this) { break; }
        old = old->parentWidget();
    }
    while (now)
    {
        if(now == this || now == mGrid) { break; }
        now = now->parentWidget();
    }

    if(old && !now)
    {
        FocusedCellSet(nullptr);
        for (const auto& btn : mCheckBoxes)
        {
            btn->setChecked(false);
        }
        SwitchView(MenuView::MainView);
    }
}

void AddDigitsControls::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    FocusedCellSet(nullptr);
}

void AddDigitsControls::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    SwitchView(MenuView::MainView);
}

void AddDigitsControls::SwitchView(MenuView newView)
{
    if(mCurrentView != newView)
    {
        mCurrentView = newView;
        mStackedLayout->setCurrentIndex(mCurrentView);
    }
}

void AddDigitsControls::FocusedCellSet(SudokuCellWidget *cell)
{
    if(mFocusedCell != cell)
    {
        if(mFocusedCell)
        {
            mFocusedCell->SetHighlighted(false);
        }
        mFocusedCell = cell;
        if(mFocusedCell)
        {
            mFocusedCell->SetHighlighted(true);
        }
    }
}
