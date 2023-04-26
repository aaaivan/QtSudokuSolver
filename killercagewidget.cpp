#include "killercagewidget.h"
#include "sudokucellwidget.h"
#include "drawkillerscontextmenu.h"
#include "variantclueslayer.h"
#include "sudokugridwidget.h"
#include "sudokusolverthread.h"
#include <QPicture>
#include <QPainterPath>
#include <QPainter>

constexpr Qt::GlobalColor kHighlightColour = Qt::green;
constexpr float kHighlightOpacity = 0.2f;
constexpr int kLineWidth = 2;

KillerCageWidget::KillerCageWidget(unsigned short maxCageSize, int cellLength, SudokuCellWidget* firstCell, unsigned int total,
                                   SudokuGridWidget* grid, DrawKillersContextMenu* killerContextMenu, QWidget *parent)
    : QLabel{parent},
      VariantClueWidget(cellLength, grid, killerContextMenu),
      mPadding(cellLength/10.0),
      mHighlighted(false),
      mTotalLabel(new QLabel(this)),
      mMinX(firstCell->ColGet()),
      mMinY(firstCell->RowGet()),
      mMaxCageSize(maxCageSize),
      mCageTotal(),
      mAdjacentCells(),
      mRemovableCells()
{
    this->setContentsMargins(mPadding - kLineWidth/2, mPadding - kLineWidth/2, mPadding - kLineWidth/2, mPadding - kLineWidth/2);
    mTotalLabel->setStyleSheet("QLabel{background-color: rgba(255, 255, 255, 0.9);}");
    KillerCageWidget::AddCell(firstCell);
    CageTotalSet(total);
}

KillerCageWidget::~KillerCageWidget()
{
    if(mCells.size() > 0)
    {
        mGrid->SolverGet()->RemoveKillerCage(mCells[0]->CellIdGet());
    }
    for(const auto& c : mCells)
    {
        c->RemoveVariantClue(this);
    }
}

void KillerCageWidget::UpdatePicture()
{
    // pen setup
    QPicture pic;
    QPainter painter(&pic);
    double penWidth = kLineWidth;
    QPen pen(Qt::GlobalColor::black, penWidth, Qt::CustomDashLine, Qt::FlatCap);
    QVector<qreal> dashes;
    dashes << 2 << 2;
    pen.setDashPattern(dashes);
    pen.setColor(QColor(80, 80, 80));
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::RenderHint::LosslessImageRendering);

    if(mHighlighted)
    {
        QColor col = kHighlightColour;
        col.setAlphaF(kHighlightOpacity);
        painter.setBrush( col );
    }

    // draw
    QPainterPath path;
    QList<QPair<SudokuCellWidget*, Direction>> edges;
    CalculatedEdges(edges);
    int index = 0;
    while (index < edges.size())
    {
        DrawEdges(path, edges, index);
    }
    painter.drawPath(path);
    painter.end();

    // add picture to label
    this->setPicture(pic);
    this->adjustSize();
    this->move(mMinX * mCellLength, mMinY * mCellLength);
    UpdateLabel();
}

void KillerCageWidget::UpdateLabel()
{
    mTotalLabel->setText(QString::number(mCageTotal));
    mTotalLabel->adjustSize();
    mTotalLabel->move((mCells[0]->ColGet() - mMinX) * mCellLength + 1,(mCells[0]->RowGet() - mMinY) * mCellLength + 1);
}

QPoint KillerCageWidget::PointMultiplierGet(Direction dir) const
{
    switch (dir) {
    case DirectionTop:
        return QPoint(0, -1);
    case DirectionRight:
        return QPoint(1, 0);
    case DirectionBottom:
        return QPoint(0, 1);
    case DirectionLeft:
        return QPoint(-1, 0);
    default:
        return QPoint(0, 0);
    }
}

void KillerCageWidget::CalculateMinCol()
{
    unsigned short result = mMaxCageSize;
    for (const auto& c : mCells)
    {
        result = std::min(result, c->ColGet());
    }
    mMinX = result;
}

void KillerCageWidget::CalculateMinRow()
{
    mMinY = mCells.size() > 0 ? mCells[0]->RowGet() : mMaxCageSize;
}

