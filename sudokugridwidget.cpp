#include "sudokugridwidget.h"
#include "sudokucellwidget.h"

SudokuGridWidget::SudokuGridWidget(unsigned short size, MainWindowContent* mainWindowContent, QWidget *parent)
    : QFrame{parent},
      mSize(size),
      mMainWindowContent(mainWindowContent),
      mGridLayout(new QGridLayout(this)),
      mGrid()
{
    this->setLayout(mGridLayout);
    this->setFrameStyle(QFrame::Box);
    this->setLineWidth(3);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed,QSizePolicy::Policy::Fixed));

    mGrid.reserve(mSize);
    for(unsigned short i = 0; i < mSize; ++i)
    {
        mGrid.push_back(QVector<SudokuCellWidget*>());
        mGrid.back().reserve(mSize);
        for(unsigned short j = 0; j < mSize; ++j)
        {
            SudokuCellWidget* cell = new SudokuCellWidget();
            mGrid.back().push_back(cell);
            mGridLayout->addWidget(cell, i, j);
        }
    }

    mGridLayout->setSpacing(0);
    mGridLayout->setContentsMargins(0, 0, 0, 0);
}

QSize SudokuGridWidget::sizeHint() const
{
    return mGridLayout->sizeHint();
}

QSize SudokuGridWidget::minimumSizeHint() const
{
    return mGridLayout->minimumSize();
}

unsigned short SudokuGridWidget::SizeGet()
{
    return mSize;
}
