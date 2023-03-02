#ifndef DRAWREGIONSCONTROLS_H
#define DRAWREGIONSCONTROLS_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>

class SudokuCellWidget;
class SudokuGridWidget;

class DrawRegionsControls : public QWidget
{
    Q_OBJECT
public:
    explicit DrawRegionsControls(SudokuGridWidget* grid, QWidget *parent = nullptr);

private:
    SudokuGridWidget* mGrid;
    QComboBox* mRegionSelect;
    QPushButton* mClearRegionsBtn;

private slots:
    void RegionSelect_CurrentIndexChanged(int index);
    void ClearRegionsBtn_Clicked();

public:
    unsigned short SelectedRegionIdGet() const;
};

#endif // DRAWREGIONSCONTROLS_H
