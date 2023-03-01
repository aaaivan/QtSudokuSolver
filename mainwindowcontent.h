#ifndef MAINWINDOWCONTENT_H
#define MAINWINDOWCONTENT_H

#include <QWidget>
#include <QStackedLayout>

class SudokuGridWidget;

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
    void ChangeView(ViewType view);
};

#endif // MAINWINDOWCONTENT_H
