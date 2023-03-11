#ifndef SUDOKUCELLWIDGET_H
#define SUDOKUCELLWIDGET_H

#include <QFrame>
#include <set>

class MainWindowContent;
class CellContentButton;
class CellRegionIdButton;
class VariantClueWidget;
class QLabel;
class QStackedLayout;
class QStackedWidget;

class SudokuCellWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SudokuCellWidget(unsigned short x, unsigned short y, unsigned short gridSize, int cellLength,
                              MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private:
    enum ContentType
    {
        GivenDigit,
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
        RegionId,

        MAX_VIEWS
    };

    unsigned short mCol;
    unsigned short mRow;
    unsigned short mGridSize;
    unsigned short mId;
    bool mIsSolved;

    MainWindowContent* mMainWindowContent;
    QList<SudokuCellWidget*> mNeighbours;
    QSet<VariantClueWidget*> mVariantClues;

    QStackedLayout* mOverlayLayout;
    QStackedWidget* mStackedContent;
    CellContentButton* mOptionsLabel;
    CellContentButton* mValueLabel;
    CellContentButton* mRegionIdLabel;
    QLabel* mGraphicsOverlay;

    QString mContentString;
    ContentType mContentType;

    unsigned short mRegionId;

    // Styling variables
    const int mLength;
    CellEdge mBoldEdges;
    bool mHighlighted;

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

public:
    // public const functions
    MainWindowContent* MainWindowContentGet() const;
    unsigned short CellIdGet() const;
    unsigned short ColGet() const;
    unsigned short RowGet() const;
    unsigned short RegionIdGet() const;
    const QList<SudokuCellWidget*>& NeighboursGet() const;
    const QSet<VariantClueWidget*>& VariantCluesGet() const;

    // public non-const functions
    void SwitchView(size_t view);
    void NeighboursSet(SudokuCellWidget* top, SudokuCellWidget* right, SudokuCellWidget* btm, SudokuCellWidget* left);
    void SetHighlighted(bool highlight);
    void ResetRegionId();
    void SetRegionId(unsigned short newId);
    void SetGivenDigit(unsigned short value);
    void RemoveGivenDigit();
    void AddVariantClue(VariantClueWidget* clue);
    void RemoveVariantClue(VariantClueWidget* clue);
    void UpdateOptions(const std::set<unsigned short> &options);
};

#endif // SUDOKUCELLWIDGET_H
