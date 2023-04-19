#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindowContent;
class SavePuzzleThread;
class PuzzleData;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(unsigned short gridSize, std::unique_ptr<PuzzleData> loadedGrid = nullptr, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void on_actionSave_As_triggered();
    void on_actionSave_triggered();
    void on_actionQuit_triggered();

    void OnSaveSuccessful(QString path);

private:
    Ui::MainWindow *ui;
    MainWindowContent* mWindowContent;
    SavePuzzleThread* mSavePuzzleThread;
    QString mSaveFilePath;
};
#endif // MAINWINDOW_H
