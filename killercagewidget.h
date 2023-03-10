#ifndef KILLERCAGEWIDGET_H
#define KILLERCAGEWIDGET_H

#include "variantcluewidget.h"
#include <QLabel>

class SudokuCellWidget;
class DrawKillersControls;
class QPainter;

class KillerCageWidget : public QLabel, public VariantClueWidget
{
    Q_OBJECT
public:
    explicit KillerCageWidget(unsigned short maxCageSize, int cellLength, SudokuCellWidget* firstCell, unsigned int total,
                              SudokuGridWidget* grid, DrawKillersControls* killerContextMenu, QWidget *parent = nullptr);
    virtual ~KillerCageWidget();

private:
    enum Direction: int
    {
        DirectionTop,
        DirectionRight,
        DirectionBottom,
        DirectionLeft,

        TOTAL_DIRECTIONS
    };

    //styling variables
    int mPadding;
    bool mHighlighted;
    QLabel* mTotalLabel;

    unsigned short mMinX;
    unsigned short mMinY;
    unsigned short mMaxCageSize;
    unsigned int mCageTotal;
    QSet<SudokuCellWidget*> mAdjacentCells;
    QSet<SudokuCellWidget*> mRemovableCells;

    void UpdatePicture();
    void UpdateLabel();
    QPoint PointMultiplierGet(Direction dir) const;
    void CalculateMinCol();
    void CalculateMinRow();
    void CalculateAdjacentCells();

    void CalculateRemovableCells();
    bool CanCellBeRemoved(SudokuCellWidget* cell) const;
    bool AreCellsInGageConnected(SudokuCellWidget* c1, SudokuCellWidget* c2, SudokuCellWidget* avoidCell) const;
    bool AreCellsInCageConnected_Inner(SudokuCellWidget* c1, SudokuCellWidget* c2, QVector<SudokuCellWidget*> &visited) const;

    void CalculatedEdges(QList<QPair<SudokuCellWidget*, Direction>>& outEdges) const;
    void DrawEdges(QPainterPath& path, QList<QPair<SudokuCellWidget*, Direction>>& edges, int& fromIndex) const;
public:
    unsigned int CageTotalGet() const;

    void AddCell(SudokuCellWidget* cell) override;
    void RemoveCell(SudokuCellWidget* cell) override;
    void ClueDidGetActive() override;
    void ClueDidGetInactive() override;

    void SetHighlighted(bool set);
    void CageTotalSet(unsigned int total);
};

#endif // KILLERCAGEWIDGET_H
