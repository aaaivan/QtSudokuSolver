#include "contextmenuwindow.h"
#include "qglobal.h"

ContextMenuWindow::ContextMenuWindow(MainWindowContent* mainWindowContent):
    mMainWindowContent(mainWindowContent)
{
}

void ContextMenuWindow::ClueAdded(QWidget *clue)
{
    Q_UNUSED(clue)
    return;
}

void ContextMenuWindow::ClueRemoved(QWidget *clue)
{
    Q_UNUSED(clue)
    return;
}

void ContextMenuWindow::ClueDidGetActive(QWidget *clue)
{
    Q_UNUSED(clue)
    return;
}

void ContextMenuWindow::ClueDidGetInactive(QWidget *clue)
{
    Q_UNUSED(clue)
    return;
}
