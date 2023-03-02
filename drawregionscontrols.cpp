#include "drawregionscontrols.h"
#include "sudokucellwidget.h"
#include "sudokugridwidget.h"
#include "puzzledata.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>

constexpr char kCounterLabelText[] = "Cells in region %1: <b>%2</b>";

DrawRegionsControls::DrawRegionsControls(SudokuGridWidget* grid, QWidget *parent)
    : QWidget{parent},
      mGrid(grid),
      mRegionSelect(new QComboBox()),
      mCellCounters(grid->SizeGet()),
      mClearRegionsBtn(new QPushButton("Clear Regions"))
{
    // build vertical layout
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    QWidget* formWidget = new QWidget();
    verticalLayout->addWidget(formWidget);
    for(int i = 0; i < mCellCounters.size(); ++i)
    {
        mCellCounters[i] = new QLabel(QString(kCounterLabelText).arg(QString::number(i+1), QString::number(0)));
        verticalLayout->addWidget(mCellCounters[i]);
    }
    QFrame* hLine = new QFrame();
    hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    verticalLayout->addWidget(hLine);
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
    auto btn = static_cast<QMessageBox::StandardButton>(QMessageBox::warning(this, "Clear Regions", "All the regions in the grid will be cleared.\nContinue?",
                                                        QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel));

    if(btn == QMessageBox::StandardButton::Ok)
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
}

unsigned short DrawRegionsControls::SelectedRegionIdGet() const
{
    return mRegionSelect->currentData().toInt();
}

void DrawRegionsControls::UpdateCellCounters(unsigned short regionId)
{
    unsigned short index = regionId - 1;
    if(index < mGrid->SizeGet())
    {
        PuzzleData* puzzleData = mGrid->PuzzleDataGet();
        int count = puzzleData->CellCountInRegion(regionId);
        QString text = kCounterLabelText;
        if(count == mGrid->SizeGet())
        {
            text = "<span style='color:green;'>" + text + "</span>";
        }
        mCellCounters[index]->setText(QString(text).arg(QString::number(regionId), QString::number(count)));
    }
}
