#ifndef SUDOKUCELLWIDGET_H
#define SUDOKUCELLWIDGET_H

#include "mainwindowcontent.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QStackedWidget>

class CellContentButton;

class SudokuCellWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SudokuCellWidget(unsigned short x, unsigned short y, unsigned short gridSize,
                              MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private:
    enum ContentType
    {
        GivenDigit,
        SolvedDigit,
        CellOptions
    };
    enum CellEdge: int
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

    unsigned short mX;
    unsigned short mY;
    unsigned short mGridSize;

    MainWindowContent* mMainWindowContent;
    QList<SudokuCellWidget*> mNeighbours;

    QStackedLayout* mOverlayLayout;
    QStackedWidget* mStackedContent;
    CellContentButton* mOptionsLabel;
    CellContentButton* mValueLabel;
    QPushButton* mRegionIdLabel;
    QLabel* mGraphicsOverlay;

    QString mContentString;
    ContentType mContentType;

    unsigned short mRegionId;

    // Styling variables
    int mLength;
    bool mStyleDirty;
    bool mOptionsLabelStyleDirty;
    bool mRegionLabelStyleDirty;
    bool mValueLabelStyleDirty;
    CellEdge mBoldEdges;
    QString mBGColour;
    QString mFocusBGColour;
    bool mRegionLabelHighlighted;

    // base class overrides
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // private const functions
    QString EdgeNameGet(CellEdge edge) const;
    CellEdge OppositeEdgeGet(CellEdge oppositeTo) const;
    QString CreateStylesheet() const;
    QString CreateOptionsLabelStylesheet() const;
    QString CreateValueLabelStylesheet() const;
    QString CreateRegionLabelStylesheet() const;

    // private non-cont functions
    void RefreshLayout();
    void ShowRegionNumber(bool show);
    void SetEdgeWeight(CellEdge edge, bool bold);
    void UpdateRegionId(unsigned short newId);

private slots:
    void RegionIdLabel_OnClicked();

public:
    // public const functions
    unsigned short RegionIdGet();

    // public non-const functions
    void SwitchView(MainWindowContent::ViewType view);
    void NeighboursSet(SudokuCellWidget* top, SudokuCellWidget* right, SudokuCellWidget* btm, SudokuCellWidget* left);
    void HighlightRegionLabel(bool highlight);
    void ResetRegionId();
    void SetGivenDigit(unsigned short value);
    void RemoveGivenDigit();
    void SetSolvedDigit(unsigned short value);
};

#endif // SUDOKUCELLWIDGET_H
