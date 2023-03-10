#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindowcontent.h"
#include "sudokusolverthread.h"

MainWindow::MainWindow(unsigned short gridSize, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mSolver(new SudokuSolverThread(gridSize))
{
    ui->setupUi(this);

    MainWindowContent* content = new MainWindowContent(gridSize, this);
    this->setCentralWidget(content);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mSolver;
}

SudokuSolverThread *MainWindow::SolverGet() const
{
    return mSolver;
}
