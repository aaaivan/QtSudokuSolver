#ifndef MAINWINDOWCONTENT_H
#define MAINWINDOWCONTENT_H

#include <QWidget>

class SudokuGridWidget;
class DrawRegionsContextMenu;
class DrawKillersContextMenu;
class ContextMenuWindow;
class MainWindow;
class QStackedLayout;
class QTabWidget;
class QLabel;
class PuzzleData;

class MainWindowContent : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindowContent(unsigned short size, MainWindow *parent, std::unique_ptr<PuzzleData> loadedGrid = nullptr);

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
    QLabel* mImpossiblePuzzleLabel;
    SudokuGridWidget* mGrid;
    ContextMenuType mCurrentView;

private slots:
    void OnTabChanged_ControlsMenu(int tab);
    void OnPuzzleBroken();
    void OnLogicalSolverReset();

public:
    MainWindow* MainWindowGet() const;
    ContextMenuWindow* ContextMenuGet(ContextMenuType menu) const;
    ContextMenuWindow* ActiveContextMenuGet() const;
    SudokuGridWidget* GridGet() const;
    ContextMenuType CurrentViewGet() const;
    void ChangeView(ContextMenuType view);
};

#endif // MAINWINDOWCONTENT_H
