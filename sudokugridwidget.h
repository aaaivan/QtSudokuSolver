#ifndef SUDOKUGRIDWIDGET_H
#define SUDOKUGRIDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

class SudokuCellWidget;

class SudokuGridWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SudokuGridWidget(unsigned short size, QWidget *parent = nullptr);

private:
    int mSize;
    QGridLayout* mGridLayout;
    QVector<QVector<SudokuCellWidget*>> mGrid;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
};

#endif // SUDOKUGRIDWIDGET_H
