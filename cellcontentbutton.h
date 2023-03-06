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
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void CellClicked(bool);
};

#endif // CELLCONTENTBUTTON_H
