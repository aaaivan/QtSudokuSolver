#ifndef MAINWINDOWCONTENT_H
#define MAINWINDOWCONTENT_H

#include <QWidget>

class SudokuGridWidget;
class DrawRegionsControls;
class DrawKillersControls;
class ContextMenuWindow;
class MainWindow;
class QStackedLayout;

class MainWindowContent : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindowContent(unsigned short size, MainWindow *parent);

    enum ViewType
    {
        EnterDigits,
        DrawRegions,
        DrawKiller,
        Solver
    };

private:
    MainWindow* mMainWindow;
    QStackedLayout* mContextMenu;
    SudokuGridWidget* mGrid;
    ViewType mCurrentView;

public:
    MainWindow* MainWindowGet() const;
    ContextMenuWindow* ContextMenuGet(ViewType menu) const;
    ContextMenuWindow* ActiveContextMenuGet() const;
    SudokuGridWidget* GridGet() const;
    ViewType CurrentViewGet() const;
    void ChangeView(ViewType view);
};

#endif // MAINWINDOWCONTENT_H
