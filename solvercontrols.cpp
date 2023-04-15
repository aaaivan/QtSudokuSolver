#include "solvercontrols.h"
#include "bruteforcesolverthread.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QFormLayout>
#include <QLabel>

SolverControls::SolverControls(BruteForceSolverThread* bruteForceSolver, QWidget *parent)
    : QWidget{parent}
    , mCountSolutionsBtn(new QPushButton("Count Solutions"))
    , mBruteForceSolveBtn(new QPushButton("Display Solution"))
    , mMaxSolutionsCount(new QSpinBox())
    , mUseHintsCheckbox(new QCheckBox("Use hints as constrainsts"))
    , mAbortCalculationsBtn(new QPushButton("Abort Calculation"))
    , mBruteForceSolver(bruteForceSolver)
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

    QFrame* line = new QFrame();
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    verticalLayout->addWidget(line);
    verticalLayout->addStretch();

    // events
    connect(mCountSolutionsBtn, SIGNAL(clicked(bool)), this, SLOT(CountSolutionsBtn_Clicked()));
    connect(mBruteForceSolveBtn, SIGNAL(clicked(bool)), this, SLOT(DisplaySolutionsBtn_Clicked()));
    connect(mAbortCalculationsBtn, SIGNAL(clicked(bool)), this, SLOT(AbortButton_Clicked()));
    connect(mBruteForceSolver, SIGNAL(CalculationStarted()), this, SLOT(CalculationStarted()));
    connect(mBruteForceSolver, SIGNAL(CalculationFinished()), this, SLOT(CalculationFinished()));
}

void SolverControls::CountSolutionsBtn_Clicked()
{
    mBruteForceSolver->CountSolutions(mMaxSolutionsCount->value(), mUseHintsCheckbox->isChecked());
}

void SolverControls::DisplaySolutionsBtn_Clicked()
{
    mBruteForceSolver->DisplaySolution(mMaxSolutionsCount->value(), mUseHintsCheckbox->isChecked());
}

void SolverControls::AbortButton_Clicked()
{
    mBruteForceSolver->AbortCalculation();
}

void SolverControls::CalculationStarted()
{
    mAbortCalculationsBtn->setEnabled(true);
}

void SolverControls::CalculationFinished()
{
    mAbortCalculationsBtn->setEnabled(false);
}


