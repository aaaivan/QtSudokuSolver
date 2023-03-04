#ifndef KILLERCAGEWIDGET_H
#define KILLERCAGEWIDGET_H

#include <QLabel>
#include <QPainter>

class SudokuCellWidget;

class KillerCageWidget : public QLabel
{
    Q_OBJECT
public:
    explicit KillerCageWidget(unsigned short maxCageSize, int cellLength, SudokuCellWidget* firstCell, QWidget *parent = nullptr);

private:
    enum Direction: int
    {
        DirectionTop,
        DirectionRight,
        DirectionBottom,
        DirectionLeft,

        TOTAL_DIRECTIONS
    };

    int mCellLength;
    int mPadding;
    int mLineWidth;
    unsigned short mMinX;
    unsigned short mMinY;
    unsigned short mMaxCageSize;
    unsigned int mCageTotal;
    QList<SudokuCellWidget*> mCells;
    QSet<SudokuCellWidget*> mAdjacentCells;
    QSet<SudokuCellWidget*> mRemovableCells;

    void UpdatePicture();
    QPoint PointMultiplierGet(Direction dir) const;
    void CalculateMinCol();
    void CalculateMinRow();
    void CalculateAdjacentCells();

    void CalculateRemovableCells();
    bool CanCellBeRemoved(SudokuCellWidget* cell) const;
    bool AreCellsInGageConnected(SudokuCellWidget* c1, SudokuCellWidget* c2, SudokuCellWidget* avoidCell) const;
    bool AreCellsInCageConnected_Inner(SudokuCellWidget* c1, SudokuCellWidget* c2, QVector<SudokuCellWidget*> &visited) const;

    void CalculatedEdges(QList<QPair<SudokuCellWidget*, Direction>>& outEdges) const;
    void DrawEdges(QPainter& painter, QList<QPair<SudokuCellWidget*, Direction>>& edges, int& fromIndex) const;
public:
    void AddCellToCage(SudokuCellWidget* cell);
    void RemoveCellFromCage(SudokuCellWidget* cell);
};

#endif // KILLERCAGEWIDGET_H