void KillerCageWidget::CalculateAdjacentCells()
{
    mAdjacentCells.clear();
    for (const auto& cell : mCells)
    {
        for(const auto& c : cell->NeighboursGet())
        {
            if(!mCells.contains(c))
            {
                mAdjacentCells.insert(c);
            }
        }
    }
}

void KillerCageWidget::CalculateRemovableCells()
{
    mRemovableCells.clear();
    for (const auto& c : mCells) {
        if(CanCellBeRemoved(c))
        {
            mRemovableCells.insert(c);
        }
    }
}

bool KillerCageWidget::CanCellBeRemoved(SudokuCellWidget *cell) const
{
    QVector<SudokuCellWidget*> adjacentCellsInCage;
    for (const auto& c : cell->NeighboursGet())
    {
        if(mCells.contains(c))
        {
            adjacentCellsInCage.push_back(c);
        }
    }
    if(adjacentCellsInCage.size() <= 1)
    {
        return true;
    }

    for (int i = 1; i < adjacentCellsInCage.size(); ++i) {
        if(!AreCellsInGageConnected(adjacentCellsInCage[i], adjacentCellsInCage[i-1], cell))
        {
            return false;
        }
    }

    return true;
}

bool KillerCageWidget::AreCellsInGageConnected(SudokuCellWidget *c1, SudokuCellWidget *c2, SudokuCellWidget *avoidCell) const
{
    QVector<SudokuCellWidget*> visited{avoidCell};
    return AreCellsInCageConnected_Inner(c1, c2, visited);
}

bool KillerCageWidget::AreCellsInCageConnected_Inner(SudokuCellWidget *c1, SudokuCellWidget *c2, QVector<SudokuCellWidget *> &visited) const
{
    visited.push_back(c1);
    if(c1 == c2)
    {
        return true;
    }

    for (const auto& n : c1->NeighboursGet()) {
        if(visited.contains(n) || !mCells.contains(n)) { continue; }
        if(AreCellsInCageConnected_Inner(n, c2, visited)) { return true; }
    }

    return false;
}

void KillerCageWidget::CalculatedEdges(QList<QPair<SudokuCellWidget*, Direction>>& outEdges) const
{
    for (const auto& c : mCells)
    {
        for(int i = DirectionTop; i < TOTAL_DIRECTIONS; ++i)
        {
            const auto n = c->NeighboursGet()[i];
            if(mCells.contains(n)) { continue; }

            Direction d = static_cast<Direction>((i + 1) % TOTAL_DIRECTIONS);
            outEdges.emplace_back(c, d);
        }
    }
}

void KillerCageWidget::DrawEdges(QPainterPath& path, QList<QPair<SudokuCellWidget*, Direction>>& edges, int& fromIndex) const
{
    const int inLen = mCellLength - (2 * mPadding);
    const int outLen = 2 * mPadding;
    const Direction startDir = static_cast<Direction>(edges[fromIndex].second);

    SudokuCellWidget* cell = edges[fromIndex].first;
    Direction dir = startDir;
    QPoint startPoint(cell->ColGet() * mCellLength + mPadding, cell->RowGet() * mCellLength + mPadding);
    for (int i = DirectionRight; i != startDir; i = (i + 1) % TOTAL_DIRECTIONS)
    {
        startPoint += PointMultiplierGet(static_cast<Direction>(i)) * inLen;
    }

    QPoint nextPoint = startPoint + PointMultiplierGet(dir) * inLen;
    bool lineInCell = true;
    ++fromIndex;

    QPolygon polygon;
    polygon << startPoint;

    while (nextPoint != startPoint)
    {
        Direction newDir = dir;
        if(lineInCell)
        {
            SudokuCellWidget* nextCell = cell->NeighboursGet()[dir];
            if(nextCell && mCells.contains(nextCell))
            {
                lineInCell = false;
                cell = nextCell;
            }
            else
            {
                newDir = static_cast<Direction>((dir + 1) % TOTAL_DIRECTIONS);
                QPair<SudokuCellWidget*, Direction> edge(cell, static_cast<Direction>(newDir));
                edges.swapItemsAt(fromIndex, edges.indexOf(edge, fromIndex));
                ++fromIndex;
            }
        }
        else
        {
            SudokuCellWidget* nextCell = cell->NeighboursGet()[(dir + TOTAL_DIRECTIONS - 1) % TOTAL_DIRECTIONS];
            if(nextCell && mCells.contains(nextCell))
            {
                cell = nextCell;
                newDir = static_cast<Direction>((dir - 1 + TOTAL_DIRECTIONS) % TOTAL_DIRECTIONS);
            }
            else
            {
                QPair<SudokuCellWidget*, Direction> edge(cell, static_cast<Direction>(newDir));
                edges.swapItemsAt(fromIndex, edges.indexOf(edge, fromIndex));
                ++fromIndex;
                lineInCell = true;
            }
        }

        if(newDir != dir)
        {
            polygon << nextPoint;
        }
        dir = newDir;
        int length = lineInCell ? inLen : outLen;
        nextPoint += PointMultiplierGet(newDir) * length;
    }

    if(dir == startDir)
    {
        polygon.remove(0);
    }
    polygon << polygon[0];

    QPainterPath newPath;
    newPath.addPolygon(polygon);
    path.addPath(newPath);
}

