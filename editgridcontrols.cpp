#include "editgridcontrols.h"
#include "sudokugridwidget.h"
#include "mainwindowcontent.h"
#include "sudokusolverthread.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QButtonGroup>

EditGridControls::EditGridControls(MainWindowContent* mainWindowContent, QWidget *parent)
    : QWidget{parent},
      mMainWindowContent(mainWindowContent),
      mButtonsGroup(new QButtonGroup(this)),
      mAddDigitsBtn(new QPushButton("Given Digits")),
      mDrawRegionsBtn(new QPushButton("Regions")),
      mDrawKillersBtn(new QPushButton("Killer Cages")),
      mPositiveDiagonalCheckbox(new QCheckBox("Positive diagonal constraint")),
      mNegativeDiagonalCheckbox(new QCheckBox("Negative diagonal constraint"))
{
    // build the layout
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    verticalLayout->addWidget(mAddDigitsBtn);
    verticalLayout->addWidget(mDrawRegionsBtn);
    verticalLayout->addWidget(mDrawKillersBtn);

    mButtonsGroup->addButton(mAddDigitsBtn);
    mButtonsGroup->addButton(mDrawRegionsBtn);
    mButtonsGroup->addButton(mDrawKillersBtn);
    mButtonsGroup->setId(mAddDigitsBtn, MainWindowContent::ContextMenuType::EnterDigits_Context);
    mButtonsGroup->setId(mDrawRegionsBtn, MainWindowContent::ContextMenuType::DrawRegions_Context);
    mButtonsGroup->setId(mDrawKillersBtn, MainWindowContent::ContextMenuType::DrawKiller_Context);

    QFrame* line = new QFrame();
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    verticalLayout->addWidget(line);
    verticalLayout->addWidget(mPositiveDiagonalCheckbox);
    verticalLayout->addWidget(mNegativeDiagonalCheckbox);
    verticalLayout->addStretch();

    // buttons mode
    mAddDigitsBtn->setCheckable(true);
    mAddDigitsBtn->setChecked(true);
    mDrawRegionsBtn->setCheckable(true);
    mDrawKillersBtn->setCheckable(true);

    // events
    connect(mButtonsGroup, SIGNAL(idClicked(int)), this, SLOT(ViewButtonClicked(int)));
    connect(mPositiveDiagonalCheckbox, SIGNAL(stateChanged(int)), this, SLOT(PositiveDiagonalCheckbox_OnChange(int)));
    connect(mNegativeDiagonalCheckbox, SIGNAL(stateChanged(int)), this, SLOT(NegativeDiagonalCheckbox_OnChange(int)));
}

void EditGridControls::ViewButtonClicked(int btnId)
{
    mMainWindowContent->ChangeView(static_cast<MainWindowContent::ContextMenuType>(btnId));
}

void EditGridControls::PositiveDiagonalCheckbox_OnChange(int checked)
{
    mMainWindowContent->GridGet()->SolverGet()->PositiveDiagonalConstraintSet(checked);
    mMainWindowContent->GridGet()->SolverGet()->SubmitChangesToSolver();
    mMainWindowContent->GridGet()->update();
}

void EditGridControls::NegativeDiagonalCheckbox_OnChange(int checked)
{
    mMainWindowContent->GridGet()->SolverGet()->NegativeDiagonalConstraintSet(checked);
    mMainWindowContent->GridGet()->SolverGet()->SubmitChangesToSolver();
    mMainWindowContent->GridGet()->update();
}

int EditGridControls::SelectedButtonIdGet() const
{
    return mButtonsGroup->checkedId();
}
