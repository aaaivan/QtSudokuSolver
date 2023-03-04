#include "sudokucellwidget.h"
#include "drawregionscontrols.h"
#include "puzzledata.h"
#include "sudokugridwidget.h"
#include "cellcontentbutton.h"
#include "cellregionidbutton.h"

SudokuCellWidget::SudokuCellWidget(unsigned short row, unsigned short col, unsigned short gridSize,int cellLength,
                                   MainWindowContent* mainWindowContent, QWidget *parent)
    : QFrame{parent},
      mCol(col),
      mRow(row),
      mGridSize(gridSize),
      mId(row * gridSize + col),
      mMainWindowContent(mainWindowContent),
      mNeighbours(4, nullptr),
      mOverlayLayout(new QStackedLayout(this)),
      mStackedContent(new QStackedWidget()),
      mOptionsLabel(new CellContentButton(gridSize, this)),
      mValueLabel(new CellContentButton(gridSize, this)),
      mRegionIdLabel(new CellRegionIdButton(gridSize, this)),
      mGraphicsOverlay(new QLabel()),
      mContentString("12345\n6789"),
      mContentType(ContentType::CellOptions),
      mRegionId(0),
      mLength(cellLength),
      mStyleDirty(true),
      mOptionsLabelStyleDirty(true),
      mRegionLabelStyleDirty(true),
      mValueLabelStyleDirty(true),
      mBoldEdges(None),
      mBGColour("transparent"),
      mFocusBGColour("rgba(255, 255, 0, 0.4)"),
      mRegionLabelHighlighted(false)
{
    this->setObjectName("cell");

    // build stacked layout of the cell
    mOverlayLayout->setStackingMode(QStackedLayout::StackingMode::StackAll);
    this->setLayout(mOverlayLayout);
    mOverlayLayout->addWidget(mGraphicsOverlay);
    mOverlayLayout->addWidget(mStackedContent);

    // set overlay graphics
    mGraphicsOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    //mGraphicsOverlay->setPixmap(QPixmap(":/assets/assets/TestBorder.png").scaled(40,40));

    // build stacked layout for the different contents of the cell
    mStackedContent->insertWidget(CellView::Options, mOptionsLabel);
    mStackedContent->insertWidget(CellView::Value, mValueLabel);
    mStackedContent->insertWidget(CellView::RegionId, mRegionIdLabel);

    // set text in labels
    mOptionsLabel->setText(mContentString);
    mValueLabel->setText(mContentString);
    mRegionIdLabel->setText(mRegionId ? QString::number(mRegionId) : "-");

    // text font
    mOptionsLabel->setFont(QFont( mOptionsLabel->font().family(), 8));
    mValueLabel->setFont(QFont(mValueLabel->font().family(),20, 500));
    mRegionIdLabel->setFont(QFont(mValueLabel->font().family(),20, 500));

    // borders and sizes
    mValueLabel->setFlat(true);

    // events
    connect(mRegionIdLabel, SIGNAL(clicked(bool)), this, SLOT(RegionIdLabel_OnClicked()));
}

QSize SudokuCellWidget::sizeHint() const
{
    return QSize(mLength, mLength);
}

QSize SudokuCellWidget::minimumSizeHint() const
{
    return QSize(mLength, mLength);
}

QString SudokuCellWidget::EdgeNameGet(CellEdge edge) const
{
    switch (edge) {
    case CellEdge::TopEdge:
        return "top";
    case CellEdge::RightEdge:
        return "right";
    case CellEdge::BottomEdge:
        return "bottom";
    case CellEdge::LeftEdge:
        return "left";
    default:
        return "";
    }
}

void SudokuCellWidget::RefreshLayout()
{
    if(mStyleDirty)
    {
        mStyleDirty = false;
        this->setStyleSheet(CreateStylesheet());
    }

    if(mOptionsLabelStyleDirty)
    {
        mOptionsLabelStyleDirty = false;
        mOptionsLabel->setStyleSheet(CreateOptionsLabelStylesheet());
    }

    if(mRegionLabelStyleDirty)
    {
        mRegionLabelStyleDirty = false;
        mRegionIdLabel->setStyleSheet(CreateRegionLabelStylesheet());
    }

    // options style
    if(mValueLabelStyleDirty)
    {
        mValueLabelStyleDirty = false;
        mValueLabel->setStyleSheet(CreateValueLabelStylesheet());
    }
}

