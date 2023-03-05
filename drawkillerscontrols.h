#ifndef DRAWKILLERSCONTROLS_H
#define DRAWKILLERSCONTROLS_H

#include "contextmenuwindow.h"
#include <QWidget>
#include <QSpinBox>
#include <QStackedLayout>

class MainWindowContent;
class SudokuGridWidget;
class SudokuCellWidget;
class KillerCageWidget;

class DrawKillersControls : public QWidget, public ContextMenuWindow
{
    Q_OBJECT
public:
    explicit DrawKillersControls(MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private slots:
    void DeleteAllKillersBtn_Clicked();
    void EditingDoneBtn_Clicked();
    void DeleteActiveKillerBtn_Clicked();
    void CageTotal_ValueChanged(int value);

private:
    enum MenuView: int
    {
        MainView,
        EditKiller
    };

    SudokuGridWidget* mGrid;
    MenuView mCurrentView;
    QStackedLayout* mStackedLayout;
    QSpinBox* mCageTotal;

    void hideEvent(QHideEvent* event) override;
    void showEvent(QShowEvent* event) override;

    void UpdateCageTotalLimits(KillerCageWidget* activeCage);
    void SwitchView(MenuView newView);
    void CellClickedInMainView(SudokuCellWidget* cell);
    void CellClickedInEditView(SudokuCellWidget* cell);
    void CreateNewCageFromCell(SudokuCellWidget* cell);

public:
    void CellClicked(SudokuCellWidget* cell) override;
    void KeyboardInput(SudokuCellWidget* cell, QKeyEvent* event) override;
    void ClueAdded(QWidget* clue) override;
    void ClueDidGetActive(QWidget* clue) override;
    void ClueDidGetInactive(QWidget* clue) override;
};

#endif // DRAWKILLERSCONTROLS_H
