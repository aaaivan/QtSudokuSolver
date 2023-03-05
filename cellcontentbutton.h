#ifndef CELLCONTENTBUTTON_H
#define CELLCONTENTBUTTON_H

#include <QPushButton>

class SudokuCellWidget;

class CellContentButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CellContentButton(unsigned short numOptions, SudokuCellWidget *parent);

private:
    unsigned short mNumOptions;
    SudokuCellWidget* mCell;

    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void OnClicked();
};

#endif // CELLCONTENTBUTTON_H
