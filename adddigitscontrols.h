#ifndef ADDDIGITSCONTROLS_H
#define ADDDIGITSCONTROLS_H

#include "contextmenuwindow.h"
#include <QWidget>
#include <QStackedLayout>
#include <QCheckBox>

class SudokuGridWidget;

class AddDigitsControls : public QWidget, public ContextMenuWindow
{
    Q_OBJECT
public:
    explicit AddDigitsControls(MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private slots:
    void DeleteAllBtn_Clicked();
    void HintCheckbox_Toggled(int id, bool checked);
    void DoneBtn_Clicked();
    void DeleteBtn_Clicked();
    void App_FocusChanged(QWidget* old, QWidget* now);

private:
    enum MenuView: int
    {
        MainView,
        DetailedView
    };

    SudokuGridWidget* mGrid;
    QStackedLayout* mStackedLayout;
    QVector<QCheckBox*> mCheckBoxes;
    MenuView mCurrentView;
    SudokuCellWidget* mFocusedCell;

    void hideEvent(QHideEvent* event) override;
    void showEvent(QShowEvent* event) override;

    void SwitchView(MenuView newView);
    void FocusedCellSet(SudokuCellWidget* cell);

public:
    void CellGainedFocus(SudokuCellWidget* cell) override;
    void CellLostFocus(SudokuCellWidget* cell) override;
    void KeyboardInput(SudokuCellWidget* cell, QKeyEvent* event) override;
};

#endif // ADDDIGITSCONTROLS_H
