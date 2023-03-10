#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class SudokuSolverThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(unsigned short gridSize, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SudokuSolverThread *mSolver;

public:
    SudokuSolverThread* SolverGet() const;
};
#endif // MAINWINDOW_H
