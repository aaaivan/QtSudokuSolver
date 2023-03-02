#include "drawregionscontrols.h"
#include "sudokucellwidget.h"
#include "sudokugridwidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>

DrawRegionsControls::DrawRegionsControls(SudokuGridWidget* grid, QWidget *parent)
    : QWidget{parent},
      mGrid(grid),
      mRegionSelect(new QComboBox()),
      mClearRegionsBtn(new QPushButton("Clear Regions"))
{
    // build vertical layout
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    QWidget* formWidget = new QWidget();
    verticalLayout->addWidget(formWidget);
    verticalLayout->addWidget(mClearRegionsBtn);
    verticalLayout->addStretch();

    // build form layout
    QFormLayout* formLayout = new QFormLayout(formWidget);
    formWidget->setLayout(formLayout);
    QLabel* selectLabel = new QLabel("Region id:");
    formLayout->addRow(selectLabel, mRegionSelect);

    // populate combo box with options
    mRegionSelect->addItem("None", 0);
    RegionSelect_CurrentIndexChanged(0);
    for(unsigned short i = 1; i <= mGrid->SizeGet(); ++i)
    {
        mRegionSelect->addItem(QString::number(i), i);
    }

    connect(mRegionSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(RegionSelect_CurrentIndexChanged(int)));
    connect(mClearRegionsBtn, SIGNAL(clicked(bool)), this, SLOT(ClearRegionsBtn_Clicked()));
}

void DrawRegionsControls::RegionSelect_CurrentIndexChanged(int index)
{
    for(const auto& vect : mGrid->CellsGet())
    {
        for(const auto& c : vect)
        {
            c->HighlightRegionLabel(c->RegionIdGet() == index);
        }
    }
}

void DrawRegionsControls::ClearRegionsBtn_Clicked()
{
    for(const auto& vect : mGrid->CellsGet())
    {
        for(const auto& c : vect)
        {
            c->ResetRegionId();
        }
    }
    mRegionSelect->setCurrentIndex(0);
}

unsigned short DrawRegionsControls::SelectedRegionIdGet() const
{
    return mRegionSelect->currentData().toInt();
}
