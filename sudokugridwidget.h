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
    const int mCellLength;
    unsigned short mSize;
    std::shared_ptr<PuzzleData> mPuzzleData;
    QVector<QVector<SudokuCellWidget*>> mCells;

    MainWindowContent* mMainWindowContent;
    GridGraphicalOverlay* mGraphicalOverlay;

    void paintEvent(QPaintEvent* event) override;

public:
    // public getters
    unsigned short CellLengthGet() const;
    unsigned short SizeGet() const;
    const QVector<QVector<SudokuCellWidget*>>& CellsGet() const;
    PuzzleData* PuzzleDataGet() const;
    GridGraphicalOverlay* GraphicalOverlayGet() const;

    // public non-const functions
    void SwitchView(MainWindowContent::ViewType view);
};

#endif // SUDOKUGRIDWIDGET_H
