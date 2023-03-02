#include "editgridcontrols.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFrame>

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
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::Shape::HLine);
    line->setFrameShadow(QFrame::Shadow::Sunken);
    verticalLayout->addWidget(line);
    verticalLayout->addWidget(mPositiveDiagonalCheckbox);
    verticalLayout->addWidget(mNegativeDiagonalCheckbox);
    verticalLayout->addStretch();

    // buttons mode
    mAddDigitsBtn->setCheckable(true);
    mAddDigitsBtn->setChecked(true);
    AddDigitsBtn_Toggled(true);
    mDrawRegionsBtn->setCheckable(true);
    mDrawKillersBtn->setCheckable(true);

    // events
    connect(mAddDigitsBtn, SIGNAL(toggled(bool)), this, SLOT(AddDigitsBtn_Toggled(bool)));
    connect(mDrawRegionsBtn, SIGNAL(toggled(bool)), this, SLOT(DrawRegionsBtn_Toggled(bool)));
    connect(mDrawKillersBtn, SIGNAL(toggled(bool)), this, SLOT(DrawKillersBtn_Toggled(bool)));
    connect(mPositiveDiagonalCheckbox, SIGNAL(stateChanged(int)), this, SLOT(PositiveDiagonalCheckbox_OnChange(int)));
    connect(mNegativeDiagonalCheckbox, SIGNAL(stateChanged(int)), this, SLOT(NegativeDiagonalCheckbox_OnChange(int)));


}

MainWindowContent::ViewType EditGridControls::GetViewForButton(QPushButton *btn)
{
    MainWindowContent::ViewType result = MainWindowContent::ViewType::EnterDigits;
    if(btn == mAddDigitsBtn)
    {
        result = MainWindowContent::ViewType::EnterDigits;
    }
    else if(btn == mDrawRegionsBtn)
    {
        result = MainWindowContent::ViewType::DrawRegions;
    }
    else if(btn == mAddDigitsBtn)
    {
        result = MainWindowContent::ViewType::DrawKiller;
    }
    return result;
}

void EditGridControls::AddDigitsBtn_Toggled(bool checked)
{
    if(checked)
    {
        OnViewButtonChecked(mAddDigitsBtn);
    }
}

void EditGridControls::DrawRegionsBtn_Toggled(bool checked)
{
    if(checked)
    {
        OnViewButtonChecked(mDrawRegionsBtn);
    }
}

void EditGridControls::DrawKillersBtn_Toggled(bool checked)
{
    if(checked)
    {
        OnViewButtonChecked(mDrawKillersBtn);
    }
}

void EditGridControls::OnViewButtonChecked(QPushButton *btn)
{
    mAddDigitsBtn->setChecked(btn == mAddDigitsBtn);
    mDrawRegionsBtn->setChecked(btn == mDrawRegionsBtn);
    mDrawKillersBtn->setChecked(btn == mDrawKillersBtn);
    mAddDigitsBtn->setAttribute(Qt::WA_TransparentForMouseEvents, btn == mAddDigitsBtn);
    mDrawRegionsBtn->setAttribute(Qt::WA_TransparentForMouseEvents, btn == mDrawRegionsBtn);
    mDrawKillersBtn->setAttribute(Qt::WA_TransparentForMouseEvents, btn == mDrawKillersBtn);

    mMainWindowContent->ChangeView(GetViewForButton(btn));
}

void EditGridControls::PositiveDiagonalCheckbox_OnChange(int checked)
{
    qDebug("positive diagonal checkbox %d\n", checked);
}

void EditGridControls::NegativeDiagonalCheckbox_OnChange(int checked)
{
    qDebug("negative diagonal checkbox %d\n", checked);
}
