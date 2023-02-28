#include "sudokucellwidget.h"

SudokuCellWidget::SudokuCellWidget(QWidget *parent)
    : QFrame{parent},
      mLength(40),
      mOverlayLayout(new QStackedLayout(this)),
      mStackedWidget(new QStackedWidget()),
      mOptions(new QLabel()),
      mValue(new QLabel()),
      mGraphicsOverlay(new QLabel()),
      mContentString("12345\n6789"),
      mBoldEdges(Edges::None),
      mCellType(CellType::Unsolved),
      mRegionNumber(0),
      mBorderDirty(true),
      mContentDirty(true)
{
    mOverlayLayout->setStackingMode(QStackedLayout::StackingMode::StackAll);
    this->setLayout(mOverlayLayout);
    mOverlayLayout->addWidget(mGraphicsOverlay);

    // set overlay graphics
    //mGraphicsOverlay->setPixmap(QPixmap(":/assets/assets/TestBorder.png").scaled(40,40));

    mOverlayLayout->addWidget(mStackedWidget);

    mStackedWidget->addWidget(mOptions);
    mStackedWidget->addWidget(mValue);

    this->setObjectName("cell");
    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    mOptions->setAlignment(Qt::AlignCenter);
    mValue->setAlignment(Qt::AlignCenter);

    RefreshLayout();
}

void SudokuCellWidget::RefreshLayout()
{
    // border styling
    if(mBorderDirty)
    {
        mBorderDirty = false;
        QString style = "";
        for(int border = Edges::LeftEdge; border > 0; border = border >> 1)
        {
            bool isBold = mBoldEdges & border;
            style += "border-" + GetEdgeName(static_cast<Edges>(border)) + ": " +
                    (isBold ? "3" : "1") + "px solid " +
                    (isBold ? "black" : "grey") + ";\n";
        }
        setStyleSheet("QWidget#cell{" + style + ";}");
    }

    // options style
    if(mContentDirty)
    {
        mContentDirty = false;
        if(mCellType == CellType::Unsolved)
        {
            mStackedWidget->setCurrentIndex(0);
            mOptions->setText(mContentString);
            QFont f( mOptions->font().family(), 8);
            mOptions->setFont(f);
        }
        else
        {
            mStackedWidget->setCurrentIndex(1);
            mValue->setText(mContentString);
            QFont f(mValue->font().family(),28, 600);
            mValue->setFont(f);

            bool isGiven = mCellType == CellType::Given;
            mValue->setStyleSheet("QLabel{color: " + (isGiven ? QString("black") : QString("grey")) + ";}");
        }
    }
}

QString SudokuCellWidget::GetEdgeName(Edges edge)
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

QSize SudokuCellWidget::sizeHint() const
{
    return QSize(mLength, mLength);
}

QSize SudokuCellWidget::minimumSizeHint() const
{
    return QSize(mLength, mLength);
}
