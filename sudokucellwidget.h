#ifndef SUDOKUCELLWIDGET_H
#define SUDOKUCELLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QStackedLayout>
#include <QStackedWidget>

class SudokuCellWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SudokuCellWidget(QWidget *parent = nullptr);
    void RefreshLayout();

private:
    enum CellType
    {
        Given,
        Solved,
        Unsolved
    };
    enum Edges: int
    {
        None = 0x0,
        TopEdge = 0x1,
        RightEdge = 0x2,
        BottomEdge = 0x4,
        LeftEdge = 0x8,
    };

    int mLength;

    QStackedLayout* mOverlayLayout;
    QStackedWidget* mStackedWidget;
    QLabel* mOptions;
    QLabel* mValue;
    QLabel* mGraphicsOverlay;

    QString mContentString;
    Edges mBoldEdges;
    CellType mCellType;

    int mRegionNumber;
    bool mBorderDirty;
    bool mContentDirty;

    QString GetEdgeName(Edges edge);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
};

#endif // SUDOKUCELLWIDGET_H
