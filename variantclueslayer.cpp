#include "variantclueslayer.h"
#include "variantcluewidget.h"
#include "sudokugridwidget.h"
#include "contextmenuwindow.h"

VariantCluesLayer::VariantCluesLayer(SudokuGridWidget* grid, int cellLength, QWidget *parent)
    : QFrame{parent},
      mGrid(grid),
      mCellLength(cellLength),
      mOverlayComponents(),
      mActiveComponent(nullptr)
{
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setFrameStyle(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed,QSizePolicy::Policy::Fixed));
}

QSet<QWidget *> VariantCluesLayer::OverlayComponentsGet() const
{
    return mOverlayComponents;
}

QWidget *VariantCluesLayer::ActiveComponentGet() const
{
    return mActiveComponent;
}

bool VariantCluesLayer::AddOverlayComponent(QWidget *component, bool setSelected)
{
    if(component && !mOverlayComponents.contains(component))
    {
        component->setParent(this);
        component->show();
        mOverlayComponents.insert(component);

        if(VariantClueWidget* clue = dynamic_cast<VariantClueWidget*>(component); clue)
        {
            clue->ContextMenuGet()->ClueAdded(component);
        }
        if(setSelected)
        {
            ActiveComponentSet(component);
        }
        return true;
    }
    return false;
}

bool VariantCluesLayer::RemoveOverlayComponent(QWidget *component)
{
    if(component && mOverlayComponents.remove(component))
    {
        if(mActiveComponent == component)
        {
            ClearActiveComponent(true);
        }
        if(VariantClueWidget* clue = dynamic_cast<VariantClueWidget*>(component); clue)
        {
            clue->ContextMenuGet()->ClueRemoved(component);
        }
        component->deleteLater();
        return true;
    }
    return false;
}

bool VariantCluesLayer::ActiveComponentSet(QWidget *component)
{
    if(mOverlayComponents.contains(component) && mActiveComponent != component)
    {
        QWidget* prevActive = mActiveComponent;
        mActiveComponent = component;
        VariantClueWidget* prevClue = dynamic_cast<VariantClueWidget*>(prevActive);
        if(prevClue)
        {
            prevClue->ClueDidGetInactive();
            prevClue->ContextMenuGet()->ClueDidGetInactive(prevActive, false);
        }
        VariantClueWidget* newClue = dynamic_cast<VariantClueWidget*>(mActiveComponent);
        if(newClue)
        {
            newClue->ClueDidGetActive();
            newClue->ContextMenuGet()->ClueDidGetActive(mActiveComponent);
        }
        return true;
    }
    return false;
}

void VariantCluesLayer::ClearActiveComponent()
{
    ClearActiveComponent(false);
}

void VariantCluesLayer::ClearActiveComponent(bool willBeDeleted)
{
    QWidget* prevActive = mActiveComponent;
    mActiveComponent = nullptr;
    VariantClueWidget* clue = dynamic_cast<VariantClueWidget*>(prevActive);
    if(clue)
    {
        clue->ClueDidGetInactive();
        clue->ContextMenuGet()->ClueDidGetInactive(prevActive, willBeDeleted);
    }
}
