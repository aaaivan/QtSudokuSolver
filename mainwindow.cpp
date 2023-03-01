#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindowcontent.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MainWindowContent* content = new MainWindowContent(9, this);
    this->setCentralWidget(content);
}

MainWindow::~MainWindow()
{
    delete ui;
}

