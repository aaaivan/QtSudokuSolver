#ifndef STARTMENU_H
#define STARTMENU_H

#include <QWidget>
#include <QStackedLayout>
#include <QPushButton>
#include <QSpinBox>

class StartMenu : public QWidget
{
    Q_OBJECT
public:
    explicit StartMenu(QWidget *parent = nullptr);

private:
    enum MainMenuType
    {
        MMT_Start,
        MMT_NewPuzzle
    };
    QStackedLayout* mMenus;

    QPushButton* mNewPuzzleButton;
    QPushButton* mLoadPuzzleButton;

    QSpinBox* mPuzzleSizeSpinbox;
    QPushButton* mCreatePuzzleBtn;
    QPushButton* mBackBtn;

private slots:
    void NewPuzzleBtn_clicked();
    void LoadPuzzleBtn_clicked();

    void CreatePuzzleBtn_clicked();
    void BackBtn_clicked();
};

#endif // STARTMENU_H
