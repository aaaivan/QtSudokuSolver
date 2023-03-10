#include "sudokucellwidget.h"
#include "cellcontentbutton.h"
#include "variantcluewidget.h"
#include "sudokugridwidget.h"
#include "drawregionscontrols.h"
#include "mainwindowcontent.h"
#include "sudokusolverthread.h"
#include "mainwindow.h"
#include <QLabel>
#include <QStackedLayout>
#include <QStackedWidget>

SudokuCellWidget::SudokuCellWidget(unsigned short row, unsigned short col, unsigned short gridSize,int cellLength,
                                   MainWindowContent* mainWindowContent, QWidget *parent)
    : QFrame{parent},
      mCol(col),
      mRow(row),
      mGridSize(gridSize),
      mId(row * gridSize + col),
      mMainWindowContent(mainWindowContent),
      mNeighbours(4, nullptr),
      mVariantClues(),
      mOverlayLayout(new QStackedLayout(this)),
      mStackedContent(new QStackedWidget()),
      mOptionsLabel(new CellContentButton(gridSize, this)),
      mValueLabel(new CellContentButton(gridSize, this)),
      mRegionIdLabel(new CellContentButton(gridSize, this)),
      mGraphicsOverlay(new QLabel()),
      mContentString(""),
      mContentType(ContentType::CellOptions),
      mRegionId(0),
      mLength(cellLength),
      mBoldEdges(None),
      mHighlighted(false)
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
    mRegionIdLabel->setFont(QFont(mValueLabel->font().family(),20, 700));

    // borders and sizes
    mValueLabel->setFlat(true);
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
    this->setStyleSheet(CreateStylesheet());
    mOptionsLabel->setStyleSheet(CreateOptionsLabelStylesheet());
    mRegionIdLabel->setStyleSheet(CreateRegionLabelStylesheet());
    mValueLabel->setStyleSheet(CreateValueLabelStylesheet());
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
    style += "background-color: transparent;\n";
    style += "}\n";

    return style;
}

QString SudokuCellWidget::CreateOptionsLabelStylesheet() const
{
    static const QString normalBG = "transparent";
    static const QString focusBG = "rgba(255, 200, 0, 0.2)";

    // normal styling
    QString style = "QPushButton{\n";
    style += "background-color: " + (mHighlighted ? focusBG : normalBG) + ";\n";
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
    style += "background-color: " + (mHighlighted ? focusBG : normalBG) + ";\n";
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
    static const QString highlightBG = "rgba(0, 255, 0, 0.2)";
    static const QString focusBG = "rgba(255, 200, 0, 0.2)";
    static const QString textColor = "grey";

    // normal styling
    QString style = "QPushButton{\n";
    style += "background-color: " + (mHighlighted ? highlightBG : normalBG) + ";\n";
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

MainWindowContent *SudokuCellWidget::MainWindowContentGet() const
{
    return mMainWindowContent;
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

const QSet<VariantClueWidget *> &SudokuCellWidget::VariantCluesGet() const
{
    return mVariantClues;
}

void SudokuCellWidget::SwitchView(size_t view)
{
    switch (static_cast<MainWindowContent::ViewType>(view))
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
    }
}

void SudokuCellWidget::UpdateRegionId(unsigned short newId)
{
    auto cellCount = mMainWindowContent->GridGet()->SolverGet()->CellCountInRegion(newId);
    if(cellCount >= mGridSize)
    {
        return;
    }

    if(newId != mRegionId)
    {
        DrawRegionsControls* context = static_cast<DrawRegionsControls*>(mMainWindowContent->ContextMenuGet(MainWindowContent::DrawRegions));

        mMainWindowContent->GridGet()->SolverGet()->RemoveCellFromRegion(mRegionId, mId);
        mMainWindowContent->GridGet()->SolverGet()->AddCellToRegion(newId, mId);
        context->UpdateCellCounters(mRegionId);
        context->UpdateCellCounters(newId);
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
        mHighlighted = mRegionId == context->SelectedRegionIdGet();
        RefreshLayout();
    }
}

void SudokuCellWidget::UpdateOptions(const std::set<unsigned short> &options)
{
    QString text = "";
    int lineBreak = 0;
    for (const auto& opt : options)
    {
        if(lineBreak == mGridSize/2)
        {
            text += '\n';
        }
        text += QString::number(opt);
        lineBreak++;
    }
    mOptionsLabel->setText(text);
}

void SudokuCellWidget::NeighboursSet(SudokuCellWidget *top, SudokuCellWidget *right, SudokuCellWidget *btm, SudokuCellWidget *left)
{
    mNeighbours[0] = top;
    mNeighbours[1] = right;
    mNeighbours[2] = btm;
    mNeighbours[3] = left;
    RefreshLayout();
}

void SudokuCellWidget::SetHighlighted(bool highlight)
{
    if(mHighlighted != highlight)
    {
        mHighlighted = highlight;
        RefreshLayout();
    }
}

void SudokuCellWidget::ResetRegionId()
{
    UpdateRegionId(0);
}

void SudokuCellWidget::SetRegionId(unsigned short newId)
{
    UpdateRegionId(newId);
}

void SudokuCellWidget::SetGivenDigit(unsigned short value)
{
    mMainWindowContent->GridGet()->SolverGet()->AddGiven(value, mId);
    mMainWindowContent->MainWindowGet()->SolverGet()->SubmitChangesToSolver();

    mValueLabel->setText(QString::number(value));
    mContentType = ContentType::GivenDigit;
    mStackedContent->setCurrentIndex(CellView::Value);
    RefreshLayout();
}

void SudokuCellWidget::RemoveGivenDigit()
{
    mMainWindowContent->GridGet()->SolverGet()->RemoveGiven(mId);
    mMainWindowContent->MainWindowGet()->SolverGet()->SubmitChangesToSolver();

    mValueLabel->setText("");
    mContentType = ContentType::CellOptions;
    mStackedContent->setCurrentIndex(CellView::Options);
    RefreshLayout();
}

void SudokuCellWidget::SetSolvedDigit(unsigned short value)
{
    mValueLabel->setText(QString::number(value));
    mContentType = ContentType::SolvedDigit;
    mStackedContent->setCurrentIndex(CellView::Value);
    RefreshLayout();
}

void SudokuCellWidget::AddVariantClue(VariantClueWidget *clue)
{
    mVariantClues.insert(clue);
}

void SudokuCellWidget::RemoveVariantClue(VariantClueWidget *clue)
{
    mVariantClues.remove(clue);
}
