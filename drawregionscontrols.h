#ifndef DRAWREGIONSCONTROLS_H
#define DRAWREGIONSCONTROLS_H

#include "contextmenuwindow.h"
#include "puzzledata.h"
#include <QWidget>
#include <set>

class SudokuCellWidget;
class SudokuGridWidget;
class MainWindowContent;
class QComboBox;
class QPushButton;
class QLabel;

class DrawRegionsControls : public QWidget, public ContextMenuWindow
{
    Q_OBJECT
public:
    explicit DrawRegionsControls(MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private:
    SudokuGridWidget* mGrid;
    QComboBox* mRegionSelect;
    QVector<QLabel*> mCellCounters;
    QPushButton* mClearRegionsBtn;

    std::vector<std::set<CellCoord>> mRegions;

    void hideEvent(QHideEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void SetRegionIdOfCell(SudokuCellWidget* cell, unsigned short newId);
    void UpdateCellCounters(unsigned short regionId);

private slots:
    void RegionSelect_CurrentIndexChanged(int index);
    void ClearRegionsBtn_Clicked();

public:
    void CellGainedFocus(SudokuCellWidget* cell) override;
    void CellLostFocus(SudokuCellWidget* cell) override;
    void CellClicked(SudokuCellWidget* cell) override;
    void KeyboardInput(SudokuCellWidget* cell, QKeyEvent* event) override;

    unsigned short SelectedRegionIdGet() const;
};

#endif // DRAWREGIONSCONTROLS_H
