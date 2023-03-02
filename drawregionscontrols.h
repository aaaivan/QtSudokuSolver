#ifndef DRAWREGIONSCONTROLS_H
#define DRAWREGIONSCONTROLS_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

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
    QVector<QLabel*> mCellCounters;
    QPushButton* mClearRegionsBtn;

private slots:
    void RegionSelect_CurrentIndexChanged(int index);
    void ClearRegionsBtn_Clicked();

public:
    unsigned short SelectedRegionIdGet() const;
    void UpdateCellCounters(unsigned short regionId);
};

#endif // DRAWREGIONSCONTROLS_H
