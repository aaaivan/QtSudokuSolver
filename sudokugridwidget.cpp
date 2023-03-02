#include "sudokugridwidget.h"
#include "sudokucellwidget.h"
#include "puzzledata.h"

SudokuGridWidget::SudokuGridWidget(unsigned short size, MainWindowContent* mainWindowContent, QWidget *parent)
    : QFrame{parent},
      mSize(size),
      mMainWindowContent(mainWindowContent),
      mGridLayout(new QGridLayout(this)),
      mCells(),
      mPuzzleData(std::make_shared<PuzzleData>(size))
{
    this->setLayout(mGridLayout);
    this->setFrameStyle(QFrame::Box);
    this->setLineWidth(3);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed,QSizePolicy::Policy::Fixed));

    mCells.reserve(mSize);
    for(unsigned short i = 0; i < mSize; ++i)
    {
        mCells.push_back(QVector<SudokuCellWidget*>());
        mCells.back().reserve(mSize);
        for(unsigned short j = 0; j < mSize; ++j)
        {
            SudokuCellWidget* cell = new SudokuCellWidget(i, j, mSize, mMainWindowContent);
            mCells.back().push_back(cell);
            mGridLayout->addWidget(cell, i, j);
        }
    }
    for(unsigned short i = 0; i < mCells.size(); ++i)
    {
        for(unsigned short j = 0; j < mCells[i].size(); ++j)
        {
            SudokuCellWidget* cell = mCells[i][j];
            SudokuCellWidget* top = i-1 >= 0 ? mCells[i-1][j] : nullptr;
            SudokuCellWidget* right = j+1 < mCells[i].size() ? mCells[i][j+1] : nullptr;
            SudokuCellWidget* btm = i+1 < mCells.size() ? mCells[i+1][j] : nullptr;
            SudokuCellWidget* left = j-1 >= 0 ? mCells[i][j-1] : nullptr;
            cell->NeighboursSet(top, right, btm, left);
        }
    }

    mGridLayout->setSpacing(0);
    mGridLayout->setContentsMargins(0, 0, 0, 0);

    // styling
    this->setStyleSheet("SudokuGridWidget{background-color: white;}");
}

QSize SudokuGridWidget::sizeHint() const
{
    return mGridLayout->sizeHint();
}

QSize SudokuGridWidget::minimumSizeHint() const
{
    return mGridLayout->minimumSize();
}

void SudokuGridWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen(Qt::GlobalColor::lightGray);
    pen.setWidth(3);
    painter.setPen(pen);

    if(mPuzzleData->HasNegativeDiagonalConstraint())
    {
        painter.drawLine(0, 0, sizeHint().width(), sizeHint().height());
    }
    if(mPuzzleData->HasPositiveDiagonalConstraint())
    {
        painter.drawLine(sizeHint().width(), 0, 0, sizeHint().height());
    }

    QFrame::paintEvent(event);
}

unsigned short SudokuGridWidget::SizeGet() const
{
    return mSize;
}

const QVector<QVector<SudokuCellWidget *> > &SudokuGridWidget::CellsGet() const
{
    return mCells;
}

PuzzleData *SudokuGridWidget::PuzzleDataGet() const
{
    return mPuzzleData.get();
}

void SudokuGridWidget::SwitchView(MainWindowContent::ViewType view)
{
    for(const auto& vect : mCells)
    {
        for(const auto& c : vect)
        {
            c->SwitchView(view);
        }
    }
}
