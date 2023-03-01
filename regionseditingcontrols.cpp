#include "regionseditingcontrols.h"
#include "sudokugridwidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>

RegionsEditingControls::RegionsEditingControls(SudokuGridWidget* grid, QWidget *parent)
    : QWidget{parent},
      mRegionSelect(new QComboBox()),
      mGrid(grid)
{
    // build vertical layout
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    QWidget* formWidget = new QWidget();
    verticalLayout->addWidget(formWidget);

    // build form layout
    QFormLayout* formLayout = new QFormLayout(formWidget);
    formWidget->setLayout(formLayout);
    QLabel* selectLabel = new QLabel("Region id:");
    formLayout->addRow(selectLabel, mRegionSelect);

    // populate combo box with options
    mRegionSelect->addItem("None", 0);
    for(unsigned short i = 1; i <= mGrid->SizeGet(); ++i)
    {
        mRegionSelect->addItem(QString::number(i), i);
    }
    connect(mRegionSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(OnRegionNumChanged(int)));
}

void RegionsEditingControls::OnRegionNumChanged(int index)
{
    qDebug("selected region with id: %d\n", index);
}
