#include "mainwindow.h"
#include "savepuzzlethread.h"
#include "sudokugridwidget.h"
#include "ui_mainwindow.h"
#include "mainwindowcontent.h"
#include <QFileDialog>

MainWindow::MainWindow(unsigned short gridSize, std::unique_ptr<PuzzleData> loadedGrid, QString loadPath, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mWindowContent(new MainWindowContent(gridSize, this, std::move(loadedGrid)))
    , mSavePuzzleThread(new SavePuzzleThread(this))
    , mSaveFilePath(loadPath)
{
    ui->setupUi(this);
    this->setCentralWidget(mWindowContent);

    connect(mSavePuzzleThread, SIGNAL(SaveSuccessul(QString)), this, SLOT(OnSaveSuccessful(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{

}

void MainWindow::on_actionSave_As_triggered()
{
    QString path =  QFileDialog::getSaveFileName(this, "Save File", "", "*.sudoku");
    if(!path.isEmpty())
    {
        PuzzleData pd = mWindowContent->GridGet()->SolverGet()->PuzzleDataGet();
        mSavePuzzleThread->SavePuzzle(std::move(pd), path);
    }
}

void MainWindow::on_actionSave_triggered()
{
    if(mSaveFilePath.isEmpty())
    {
        on_actionSave_As_triggered();
    }
    else
    {
        PuzzleData pd = mWindowContent->GridGet()->SolverGet()->PuzzleDataGet();
        mSavePuzzleThread->SavePuzzle(std::move(pd), mSaveFilePath);
    }
}

void MainWindow::on_actionQuit_triggered()
{

}

void MainWindow::OnSaveSuccessful(QString path)
{
    mSaveFilePath = path;
}

