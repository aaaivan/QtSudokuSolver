#include "sudokucellwidget.h"

SudokuCellWidget::SudokuCellWidget(QWidget *parent)
    : QFrame{parent},
      mLength(40),
      mOverlayLayout(new QStackedLayout(this)),
      mStackedContent(new QStackedWidget()),
      mOptionsLabel(new QPushButton()),
      mValueLabel(new QPushButton()),
      mRegionIdLabel(new QPushButton()),
      mGraphicsOverlay(new QLabel()),
      mContentString("12345\n6789"),
      mContentType(ContentType::CellOptions),
      mRegionId(0),
      mValueDirty(true),
      mStyleDirty(true),
      mBoldEdges(None),
      mBGColour("transparent"),
      mFocusBGColour("rgba(255, 255, 0, 0.4)")
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
    mValueLabel->setFont(QFont(mValueLabel->font().family(),28, 600));

    // borders and sizes
    mOptionsLabel->setFlat(true);
    mValueLabel->setFlat(true);
    mRegionIdLabel->setFlat(true);

    RefreshLayout();
}

void SudokuCellWidget::RefreshLayout()
{
    if(mStyleDirty)
    {
        this->setStyleSheet(CreateStylesheet());
    }

    // options style
    if(mValueDirty)
    {
        if(mContentType != ContentType::CellOptions)
        {
            mValueDirty = false;
            bool isGiven = mContentType == ContentType::GivenDigit;
            mValueLabel->setStyleSheet("QLabel{color: " + (isGiven ? QString("black") : QString("grey")) + ";}");
        }
    }
}

QSize SudokuCellWidget::sizeHint() const
{
    return QSize(mLength, mLength);
}

QSize SudokuCellWidget::minimumSizeHint() const
{
    return QSize(mLength, mLength);
}

QString SudokuCellWidget::GetEdgeName(Edges edge) const
{
    switch (edge) {
    case Edges::TopEdge:
        return "top";
    case Edges::RightEdge:
        return "right";
    case Edges::BottomEdge:
        return "bottom";
    case Edges::LeftEdge:
        return "left";
    default:
        return "";
    }
}

QString SudokuCellWidget::CreateStylesheet() const
{
    // normal styling
    QString style = "SudokuCellWidget{\n";
    // borders
    for(int border = Edges::LeftEdge; border > 0; border = border >> 1)
    {
        bool isBold = mBoldEdges & border;
        style += "border-" + GetEdgeName(static_cast<Edges>(border)) + ": " +
                (isBold ? "3" : "1") + "px solid " +
                (isBold ? "black" : "grey") + ";\n";
    }
    style += "background-color: " + mBGColour + ";\n";
    style += "}\n";

    // focus styling
    style += "SudokuCellWidget:focus{\n";

    style += "}\n";
    style += "background-color: " + mFocusBGColour + ";\n";
    return style;
}

void SudokuCellWidget::ShowRegionNumber()
{
    mStackedContent->setCurrentIndex(CellView::RegionId);
}

void SudokuCellWidget::HideRegionNumber()
{
    CellView view = mContentType == ContentType::CellOptions ?
                    CellView::Options :
                    CellView::Value;
    mStackedContent->setCurrentIndex(view);
}
