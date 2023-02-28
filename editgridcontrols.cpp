#include "editgridcontrols.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFrame>

EditGridControls::EditGridControls(QWidget *parent)
    : QWidget{parent},
      addDigitsBtn(new QPushButton("Enter Given Digits")),
      drawRegionsBtn(new QPushButton("Draw Regions")),
      drawKillersBtn(new QPushButton("Draw Killer Cages")),
      positiveDiagonalCheckbox(new QCheckBox("Add positive diagonal constraint")),
      negativeDiagonalCheckbox(new QCheckBox("Add negative diagonal constraint"))
{
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    verticalLayout->addWidget(addDigitsBtn);
    verticalLayout->addWidget(drawRegionsBtn);
    verticalLayout->addWidget(drawKillersBtn);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::Shape::HLine);
    line->setFrameShadow(QFrame::Shadow::Sunken);
    verticalLayout->addWidget(line);
    verticalLayout->addWidget(positiveDiagonalCheckbox);
    verticalLayout->addWidget(negativeDiagonalCheckbox);
    verticalLayout->addStretch();

    connect(positiveDiagonalCheckbox, SIGNAL(stateChanged(int)), this, SLOT(OnPositiveDiagonalChanged(int)));
    connect(negativeDiagonalCheckbox, SIGNAL(stateChanged(int)), this, SLOT(OnNegativeDiagonalChanged(int)));
}

void EditGridControls::OnPositiveDiagonalChanged(int checked)
{
    qDebug("positive diagonal checkbox %d\n", checked);
}

void EditGridControls::OnNegativeDiagonalChanged(int checked)
{
    printf("negative diagonal checkbox: %d\n", checked);
}
