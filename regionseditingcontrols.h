#ifndef REGIONSEDITINGCONTROLS_H
#define REGIONSEDITINGCONTROLS_H

#include <QWidget>
#include <QComboBox>

class SudokuCellWidget;
class SudokuGridWidget;

class RegionsEditingControls : public QWidget
{
    Q_OBJECT
public:
    explicit RegionsEditingControls(SudokuGridWidget* grid, QWidget *parent = nullptr);

private:
    QComboBox* mRegionSelect;
    SudokuGridWidget* mGrid;

private slots:
    void OnRegionNumChanged(int index);
};

#endif // REGIONSEDITINGCONTROLS_H
