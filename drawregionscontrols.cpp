#include "drawregionscontrols.h"
#include "sudokucellwidget.h"
#include "sudokugridwidget.h"
#include "mainwindowcontent.h"
#include "sudokusolverthread.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QKeyEvent>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

constexpr char kCounterLabelText[] = "Cells in region %1: <b>%2</b>";

DrawRegionsControls::DrawRegionsControls(MainWindowContent* mainWindowContent, const PuzzleData* loadedGrid, QWidget *parent)
    : QWidget{parent},
      ContextMenuWindow(mainWindowContent),
      mGrid(mainWindowContent->GridGet()),
      mRegionSelect(new QComboBox()),
      mCellCounters(mGrid->SizeGet()),
      mClearRegionsBtn(new QPushButton("Clear Regions")),
      mRegions(mGrid->SizeGet(), std::set<CellCoord>())
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

    // load puzzle
    if(loadedGrid)
    {
        for (size_t i = 0; i < loadedGrid->mRegions.size(); ++i)
        {
            for (const auto& cId : loadedGrid->mRegions[i])
            {
                SudokuCellWidget* c = mGrid->CellGet(cId);
                c->SetRegionId(i+1);
            }
            mGrid->SolverGet()->SetRegion(i+1, loadedGrid->mRegions[i]);
        }
        mGrid->SolverGet()->SubmitChangesToSolver();
    }
    else
    {
        unsigned int size = mGrid->SizeGet();
        int rf = std::sqrt(size);
        while(size % rf != 0)
        {
            rf--;
        }
        int cf = size / rf;

        for(unsigned int row = 0; row < size; ++row)
        {
            for(unsigned int col = 0; col < size; ++col)
            {
                int i = row / rf;
                int j = col / cf;
                int regId = i * rf + j + 1;
                CellCoord cellId = row * size + col;
                mGrid->CellGet(cellId)->SetRegionId(regId);
                mRegions.at(regId-1).insert(cellId);
            }
        }

        for(size_t i = 0; i < mRegions.size(); ++i)
        {
            mGrid->SolverGet()->SetRegion(i+1, mRegions.at(i));
            mRegions.at(i).clear();
        }
        mGrid->SolverGet()->SubmitChangesToSolver();
    }
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

    for(size_t i = 0; i < mRegions.size(); ++i)
    {
        mGrid->SolverGet()->SetRegion(i+1, mRegions.at(i));
        mRegions.at(i).clear();
    }
    mGrid->SolverGet()->SubmitChangesToSolver();
}

void DrawRegionsControls::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    for(const auto& vect : mGrid->CellsGet())
    {
        for(const auto& c : vect)
        {
            c->SetHighlighted(c->RegionIdGet() == mRegionSelect->currentIndex());
            if(c->RegionIdGet() > 0)
            {
                mRegions.at(c->RegionIdGet() - 1).insert(c->CellIdGet());
            }
        }
    }
    for(size_t i = 1; i <= mRegions.size(); ++i)
    {
        UpdateCellCounters(i);
    }
}

void DrawRegionsControls::SetRegionIdOfCell(SudokuCellWidget *cell, unsigned short newId)
{
    const unsigned short oldId = cell->RegionIdGet();
    const unsigned short newIndex = newId - 1;
    const unsigned short oldIndex = oldId - 1;
    if(newId != oldId)
    {
        if(newId == 0)
        {
            cell->ResetRegionId();
            if(auto it = mRegions.at(oldIndex).find(cell->CellIdGet());
                    it != mRegions.at(oldIndex).end())
            {
                mRegions.at(oldIndex).erase(it);
            }
        }
        else if(mRegions.at(newIndex).size() < mGrid->SizeGet())
        {
            cell->SetRegionId(newId);
            mRegions.at(newIndex).insert(cell->CellIdGet());
            if(oldId != 0)
            {
                if(auto it = mRegions.at(oldIndex).find(cell->CellIdGet());
                        it != mRegions.at(oldIndex).end())
                {
                    mRegions.at(oldIndex).erase(it);
                }
            }
        }
        UpdateCellCounters(oldId);
        UpdateCellCounters(newId);
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
                SetRegionIdOfCell(c, 0);
            }
        }
        mRegionSelect->setCurrentIndex(0);
    }
}

void DrawRegionsControls::CellGainedFocus(SudokuCellWidget *cell)
{
    Q_UNUSED(cell)
}

void DrawRegionsControls::CellLostFocus(SudokuCellWidget *cell)
{
    Q_UNUSED(cell)
}

void DrawRegionsControls::CellClicked(SudokuCellWidget *cell)
{
    unsigned short newId = SelectedRegionIdGet();
    const unsigned short oldId = cell->RegionIdGet();
    if(newId == oldId)
    {
        newId = 0;
    }
    SetRegionIdOfCell(cell, newId);
}

void DrawRegionsControls::KeyboardInput(SudokuCellWidget *cell, QKeyEvent *event)
{
    bool ok;
    int num = event->text().toInt(&ok);

    if(ok && num >= 0 && num <= mGrid->SizeGet())
    {
        if(cell->RegionIdGet() == num)
        {
            SetRegionIdOfCell(cell, 0);
        }
        else
        {
            SetRegionIdOfCell(cell, num);
        }
    }
    else if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        SetRegionIdOfCell(cell, 0);
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
        int count = mRegions.at(index).size();
        QString text = kCounterLabelText;
        if(count == mGrid->SizeGet())
        {
            text = "<span style='color:green;'>" + text + "</span>";
        }
        mCellCounters[index]->setText(QString(text).arg(QString::number(regionId), QString::number(count)));
    }
}