QString SudokuCellWidget::CreateStylesheet() const
{
    // normal styling
    QString style = "SudokuCellWidget{\n";
    // borders
    style += "border-width: 0px;\n";
    for(int border = CellEdge::RightEdge, i = 1; border > 0; border = border >> 1, --i)
    {
        if(mNeighbours[i])
        {
            bool isBold = mBoldEdges & border;
            style += "border-" + EdgeNameGet(static_cast<CellEdge>(border)) + ": " +
                    (isBold ? "3" : "1") + "px solid black;\n";
        }
    }
    style += "background-color: " + mBGColour + ";\n";
    style += "}\n";

    return style;
}

QString SudokuCellWidget::CreateOptionsLabelStylesheet() const
{
    static const QString normalBG = "transparent";
    static const QString focusBG = "rgba(255, 200, 0, 0.2)";

    // normal styling
    QString style = "QPushButton{\n";
    style += "background-color: " + normalBG + ";\n";
    style += "border: 0px;\n";
    style += "color: grey;";
    style += "}\n";

    // focus styling
    style += "QPushButton:focus{\n";
    style += "background-color: " + focusBG + ";\n";
    style += "}\n";
    return style;
}

QString SudokuCellWidget::CreateValueLabelStylesheet() const
{
    static const QString normalBG = "transparent";
    static const QString focusBG = "rgba(255, 200, 0, 0.2)";

    bool isGiven = mContentType == ContentType::GivenDigit;
    QString textColour = isGiven ? "black" : "grey";
    // normal styling
    QString style = "QPushButton{\n";
    style += "background-color: " + normalBG + ";\n";
    style += "border: 0px;\n";
    style +="color: " + textColour + ";";
    style += "}\n";

    // focus styling
    style += "QPushButton:focus{\n";
    style += "background-color: " + focusBG + ";\n";
    style += "}\n";

    return style;
}

QString SudokuCellWidget::CreateRegionLabelStylesheet() const
{
    static const QString normalBG = "transparent";
    static const QString highlightBG = "rgba(0, 160, 200, 0.2)";
    static const QString focusBG = "rgba(255, 200, 0, 0.2)";
    static const QString textColor = "rgb(0, 0, 255)";

    // normal styling
    QString style = "QPushButton{\n";
    style += "background-color: " + (mRegionLabelHighlighted ? highlightBG : normalBG) + ";\n";
    style += "border: 0px;\n";
    style += "color: " + textColor + ";";
    style += "}\n";

    // focus styling
    style += "QPushButton:focus{\n";
    style += "background-color: " + focusBG + ";\n";
    style += "}\n";

    return style;
}

SudokuCellWidget::CellEdge SudokuCellWidget::OppositeEdgeGet(CellEdge oppositeTo) const
{
    switch (oppositeTo) {
    case CellEdge::TopEdge:
        return CellEdge::BottomEdge;
    case CellEdge::RightEdge:
        return CellEdge::LeftEdge;
    case CellEdge::BottomEdge:
        return CellEdge::TopEdge;
    case CellEdge::LeftEdge:
        return CellEdge::RightEdge;
    default:
        return CellEdge::None;
    }
}

void SudokuCellWidget::RegionIdLabel_OnClicked()
{
    unsigned short newId = mMainWindowContent->DrawRegionContextMenuGet()->SelectedRegionIdGet();
    if(newId == mRegionId)
    {
        newId = 0;
    }
    UpdateRegionId(newId);
}

unsigned short SudokuCellWidget::CellIdGet() const
{
    return mId;
}

unsigned short SudokuCellWidget::RowGet() const
{
    return mRow;
}

unsigned short SudokuCellWidget::ColGet() const
{
    return mCol;
}

unsigned short SudokuCellWidget::RegionIdGet() const
{
    return mRegionId;
}

const QList<SudokuCellWidget *> &SudokuCellWidget::NeighboursGet() const
{
    return mNeighbours;
}

void SudokuCellWidget::SwitchView(MainWindowContent::ViewType view)
{
    switch (view)
    {
    case MainWindowContent::ViewType::EnterDigits:
        ShowRegionNumber(false);
        break;
    case MainWindowContent::ViewType::DrawRegions:
        ShowRegionNumber(true);
        break;
    case MainWindowContent::ViewType::DrawKiller:
        ShowRegionNumber(false);
        break;
    default:
        break;
    }
}

