#include "drawkillerscontrols.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>

DrawKillersControls::DrawKillersControls(QWidget *parent)
    : QWidget{parent},
    mStackedLayout(new QStackedLayout())
{
    // build stacked layout
    this->setLayout(mStackedLayout);
    QWidget* buttonsView = new QWidget();
    QPushButton* v1 = new QPushButton("Needs implementation");
    QPushButton* v2 = new QPushButton("Needs implementation");
    mStackedLayout->insertWidget(MenuView::ControlButtons, buttonsView);
    mStackedLayout->insertWidget(MenuView::AddKiller, v1);
    mStackedLayout->insertWidget(MenuView::EditKiller, v2);

    // build buttons view
    QVBoxLayout* horizontalLayout = new QVBoxLayout();
    buttonsView->setLayout(horizontalLayout);
    QPushButton* newKiller = new QPushButton("New Killer Cage");
    QPushButton* editKiller = new QPushButton("Edit Killer Cage");
    QPushButton* clearAllKillerCages = new QPushButton("Clear All Killer Cages");
    horizontalLayout->addWidget(newKiller);
    horizontalLayout->addWidget(editKiller);
    QFrame* hLine = new QFrame();
    hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    horizontalLayout->addWidget(hLine);
    horizontalLayout->addWidget(clearAllKillerCages);
    horizontalLayout->addStretch();

    // events
    connect(newKiller, SIGNAL(clicked(bool)), this, SLOT(NewKillerBtn_Clicked()));
    connect(editKiller, SIGNAL(clicked(bool)), this, SLOT(EditKillerBtn_Clicked()));
    connect(clearAllKillerCages, SIGNAL(clicked(bool)), this, SLOT(ClearKillersBtn_Clicked()));
}

void DrawKillersControls::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    mStackedLayout->setCurrentIndex(MenuView::ControlButtons);
}

void DrawKillersControls::NewKillerBtn_Clicked()
{
    mStackedLayout->setCurrentIndex(MenuView::AddKiller);
}

void DrawKillersControls::EditKillerBtn_Clicked()
{
    mStackedLayout->setCurrentIndex(MenuView::EditKiller);
}

void DrawKillersControls::ClearKillersBtn_Clicked()
{
    auto btn = static_cast<QMessageBox::StandardButton>(QMessageBox::warning(this, "Clear Killer Cages", "All the killer cages in the grid will be cleared.\nContinue?",
                                                        QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel));

    if(btn == QMessageBox::StandardButton::Ok)
    {
        //TODO: implement the killer deletion
    }
}
