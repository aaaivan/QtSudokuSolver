#include "editgridcontrols.h"
#include "puzzledata.h"
#include "sudokugridwidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QButtonGroup>

EditGridControls::EditGridControls(MainWindowContent* mainWindowContent, QWidget *parent)
    : QWidget{parent},
      mMainWindowContent(mainWindowContent),
      mAddDigitsBtn(new QPushButton("Enter Given Digits")),
      mDrawRegionsBtn(new QPushButton("Draw Regions")),
      mDrawKillersBtn(new QPushButton("Draw Killer Cages")),
      mPositiveDiagonalCheckbox(new QCheckBox("Add positive diagonal constraint")),
      mNegativeDiagonalCheckbox(new QCheckBox("Add negative diagonal constraint"))
{
    // build the layout
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    verticalLayout->addWidget(mAddDigitsBtn);
    verticalLayout->addWidget(mDrawRegionsBtn);
    verticalLayout->addWidget(mDrawKillersBtn);
    QButtonGroup* btnGroup = new QButtonGroup(this);
    btnGroup->addButton(mAddDigitsBtn);
    btnGroup->addButton(mDrawRegionsBtn);
    btnGroup->addButton(mDrawKillersBtn);
    btnGroup->setId(mAddDigitsBtn, MainWindowContent::ViewType::EnterDigits);
    btnGroup->setId(mDrawRegionsBtn, MainWindowContent::ViewType::DrawRegions);
    btnGroup->setId(mDrawKillersBtn, MainWindowContent::ViewType::DrawKiller);


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
    connect(btnGroup, SIGNAL(idClicked(int)), this, SLOT(ViewButtonClicked(int)));
    connect(mPositiveDiagonalCheckbox, SIGNAL(stateChanged(int)), this, SLOT(PositiveDiagonalCheckbox_OnChange(int)));
    connect(mNegativeDiagonalCheckbox, SIGNAL(stateChanged(int)), this, SLOT(NegativeDiagonalCheckbox_OnChange(int)));
}

void EditGridControls::ViewButtonClicked(int btnId)
{
    mMainWindowContent->ChangeView(static_cast<MainWindowContent::ViewType>(btnId));
}

void EditGridControls::PositiveDiagonalCheckbox_OnChange(int checked)
{
    mMainWindowContent->GridGet()->PuzzleDataGet()->PositiveDiagonalConstraintSet(checked);
    mMainWindowContent->GridGet()->update();
}

void EditGridControls::NegativeDiagonalCheckbox_OnChange(int checked)
{
    mMainWindowContent->GridGet()->PuzzleDataGet()->NegativeDiagonalConstraintSet(checked);
    mMainWindowContent->GridGet()->update();
}
