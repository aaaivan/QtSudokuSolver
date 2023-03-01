#ifndef SUDOKUCELLWIDGET_H
#define SUDOKUCELLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QStackedWidget>

class SudokuCellWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SudokuCellWidget(QWidget *parent = nullptr);
    void RefreshLayout();

private:
    enum ContentType
    {
        GivenDigit,
        SolvedDigit,
        CellOptions
    };
    enum Edges: int
    {
        None = 0x0,
        TopEdge = 0x1,
        RightEdge = 0x2,
        BottomEdge = 0x4,
        LeftEdge = 0x8,
    };
    enum CellView: int
    {
        Options,
        Value,
        RegionId
    };

    int mLength;

    QStackedLayout* mOverlayLayout;
    QStackedWidget* mStackedContent;
    QPushButton* mOptionsLabel;
    QPushButton* mValueLabel;
    QPushButton* mRegionIdLabel;
    QLabel* mGraphicsOverlay;

    QString mContentString;
    ContentType mContentType;

    int mRegionId;
    bool mValueDirty;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    QString GetEdgeName(Edges edge) const;
    QString CreateStylesheet() const;

    // Styling variables
    bool mStyleDirty;
    Edges mBoldEdges;
    QString mBGColour;
    QString mFocusBGColour;

public:
    void ShowRegionNumber();
    void HideRegionNumber();
};

#endif // SUDOKUCELLWIDGET_H
