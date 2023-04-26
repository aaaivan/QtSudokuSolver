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
    QSet<QWidget*> mVariantClues;
    QWidget* mActiveClue;

    void ClearActiveClue(bool willBeDeleted);
public:
    QSet<QWidget*> VariantCluesGet() const;
    QWidget* ActiveClueGet() const;

    bool AddVariantClue(QWidget* widget, bool setSelected);
    bool RemoveVariantClue(QWidget* widget);
    bool ActiveClueSet(QWidget *widget);
    void ClearActiveClue();
};

#endif // VARIANTCLUESLAYER_H
