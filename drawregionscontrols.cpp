#include "drawregionscontrols.h"
#include "sudokucellwidget.h"
#include "sudokugridwidget.h"
#include "puzzledata.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QKeyEvent>

constexpr char kCounterLabelText[] = "Cells in region %1: <b>%2</b>";

DrawRegionsControls::DrawRegionsControls(MainWindowContent* mainWindowContent, QWidget *parent)
    : QWidget{parent},
      ContextMenuWindow(mainWindowContent),
      mGrid(mainWindowContent->GridGet()),
      mRegionSelect(new QComboBox()),
      mCellCounters(mGrid->SizeGet()),
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

void DrawRegionsControls::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    for(const auto& vect : mGrid->CellsGet())
    {
        for(const auto& c : vect)
        {
            c->SetHighlighted(false);
        }
    }
}

void DrawRegionsControls::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    for(const auto& vect : mGrid->CellsGet())
    {
        for(const auto& c : vect)
        {
            c->SetHighlighted(c->RegionIdGet() == mRegionSelect->currentIndex());
        }
    }
}

void DrawRegionsControls::RegionSelect_CurrentIndexChanged(int index)
{
    if(mMainWindowContent->ActiveContextMenuGet() != this)
    {
        return;
    }

    for(const auto& vect : mGrid->CellsGet())
    {
        for(const auto& c : vect)
        {
            c->SetHighlighted(c->RegionIdGet() == index);
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

void DrawRegionsControls::CellGainedFocus(SudokuCellWidget *cell)
{
    unsigned short newId = SelectedRegionIdGet();
    if(cell->RegionIdGet() == newId)
    {
        cell->ResetRegionId();
    }
    else
    {
        cell->SetRegionId(newId);
    }
}

void DrawRegionsControls::CellLostFocus(SudokuCellWidget *cell)
{
    Q_UNUSED(cell)
}

void DrawRegionsControls::KeyboardInput(SudokuCellWidget *cell, QKeyEvent *event)
{
    bool ok;
    int num = event->text().toInt(&ok);

    if(ok && num >= 0 && num <= mGrid->SizeGet())
    {
        if(cell->RegionIdGet() == num)
        {
            cell->ResetRegionId();
        }
        else
        {
            cell->SetRegionId(num);
        }
    }
    else if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        cell->ResetRegionId();
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
