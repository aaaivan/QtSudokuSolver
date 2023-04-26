#ifndef DRAWKILLERSCONTEXTMENU_H
#define DRAWKILLERSCONTEXTMENU_H

#include "contextmenuwindow.h"
#include "puzzledata.h"
#include <QWidget>

class MainWindowContent;
class SudokuGridWidget;
class SudokuCellWidget;
class KillerCageWidget;
class QSpinBox;
class QStackedLayout;

class DrawKillersContextMenu : public QWidget, public ContextMenuWindow
{
    Q_OBJECT
public:
    explicit DrawKillersContextMenu(MainWindowContent* mainWindowContent, const PuzzleData* loadedGrid = nullptr, QWidget *parent = nullptr);

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

    CellCoord mIdOfKillerBeingModified;

    void hideEvent(QHideEvent* event) override;
    void showEvent(QShowEvent* event) override;

    void UpdateCageTotalLimits(KillerCageWidget* activeCage);
    void SwitchView(MenuView newView);
    void CellClickedInMainView(SudokuCellWidget* cell);
    void CellClickedInEditView(SudokuCellWidget* cell);
    void CreateNewCageFromCell(SudokuCellWidget* cell);

public:
    void CellGainedFocus(SudokuCellWidget* cell) override;
    void CellLostFocus(SudokuCellWidget* cell) override;
    void CellClicked(SudokuCellWidget* cell) override;
    void KeyboardInput(SudokuCellWidget* cell, QKeyEvent* event) override;
    void ClueDidGetActive(QWidget* clue) override;
    void ClueDidGetInactive(QWidget* clue, bool willBeDeleted) override;
};

#endif // DRAWKILLERSCONTEXTMENU_H
