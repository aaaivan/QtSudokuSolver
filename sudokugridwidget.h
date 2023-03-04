#ifndef SUDOKUGRIDWIDGET_H
#define SUDOKUGRIDWIDGET_H

#include "mainwindowcontent.h"
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>

class SudokuCellWidget;
class PuzzleData;
class GridGraphicalOverlay;

class SudokuGridWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SudokuGridWidget(unsigned short size, MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private:
    // style param
    const int mCellLength;

    MainWindowContent* mMainWindowContent;
    GridGraphicalOverlay* mGraphicalOverlay;

    unsigned short mSize;
    QVector<QVector<SudokuCellWidget*>> mCells;
    std::shared_ptr<PuzzleData> mPuzzleData;



    void paintEvent(QPaintEvent* event) override;

public:
    // public getters
    unsigned short SizeGet() const;
    const QVector<QVector<SudokuCellWidget*>>& CellsGet() const;
    PuzzleData* PuzzleDataGet() const;

    // public non-const functions
    void SwitchView(MainWindowContent::ViewType view);
};

#endif // SUDOKUGRIDWIDGET_H
