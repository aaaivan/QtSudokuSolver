#ifndef SUDOKUGRIDWIDGET_H
#define SUDOKUGRIDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

class MainWindowContent;
class SudokuCellWidget;

class SudokuGridWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SudokuGridWidget(unsigned short size, MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private:
    unsigned short mSize;
    MainWindowContent* mMainWindowContent;
    QGridLayout* mGridLayout;
    QVector<QVector<SudokuCellWidget*>> mGrid;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public:
    unsigned short SizeGet();
};

#endif // SUDOKUGRIDWIDGET_H
