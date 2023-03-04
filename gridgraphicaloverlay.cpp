#include "gridgraphicaloverlay.h"
#include "sudokugridwidget.h"

GridGraphicalOverlay::GridGraphicalOverlay(SudokuGridWidget* grid, int cellLength, QWidget *parent)
    : QFrame{parent},
      mGrid(grid),
      mCellLength(cellLength),
      mOverlayComponents()
{
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setFrameStyle(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed,QSizePolicy::Policy::Fixed));
}

void GridGraphicalOverlay::AddOverlayComponent(QLabel *component)
{
    component->setParent(this);
    mOverlayComponents.push_back(component);
}
