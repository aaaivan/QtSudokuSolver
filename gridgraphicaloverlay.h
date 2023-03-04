#ifndef GRIDGRAPHICALOVERLAY_H
#define GRIDGRAPHICALOVERLAY_H

#include <QFrame>
#include <QLabel>

class SudokuGridWidget;

class GridGraphicalOverlay : public QFrame
{
    Q_OBJECT
public:
    explicit GridGraphicalOverlay(SudokuGridWidget* grid, int cellLength, QWidget *parent = nullptr);

private :
    SudokuGridWidget* mGrid;
    const int mCellLength;

    QVector<QLabel*> mOverlayComponents;

public:
    void AddOverlayComponent(QLabel* component);
};

#endif // GRIDGRAPHICALOVERLAY_H
