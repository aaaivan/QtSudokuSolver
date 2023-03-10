#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindowcontent.h"

MainWindow::MainWindow(unsigned short gridSize, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MainWindowContent* content = new MainWindowContent(gridSize, this);
    this->setCentralWidget(content);
}

MainWindow::~MainWindow()
{
    delete ui;
}
