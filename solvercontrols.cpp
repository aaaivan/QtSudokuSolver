#include "solvercontrols.h"
#include "mainwindowcontent.h"
#include "sudokugridwidget.h"
#include "sudokusolverthread.h"
#include <QVBoxLayout>
#include <QFrame>

SolverControls::SolverControls(MainWindowContent* mainWindowContent, QWidget *parent)
    : QWidget{parent}
    , mCountSolutionsButton(new QPushButton("Count Solutions"))
    , mBruteForceSolve(new QPushButton("Find Solution"))
    , mUseHintsCheckbox(new QCheckBox("Use hints as constrainsts"))
    , mMainWindowContent(mainWindowContent)
{
    // build the layout
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    verticalLayout->addWidget(mCountSolutionsButton);
    verticalLayout->addWidget(mBruteForceSolve);
    verticalLayout->addWidget(mUseHintsCheckbox);
    QFrame* line = new QFrame();
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    verticalLayout->addWidget(line);
    verticalLayout->addStretch();

    // events
    connect(mCountSolutionsButton, SIGNAL(clicked(bool)), this, SLOT(CountSolutionsBtn_Clicked()));
}

void SolverControls::CountSolutionsBtn_Clicked()
{
    SudokuSolverThread* solver = mMainWindowContent->GridGet()->SolverGet();
    solver->CountSolutions(1000, mUseHintsCheckbox->isChecked());
}