void SudokuCellWidget::ShowRegionNumber(bool show)
{
    if(show)
    {
        mStackedContent->setCurrentIndex(CellView::RegionId);
    }
    else
    {
        CellView view = mContentType == ContentType::CellOptions ?
                        CellView::Options :
                        CellView::Value;
        mStackedContent->setCurrentIndex(view);
    }
}

void SudokuCellWidget::SetEdgeWeight(CellEdge edge, bool bold)
{
    if(((mBoldEdges & edge) && !bold) ||
       (!(mBoldEdges & edge) && bold))
    {
        if(bold)
        {
            mBoldEdges = static_cast<CellEdge>(edge | mBoldEdges);
        }
        else
        {
            mBoldEdges = static_cast<CellEdge>(~edge & mBoldEdges);
        }
        mStyleDirty = true;
    }
}

void SudokuCellWidget::UpdateRegionId(unsigned short newId)
{
    auto cellCount = mMainWindowContent->GridGet()->PuzzleDataGet()->CellCountInRegion(newId);
    auto puzzleSize = mMainWindowContent->GridGet()->SizeGet();
    if(cellCount >= puzzleSize)
    {
        return;
    }

    if(newId != mRegionId)
    {
        mMainWindowContent->GridGet()->PuzzleDataGet()->RemoveCellFromRegion(mRegionId, mCol, mRow);
        mMainWindowContent->GridGet()->PuzzleDataGet()->AddCellToRegion(newId, mCol, mRow);
        mMainWindowContent->DrawRegionContextMenuGet()->UpdateCellCounters(mRegionId);
        mMainWindowContent->DrawRegionContextMenuGet()->UpdateCellCounters(newId);
        mRegionIdLabel->setText(newId ? QString::number(newId) : "-");
        mRegionId = newId;

        int edge = 1;
        for(const auto& c : mNeighbours)
        {
            if(c)
            {
                SetEdgeWeight(static_cast<CellEdge>(edge), c->RegionIdGet() != mRegionId);
                c->SetEdgeWeight(OppositeEdgeGet(static_cast<CellEdge>(edge)), c->RegionIdGet() != mRegionId);
                c->RefreshLayout();
            }
            edge = edge << 1;
        }
        mRegionLabelHighlighted = mRegionId == mMainWindowContent->DrawRegionContextMenuGet()->SelectedRegionIdGet();
        mRegionLabelStyleDirty = true;
        RefreshLayout();
    }
}

void SudokuCellWidget::NeighboursSet(SudokuCellWidget *top, SudokuCellWidget *right, SudokuCellWidget *btm, SudokuCellWidget *left)
{
    mNeighbours[0] = top;
    mNeighbours[1] = right;
    mNeighbours[2] = btm;
    mNeighbours[3] = left;
    RefreshLayout();
}

void SudokuCellWidget::HighlightRegionLabel(bool highlight)
{
    if(mRegionLabelHighlighted != highlight)
    {
        mRegionLabelHighlighted = highlight;
        mRegionLabelStyleDirty = true;
        RefreshLayout();
    }
}

void SudokuCellWidget::ResetRegionId()
{
    UpdateRegionId(0);
}

void SudokuCellWidget::SetGivenDigit(unsigned short value)
{
    mMainWindowContent->GridGet()->PuzzleDataGet()->AddGiven(value, mCol, mRow);

    mValueLabel->setText(QString::number(value));
    mContentType = ContentType::GivenDigit;
    mStackedContent->setCurrentIndex(CellView::Value);
    mValueLabelStyleDirty = true;
    RefreshLayout();
}

void SudokuCellWidget::RemoveGivenDigit()
{
    mMainWindowContent->GridGet()->PuzzleDataGet()->RemoveGiven(mCol, mRow);

    mValueLabel->setText("");
    mContentType = ContentType::CellOptions;
    mStackedContent->setCurrentIndex(CellView::Options);
    mValueLabelStyleDirty = true;
    RefreshLayout();
}

void SudokuCellWidget::SetSolvedDigit(unsigned short value)
{
    mValueLabel->setText(QString::number(value));
    mContentType = ContentType::SolvedDigit;
    mStackedContent->setCurrentIndex(CellView::Value);
    mValueLabelStyleDirty = true;
    RefreshLayout();
}
