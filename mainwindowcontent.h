#ifndef MAINWINDOWCONTENT_H
#define MAINWINDOWCONTENT_H

#include <QWidget>
#include <QStackedLayout>
#include <memory>

class SudokuGridWidget;
class DrawRegionsControls;
class DrawKillersControls;
class ContextMenuWindow;

class MainWindowContent : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindowContent(unsigned short size, QWidget *parent = nullptr);

    enum ViewType
    {
        EnterDigits,
        DrawRegions,
        DrawKiller
    };

private:
    QStackedLayout* mContextMenu;
    SudokuGridWidget* mGrid;
    ViewType mCurrentView;

public:
    ContextMenuWindow* ContextMenuGet(ViewType menu) const;
    ContextMenuWindow* ActiveContextMenuGet();
    SudokuGridWidget* GridGet() const;
    ViewType CurrentViewGet() const;
    void ChangeView(ViewType view);
};

#endif // MAINWINDOWCONTENT_H
