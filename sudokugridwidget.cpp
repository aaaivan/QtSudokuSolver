#include "sudokugridwidget.h"
#include "sudokucellwidget.h"
#include "gridgraphicaloverlay.h"
#include "mainwindowcontent.h"
#include <QStackedLayout>
#include <QPainter>

SudokuGridWidget::SudokuGridWidget(unsigned short size, MainWindowContent* mainWindowContent, QWidget *parent)
    : QFrame{parent},
      mCellLength(50),
      mSize(size),
      mCells(),
      mMainWindowContent(mainWindowContent),
      mGraphicalOverlay(new GridGraphicalOverlay(this, mCellLength)),
      mSolver(std::make_unique<SudokuSolverThread>(size, this))
{
    // build stacked layout
    QStackedLayout* stackedLayout = new QStackedLayout(this);
    this->setLayout(stackedLayout);
    stackedLayout->setStackingMode(QStackedLayout::StackingMode::StackAll);
    QWidget* grid = new QWidget();
    stackedLayout->addWidget(mGraphicalOverlay);
    stackedLayout->addWidget(grid);

    // build grid
    QGridLayout* gridLayout = new QGridLayout();
    grid->setLayout(gridLayout);

    mCells.reserve(mSize);
    for(unsigned short row = 0; row < mSize; ++row)
    {
        mCells.push_back(QVector<SudokuCellWidget*>());
        mCells.back().reserve(mSize);
        for(unsigned short col = 0; col < mSize; ++col)
        {
            SudokuCellWidget* cell = new SudokuCellWidget(row, col, mSize, mCellLength, mMainWindowContent);
            mCells.back().push_back(cell);
            gridLayout->addWidget(cell, row, col);
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

    // Styling
    this->setFrameStyle(QFrame::Box);
    this->setLineWidth(3);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed,QSizePolicy::Policy::Fixed));
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    this->setStyleSheet("SudokuGridWidget{background-color: white;}");

    // Solver update event
    connect(mSolver.get(), &SudokuSolverThread::CellUpdated, this, &SudokuGridWidget::UpdateOptionsOfCell);
    mSolver->Init();
}

SudokuGridWidget::~SudokuGridWidget()
{
    delete mGraphicalOverlay;
    mGraphicalOverlay = nullptr;
}

void SudokuGridWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen(Qt::GlobalColor::lightGray);
    pen.setWidth(3);
    painter.setPen(pen);

    if(SolverGet()->HasNegativeDiagonalConstraint())
    {
        painter.drawLine(0, 0, sizeHint().width(), sizeHint().height());
    }
    if(SolverGet()->HasPositiveDiagonalConstraint())
    {
        painter.drawLine(sizeHint().width(), 0, 0, sizeHint().height());
    }

    QFrame::paintEvent(event);
}

void SudokuGridWidget::UpdateOptionsOfCell(unsigned short id, const std::set<unsigned short> &content)
{
    unsigned short row = id / mSize;
    unsigned short col = id % mSize;
    mCells[row][col]->UpdateOptions(content);
}

unsigned short SudokuGridWidget::CellLengthGet() const
{
    return mCellLength;
}

unsigned short SudokuGridWidget::SizeGet() const
{
    return mSize;
}

const QVector<QVector<SudokuCellWidget *> > &SudokuGridWidget::CellsGet() const
{
    return mCells;
}

SudokuSolverThread *SudokuGridWidget::SolverGet() const
{
    return mSolver.get();
}

GridGraphicalOverlay *SudokuGridWidget::GraphicalOverlayGet() const
{
    return mGraphicalOverlay;
}

void SudokuGridWidget::SwitchView(size_t view)
{
    for(const auto& vect : mCells)
    {
        for(const auto& c : vect)
        {
            c->SwitchView(view);
        }
    }
}
