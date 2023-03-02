#ifndef SUDOKUGRIDWIDGET_H
#define SUDOKUGRIDWIDGET_H

#include "mainwindowcontent.h"
#include <QWidget>
#include <QLabel>
#include <QGridLayout>

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
    QVector<QVector<SudokuCellWidget*>> mCells;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public:
    // public getters
    unsigned short SizeGet();
    const QVector<QVector<SudokuCellWidget*>>& CellsGet() const;

    // public non-const functions
    void SwitchView(MainWindowContent::ViewType view);
};

#endif // SUDOKUGRIDWIDGET_H
