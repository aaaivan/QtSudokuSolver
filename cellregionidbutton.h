#ifndef CELLREGIONIDBUTTON_H
#define CELLREGIONIDBUTTON_H

#include <QPushButton>

class SudokuCellWidget;

class CellRegionIdButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CellRegionIdButton(unsigned short numOptions, SudokuCellWidget *parent);

private:
    unsigned short mNumRegions;
    SudokuCellWidget* mCell;

    void keyReleaseEvent(QKeyEvent* event) override;
};

#endif // CELLREGIONIDBUTTON_H
