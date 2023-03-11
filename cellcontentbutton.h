#ifndef CELLCONTENTBUTTON_H
#define CELLCONTENTBUTTON_H

#include <QLabel>

class SudokuCellWidget;

class CellContentButton : public QLabel
{
    Q_OBJECT
public:
    explicit CellContentButton(unsigned short numOptions, SudokuCellWidget *parent);

private:
    unsigned short mNumOptions;
    SudokuCellWidget* mCell;

    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
};

#endif // CELLCONTENTBUTTON_H
