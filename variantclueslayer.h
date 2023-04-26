#ifndef VARIANTCLUESLAYER_H
#define VARIANTCLUESLAYER_H

#include <QFrame>

class SudokuGridWidget;

class VariantCluesLayer : public QFrame
{
    Q_OBJECT
public:
    explicit VariantCluesLayer(SudokuGridWidget* grid, int cellLength, QWidget *parent = nullptr);

private :
    SudokuGridWidget* mGrid;
    const int mCellLength;
    QSet<QWidget*> mOverlayComponents;
    QWidget* mActiveComponent;

    void ClearActiveComponent(bool willBeDeleted);
public:
    QSet<QWidget*> OverlayComponentsGet() const;
    QWidget* ActiveComponentGet() const;

    bool AddOverlayComponent(QWidget* component, bool setSelected);
    bool RemoveOverlayComponent(QWidget* component);
    bool ActiveComponentSet(QWidget *component);
    void ClearActiveComponent();
};

#endif // VARIANTCLUESLAYER_H
