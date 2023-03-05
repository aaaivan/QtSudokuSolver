#ifndef DRAWREGIONSCONTROLS_H
#define DRAWREGIONSCONTROLS_H

#include "contextmenuwindow.h"
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class SudokuCellWidget;
class SudokuGridWidget;
class MainWindowContent;

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

private slots:
    void RegionSelect_CurrentIndexChanged(int index);
    void ClearRegionsBtn_Clicked();

public:
    void CellClicked(SudokuCellWidget* cell) override;
    void KeyboardInput(SudokuCellWidget* cell, QKeyEvent* event) override;

    unsigned short SelectedRegionIdGet() const;
    void UpdateCellCounters(unsigned short regionId);
};

#endif // DRAWREGIONSCONTROLS_H
