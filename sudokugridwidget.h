#ifndef SUDOKUGRIDWIDGET_H
#define SUDOKUGRIDWIDGET_H

#include "sudokusolverthread.h"
#include <QFrame>
#include <set>

class SudokuCellWidget;
class PuzzleData;
class VariantCluesLayer;
class SudokuSolverThread;
class MainWindowContent;
class SudokuSolverThread;
class QWidget;
class QLabel;
class QGridLayout;
class QPainter;

class SudokuGridWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SudokuGridWidget(unsigned short size, MainWindowContent* mainWindowContent, QWidget *parent = nullptr);
    virtual ~SudokuGridWidget();

private:
    const int mCellLength;
    unsigned short mSize;
    QVector<QVector<SudokuCellWidget*>> mCells;

    MainWindowContent* mMainWindowContent;
    VariantCluesLayer* mVariantCluesLayer;
    std::unique_ptr<SudokuSolverThread> mSolver;

    void paintEvent(QPaintEvent* event) override;

private slots:
    void UpdateOptionsOfCell(unsigned int id, const std::set<unsigned short>& content, bool isSolved);
    void SetCellValue(unsigned int id, unsigned short value);

public:
    // public getters
    unsigned short CellLengthGet() const;
    unsigned short SizeGet() const;
    const QVector<QVector<SudokuCellWidget*>>& CellsGet() const;
    SudokuCellWidget* CellGet(CellCoord id) const;
    SudokuSolverThread* SolverGet() const;
    VariantCluesLayer* VariantCluesLayerGet() const;

    // public non-const functions
    void SwitchView(size_t view);
};

#endif // SUDOKUGRIDWIDGET_H
