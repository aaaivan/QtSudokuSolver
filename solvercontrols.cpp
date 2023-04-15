#include "solvercontrols.h"
#include "bruteforcesolverthread.h"
#include <QVBoxLayout>
#include <QFrame>

SolverControls::SolverControls(BruteForceSolverThread* bruteForceSolver, QWidget *parent)
    : QWidget{parent}
    , mCountSolutionsBtn(new QPushButton("Count Solutions"))
    , mBruteForceSolveBtn(new QPushButton("Display Solution"))
    , mUseHintsCheckbox(new QCheckBox("Use hints as constrainsts"))
    , mAbortCalculationsBtn(new QPushButton("Abort Calculation"))
    , mBruteForceSolver(bruteForceSolver)
{
    // build the layout
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    verticalLayout->addWidget(mCountSolutionsBtn);
    verticalLayout->addWidget(mBruteForceSolveBtn);
    verticalLayout->addWidget(mUseHintsCheckbox);
    verticalLayout->addWidget(mAbortCalculationsBtn);
    mAbortCalculationsBtn->setEnabled(false);

    QFrame* line = new QFrame();
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    verticalLayout->addWidget(line);
    verticalLayout->addStretch();

    // events
    connect(mCountSolutionsBtn, SIGNAL(clicked(bool)), this, SLOT(CountSolutionsBtn_Clicked()));
    connect(mBruteForceSolveBtn, SIGNAL(clicked(bool)), this, SLOT(DisplaySolutionsBtn_Clicked()));
    connect(mAbortCalculationsBtn, SIGNAL(clicked(bool)), this, SLOT(AbortButton_Clicked()));
    connect(mBruteForceSolver, SIGNAL(NumberOfSolutionsComputed(size_t)), this, SLOT(CalculationFinished()));
}

void SolverControls::CountSolutionsBtn_Clicked()
{
    mBruteForceSolver->CountSolutions(1000, mUseHintsCheckbox->isChecked());
    mAbortCalculationsBtn->setEnabled(true);
}

void SolverControls::DisplaySolutionsBtn_Clicked()
{
    mBruteForceSolver->DisplaySolution(1000, mUseHintsCheckbox->isChecked());
    mAbortCalculationsBtn->setEnabled(true);
}

void SolverControls::AbortButton_Clicked()
{
    mBruteForceSolver->AbortCalculation();
    mAbortCalculationsBtn->setEnabled(false);
}

void SolverControls::CalculationFinished()
{
    mAbortCalculationsBtn->setEnabled(false);
}


