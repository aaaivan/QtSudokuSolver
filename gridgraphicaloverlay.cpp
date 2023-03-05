#include "gridgraphicaloverlay.h"
#include "contextmenuwindow.h"
#include "sudokugridwidget.h"
#include "variantcluewidget.h"

GridGraphicalOverlay::GridGraphicalOverlay(SudokuGridWidget* grid, int cellLength, QWidget *parent)
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

QSet<QWidget *> GridGraphicalOverlay::OverlayComponentsGet() const
{
    return mOverlayComponents;
}

QWidget *GridGraphicalOverlay::ActiveComponentGet() const
{
    return mActiveComponent;
}

bool GridGraphicalOverlay::AddOverlayComponent(QWidget *component)
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
        return true;
    }
    return false;
}

bool GridGraphicalOverlay::RemoveOverlayComponent(QWidget *component)
{
    if(component && mOverlayComponents.remove(component))
    {
        if(mActiveComponent == component)
        {
            ClearActiveComponent();
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

bool GridGraphicalOverlay::ActiveComponentSet(QWidget *component)
{
    if(mOverlayComponents.contains(component) && mActiveComponent != component)
    {
        QWidget* prevActive = mActiveComponent;
        mActiveComponent = component;
        VariantClueWidget* prevClue = dynamic_cast<VariantClueWidget*>(prevActive);
        if(prevClue)
        {
            prevClue->ClueDidGetInactive();
            prevClue->ContextMenuGet()->ClueDidGetInactive(prevActive);
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

void GridGraphicalOverlay::ClearActiveComponent()
{
    QWidget* prevActive = mActiveComponent;
    mActiveComponent = nullptr;
    VariantClueWidget* clue = dynamic_cast<VariantClueWidget*>(prevActive);
    if(clue)
    {
        clue->ClueDidGetInactive();
        clue->ContextMenuGet()->ClueDidGetInactive(prevActive);
    }
}
