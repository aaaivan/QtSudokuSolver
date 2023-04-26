#include "variantclueslayer.h"
#include "variantcluewidget.h"
#include "sudokugridwidget.h"
#include "contextmenuwindow.h"

VariantCluesLayer::VariantCluesLayer(SudokuGridWidget* grid, int cellLength, QWidget *parent)
    : QFrame{parent},
      mGrid(grid),
      mCellLength(cellLength),
      mVariantClues(),
      mActiveClue(nullptr)
{
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setFrameStyle(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed,QSizePolicy::Policy::Fixed));
}

QSet<QWidget *> VariantCluesLayer::VariantCluesGet() const
{
    return mVariantClues;
}

QWidget *VariantCluesLayer::ActiveClueGet() const
{
    return mActiveClue;
}

bool VariantCluesLayer::AddVariantClue(QWidget *widget, bool setSelected)
{
    if(widget && !mVariantClues.contains(widget))
    {
        widget->setParent(this);
        widget->show();
        mVariantClues.insert(widget);

        if(VariantClueWidget* clue = dynamic_cast<VariantClueWidget*>(widget); clue)
        {
            clue->ContextMenuGet()->ClueAdded(widget);
        }
        if(setSelected)
        {
            ActiveClueSet(widget);
        }
        return true;
    }
    return false;
}

bool VariantCluesLayer::RemoveVariantClue(QWidget *widget)
{
    if(widget && mVariantClues.remove(widget))
    {
        if(mActiveClue == widget)
        {
            ClearActiveClue(true);
        }
        if(VariantClueWidget* clue = dynamic_cast<VariantClueWidget*>(widget); clue)
        {
            clue->ContextMenuGet()->ClueRemoved(widget);
        }
        widget->deleteLater();
        return true;
    }
    return false;
}

bool VariantCluesLayer::ActiveClueSet(QWidget *widget)
{
    if(mVariantClues.contains(widget) && mActiveClue != widget)
    {
        QWidget* prevActive = mActiveClue;
        mActiveClue = widget;
        VariantClueWidget* prevClue = dynamic_cast<VariantClueWidget*>(prevActive);
        if(prevClue)
        {
            prevClue->ClueDidGetInactive();
            prevClue->ContextMenuGet()->ClueDidGetInactive(prevActive, false);
        }
        VariantClueWidget* newClue = dynamic_cast<VariantClueWidget*>(mActiveClue);
        if(newClue)
        {
            newClue->ClueDidGetActive();
            newClue->ContextMenuGet()->ClueDidGetActive(mActiveClue);
        }
        return true;
    }
    return false;
}

void VariantCluesLayer::ClearActiveClue()
{
    ClearActiveClue(false);
}

void VariantCluesLayer::ClearActiveClue(bool willBeDeleted)
{
    QWidget* prevActive = mActiveClue;
    mActiveClue = nullptr;
    VariantClueWidget* clue = dynamic_cast<VariantClueWidget*>(prevActive);
    if(clue)
    {
        clue->ClueDidGetInactive();
        clue->ContextMenuGet()->ClueDidGetInactive(prevActive, willBeDeleted);
    }
}
