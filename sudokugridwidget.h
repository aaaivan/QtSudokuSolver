#ifndef SUDOKUGRIDWIDGET_H
#define SUDOKUGRIDWIDGET_H

#include "mainwindowcontent.h"
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>

class SudokuCellWidget;
class PuzzleData;

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
    std::shared_ptr<PuzzleData> mPuzzleData;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
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
