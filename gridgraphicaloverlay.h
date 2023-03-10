#ifndef GRIDGRAPHICALOVERLAY_H
#define GRIDGRAPHICALOVERLAY_H

#include <QFrame>

class SudokuGridWidget;

class GridGraphicalOverlay : public QFrame
{
    Q_OBJECT
public:
    explicit GridGraphicalOverlay(SudokuGridWidget* grid, int cellLength, QWidget *parent = nullptr);

private :
    SudokuGridWidget* mGrid;
    const int mCellLength;
    QSet<QWidget*> mOverlayComponents;
    QWidget* mActiveComponent;

public:
    QSet<QWidget*> OverlayComponentsGet() const;
    QWidget* ActiveComponentGet() const;

    bool AddOverlayComponent(QWidget* component);
    bool RemoveOverlayComponent(QWidget* component);
    bool ActiveComponentSet(QWidget *component);
    void ClearActiveComponent();
};

#endif // GRIDGRAPHICALOVERLAY_H
