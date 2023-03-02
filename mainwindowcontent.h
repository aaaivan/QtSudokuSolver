#ifndef MAINWINDOWCONTENT_H
#define MAINWINDOWCONTENT_H

#include <QWidget>
#include <QStackedLayout>
#include <memory>

class SudokuGridWidget;
class DrawRegionsControls;

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
    DrawRegionsControls* DrawRegionContextMenuGet() const;
    SudokuGridWidget* GridGet() const;
    void ChangeView(ViewType view);
};

#endif // MAINWINDOWCONTENT_H
