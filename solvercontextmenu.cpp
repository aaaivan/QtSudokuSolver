#include "solvercontextmenu.h"
#include "mainwindowcontent.h"
#include "sudokugridwidget.h"
#include <QVBoxLayout>

constexpr char kFinishedStr[] = "Finished.";
constexpr char kCalculatingStr[] = "Calculating...";
constexpr char kCappedSolutionsCount[] = "The puzzle has at least %1 solutions.";
constexpr char kExactSolutionsCount[] = "The puzzle has %1 solutions.";

SolverContextMenu::SolverContextMenu(MainWindowContent* mainWindowContent, QWidget *parent)
    : QWidget{parent}
    , ContextMenuWindow(mainWindowContent)
    , mStatusLabel(new QLabel(kFinishedStr))
    , mSolverOutput(new QPlainTextEdit())
{
    // build vertical layout
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    verticalLayout->addWidget(mStatusLabel);
    verticalLayout->addWidget(mSolverOutput);
    mSolverOutput->setReadOnly(true);

    // events
    BruteForceSolverThread* bruteForceSolver = mMainWindowContent->GridGet()->SolverGet()->BruteSolverGet();
    connect(bruteForceSolver, &BruteForceSolverThread::CalculationStarted, this, &SolverContextMenu::OnCalculationStarted);
    connect(bruteForceSolver, &BruteForceSolverThread::CalculationFinished, this, &SolverContextMenu::OnCalculationFinished);
    connect(bruteForceSolver, &BruteForceSolverThread::NumberOfSolutionsComputed, this, &SolverContextMenu::OnSolutionsCounted);
}

void SolverContextMenu::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    mMainWindowContent->GridGet()->SolverGet()->BruteSolverGet()->AbortCalculation();
    mMainWindowContent->GridGet()->SolverGet()->SetLogicalSolverPaused(false);
    mSolverOutput->clear();
}

void SolverContextMenu::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    mMainWindowContent->GridGet()->SolverGet()->SetLogicalSolverPaused(true);
}

void SolverContextMenu::OnCalculationStarted()
{
    mStatusLabel->setText(kCalculatingStr);
}

void SolverContextMenu::OnCalculationFinished()
{
    mStatusLabel->setText(kFinishedStr);
}

void SolverContextMenu::OnSolutionsCounted(size_t count, bool stopped)
{
    if(stopped)
    {
        mSolverOutput->clear();
        mSolverOutput->appendPlainText(QString(kCappedSolutionsCount).arg(count));
    }
    else
    {
        mSolverOutput->clear();
        mSolverOutput->appendPlainText(QString(kExactSolutionsCount).arg(count));
    }
}

void SolverContextMenu::CellGainedFocus(SudokuCellWidget *cell)
{

}

void SolverContextMenu::CellLostFocus(SudokuCellWidget *cell)
{

}

void SolverContextMenu::CellClicked(SudokuCellWidget *cell)
{

}

void SolverContextMenu::KeyboardInput(SudokuCellWidget *cell, QKeyEvent *event)
{

}
