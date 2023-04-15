#ifndef MAINWINDOWCONTENT_H
#define MAINWINDOWCONTENT_H

#include <QWidget>

class SudokuGridWidget;
class DrawRegionsControls;
class DrawKillersControls;
class ContextMenuWindow;
class MainWindow;
class QStackedLayout;
class QTabWidget;

class MainWindowContent : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindowContent(unsigned short size, MainWindow *parent);

    enum ContextMenuType
    {
        EnterDigits_Context,
        DrawRegions_Context,
        DrawKiller_Context,
        Solver_Context
    };

    enum ControlsMenuType
    {
        CreateTab,
        SolverTab
    };

private:
    MainWindow* mMainWindow;
    QTabWidget* mControlsMenu;
    QStackedLayout* mContextMenu;
    SudokuGridWidget* mGrid;
    ContextMenuType mCurrentView;

private slots:
    void OnTabChanged_ControlsMenu(int tab);

public:
    MainWindow* MainWindowGet() const;
    ContextMenuWindow* ContextMenuGet(ContextMenuType menu) const;
    ContextMenuWindow* ActiveContextMenuGet() const;
    SudokuGridWidget* GridGet() const;
    ContextMenuType CurrentViewGet() const;
    void ChangeView(ContextMenuType view);
};

#endif // MAINWINDOWCONTENT_H
