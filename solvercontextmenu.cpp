#include "solvercontextmenu.h"
#include "mainwindowcontent.h"
#include "sudokucellwidget.h"
#include "sudokugridwidget.h"
#include <QVBoxLayout>
#include <QScrollBar>

constexpr char kFinishedStr[] = "Finished.";
constexpr char kCalculatingStr[] = "Calculating...";
constexpr char kCappedSolutionsCount[] = "The puzzle has at least %1 solutions.";
constexpr char kExactSolutionsCount[] = "The puzzle has %1 solutions.";

SolverContextMenu::SolverContextMenu(MainWindowContent* mainWindowContent, QWidget *parent)
    : QWidget{parent}
    , ContextMenuWindow(mainWindowContent)
    , mLogicalSolver(mMainWindowContent->GridGet()->SolverGet())
    , mBruteForceSolver(mMainWindowContent->GridGet()->SolverGet()->BruteSolverGet())
    , mStatusLabel(new QLabel(kFinishedStr))
    , mSolverOutput(new QPlainTextEdit())
    , mFirstMessage(true)
{
    // build vertical layout
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    verticalLayout->addWidget(mStatusLabel);
    verticalLayout->addWidget(mSolverOutput);
    mSolverOutput->setReadOnly(true);

    // events
    connect(mSolverOutput, &QPlainTextEdit::textChanged, this, &SolverContextMenu::SolverOutput_TextChanged);
    connect(mBruteForceSolver, &BruteForceSolverThread::CalculationStarted, this, &SolverContextMenu::OnCalculationStarted);
    connect(mBruteForceSolver, &BruteForceSolverThread::CalculationFinished, this, &SolverContextMenu::OnCalculationFinished);
    connect(mBruteForceSolver, &BruteForceSolverThread::NumberOfSolutionsComputed, this, &SolverContextMenu::OnSolutionsCounted);
    connect(mLogicalSolver, &SudokuSolverThread::CalculationStarted, this, &SolverContextMenu::OnCalculationStarted);
    connect(mLogicalSolver, &SudokuSolverThread::CalculationFinished, this, &SolverContextMenu::OnCalculationFinished);
    connect(mLogicalSolver, &SudokuSolverThread::NewLogicalStep, this, &SolverContextMenu::OnNewSolverMessage);
    connect(mLogicalSolver, &SudokuSolverThread::PuzzleHasNoSolution, this, &SolverContextMenu::OnNewSolverMessage);
    connect(mLogicalSolver, &SudokuSolverThread::SolverHasBeenReset, this, &SolverContextMenu::OnSolverReset);
}

void SolverContextMenu::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    mBruteForceSolver->AbortCalculation();
    mBruteForceSolver->ResetGridContents();
    mLogicalSolver->SetLogicalSolverPaused(false);
    mSolverOutput->clear();
}

void SolverContextMenu::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    mLogicalSolver->SetLogicalSolverPaused(true);
    mSolverOutput->clear();
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

void SolverContextMenu::OnNewSolverMessage(QString message)
{
    if(mFirstMessage)
    {
        mSolverOutput->clear();
        mFirstMessage = false;
    }
    mSolverOutput->appendPlainText(message);
}

void SolverContextMenu::OnSolverReset()
{
    mSolverOutput->clear();
}

void SolverContextMenu::SolverOutput_TextChanged()
{
    if(mSolverOutput->toPlainText().isEmpty())
    {
        mFirstMessage = true;
    }

    mSolverOutput->verticalScrollBar()->setValue(mSolverOutput->verticalScrollBar()->maximum());
}

void SolverContextMenu::CellGainedFocus(SudokuCellWidget *cell)
{
    Q_UNUSED(cell)
}

void SolverContextMenu::CellLostFocus(SudokuCellWidget *cell)
{
    Q_UNUSED(cell)
}

void SolverContextMenu::CellClicked(SudokuCellWidget *cell)
{
    Q_UNUSED(cell)
}

void SolverContextMenu::KeyboardInput(SudokuCellWidget *cell, QKeyEvent *event)
{
    bool ok;
    int num = event->text().toInt(&ok);

    SudokuGridWidget* grid = mMainWindowContent->GridGet();
    if(ok && num > 0 && num <= grid->SizeGet())
    {
        cell->SetGivenDigit(static_cast<unsigned short>(num));
    }
    else if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        cell->RemoveGivenDigit();
        mBruteForceSolver->DisplayCandidatesForCell(cell->CellIdGet());
    }
}
