#include "solvercontrols.h"
#include "bruteforcesolverthread.h"
#include "mainwindowcontent.h"
#include "sudokugridwidget.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QFormLayout>
#include <QLabel>

SolverControls::SolverControls(BruteForceSolverThread* bruteForceSolver, MainWindowContent* mainWindow, QWidget *parent)
    : QWidget{parent}
    , mCountSolutionsBtn(new QPushButton("Count Solutions"))
    , mBruteForceSolveBtn(new QPushButton("Display Solution"))
    , mMaxSolutionsCount(new QSpinBox())
    , mUseHintsCheckbox(new QCheckBox("Use hints as constrainsts"))
    , mAbortCalculationsBtn(new QPushButton("Abort Calculation"))
    , mClearGridBtn(new QPushButton("Clear Grid"))
    , mLogicalStepBtn(new QPushButton("Take Logical Step"))
    , mMainWindow(mainWindow)
    , mBruteForceSolver(bruteForceSolver)
    , mSolverThread(mainWindow->GridGet()->SolverGet())
{
    // build the vertical layout
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    QWidget* formWidget = new QWidget();
    verticalLayout->addWidget(mCountSolutionsBtn);
    verticalLayout->addWidget(mBruteForceSolveBtn);
    verticalLayout->addWidget(formWidget);
    verticalLayout->addWidget(mUseHintsCheckbox);
    verticalLayout->addWidget(mAbortCalculationsBtn);

    QFrame* line = new QFrame();
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line = new QFrame();
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    verticalLayout->addWidget(line);
    verticalLayout->addWidget(mLogicalStepBtn);
    verticalLayout->addWidget(mClearGridBtn);
    verticalLayout->addStretch();

    // max solutions count layout
    QFormLayout* formLayout = new QFormLayout(formWidget);
    formWidget->setLayout(formLayout);
    QLabel* solutionsLabel = new QLabel("Max solutions count:");
    formLayout->addRow(solutionsLabel, mMaxSolutionsCount);
    formLayout->setContentsMargins(0,0,0,0);

    // set intial states
    mMaxSolutionsCount->setRange(100, 100000);
    mMaxSolutionsCount->setValue(1000);
    mAbortCalculationsBtn->setEnabled(false);

    // events
    connect(mCountSolutionsBtn, SIGNAL(clicked(bool)), this, SLOT(CountSolutionsBtn_Clicked()));
    connect(mBruteForceSolveBtn, SIGNAL(clicked(bool)), this, SLOT(DisplaySolutionsBtn_Clicked()));
    connect(mAbortCalculationsBtn, SIGNAL(clicked(bool)), this, SLOT(AbortButton_Clicked()));
    connect(mClearGridBtn, SIGNAL(clicked(bool)), this, SLOT(ClearGridBtn_Clicked()));
    connect(mLogicalStepBtn, SIGNAL(clicked(bool)), this, SLOT(LogicalStepBtn_Clicked()));
    connect(mBruteForceSolver, SIGNAL(CalculationStarted()), this, SLOT(CalculationStarted()));
    connect(mBruteForceSolver, SIGNAL(CalculationFinished()), this, SLOT(CalculationFinished()));
}

void SolverControls::CountSolutionsBtn_Clicked()
{
    mSolverThread->SetLogicalSolverPaused(true);
    mBruteForceSolver->CountSolutions(mMaxSolutionsCount->value(), mUseHintsCheckbox->isChecked());
}

void SolverControls::DisplaySolutionsBtn_Clicked()
{
    mSolverThread->SetLogicalSolverPaused(true);
    mBruteForceSolver->DisplaySolution(mMaxSolutionsCount->value(), mUseHintsCheckbox->isChecked());
}

void SolverControls::ClearGridBtn_Clicked()
{
    mSolverThread->ResetSolver();
}

void SolverControls::AbortButton_Clicked()
{
    mBruteForceSolver->AbortCalculation();
}

void SolverControls::LogicalStepBtn_Clicked()
{
    mBruteForceSolver->AbortCalculation();
    mBruteForceSolver->ResetGridContents();
    mSolverThread->TakeStep();
}

void SolverControls::CalculationStarted()
{
    mAbortCalculationsBtn->setEnabled(true);
}

void SolverControls::CalculationFinished()
{
    mAbortCalculationsBtn->setEnabled(false);
}
