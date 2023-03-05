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
            mActiveComponent = nullptr;
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
        if(VariantClueWidget* clue = dynamic_cast<VariantClueWidget*>(mActiveComponent); clue)
        {
            clue->ClueDidGetInactive();
        }
        mActiveComponent = component;
        if(VariantClueWidget* clue = dynamic_cast<VariantClueWidget*>(mActiveComponent); clue)
        {
            clue->ClueDidGetActive();
        }
        return true;
    }
    return false;
}

void GridGraphicalOverlay::ClearActiveComponent()
{
    if(VariantClueWidget* clue = dynamic_cast<VariantClueWidget*>(mActiveComponent); clue)
    {
        clue->ClueDidGetInactive();
    }
    mActiveComponent = nullptr;
}