unsigned int KillerCageWidget::CageTotalGet() const
{
    return mCageTotal;
}

CellCoord KillerCageWidget::CageIdGet() const
{
    CellCoord result = -1;
    if(mCells.size() > 0)
    {
        result = mCells.at(0)->CellIdGet();
    }

    return result;
}

void KillerCageWidget::AddCell(SudokuCellWidget *cell)
{
    if(mCells.size() >= mMaxCageSize)
        return;

    if( mCells.size() == 0 ||
       (!mCells.contains(cell) && mAdjacentCells.contains(cell)))
    {
        mCells.push_back(cell);
        cell->AddVariantClue(this);
        mMinX = std::min(mMinX, cell->ColGet());
        mMinY = std::min(mMinY, cell->RowGet());

        CalculateAdjacentCells();
        CalculateRemovableCells();

        std::sort(mCells.begin(), mCells.end(), [](const SudokuCellWidget *a, const SudokuCellWidget *b)
            {return a->CellIdGet() < b->CellIdGet();});

        UpdatePicture();
    }
}

void KillerCageWidget::AddDisconnectedCell(SudokuCellWidget *cell)
{
    if(mCells.size() >= mMaxCageSize)
        return;

    if(!mCells.contains(cell))
    {
        mCells.push_back(cell);
        cell->AddVariantClue(this);
        mMinX = std::min(mMinX, cell->ColGet());
        mMinY = std::min(mMinY, cell->RowGet());

        CalculateAdjacentCells();
        CalculateRemovableCells();

        std::sort(mCells.begin(), mCells.end(), [](const SudokuCellWidget *a, const SudokuCellWidget *b)
            {return a->CellIdGet() < b->CellIdGet();});

        UpdatePicture();
    }
}

void KillerCageWidget::RemoveCell(SudokuCellWidget *cell)
{
    if(mRemovableCells.contains(cell))
    {
        if(mCells.size() > 1)
        {
            int index = mCells.indexOf(cell);
            mCells.removeAt(index);
            cell->RemoveVariantClue(this);

            CalculateMinCol();
            CalculateMinRow();
            CalculateAdjacentCells();
            CalculateRemovableCells();
            UpdatePicture();
        }
        else
        {
            mGrid->VariantCluesLayerGet()->RemoveOverlayComponent(this);
        }

    }
}

void KillerCageWidget::ClueDidGetActive()
{
    SetHighlighted(true);
}

void KillerCageWidget::ClueDidGetInactive()
{
    SetHighlighted(false);
}

void KillerCageWidget::SetHighlighted(bool highlight)
{
    if(mHighlighted != highlight)
    {
        mHighlighted = highlight;
        UpdatePicture();
    }
}

void KillerCageWidget::CageTotalSet(unsigned int total)
{
    if(mCageTotal != total)
    {
        mCageTotal = total;
        UpdateLabel();
    }
}
