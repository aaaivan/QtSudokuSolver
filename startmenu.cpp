#include "startmenu.h"
#include "mainwindow.h"
#include "puzzledata.h"
#include "IO/SaveLoadManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QFormLayout>
#include <QFileDialog>

unsigned short kMinPuzzleSize = 4;
unsigned short kMaxPuzzleSize = 9;

StartMenu::StartMenu(QWidget *parent)
    : QWidget{parent}
    , mMenus(new QStackedLayout())
    , mNewPuzzleButton(new QPushButton("New Puzzle"))
    , mLoadPuzzleButton(new QPushButton("Load Puzzle"))
    , mPuzzleSizeSpinbox(new QSpinBox())
    , mCreatePuzzleBtn(new QPushButton("Create Puzzle"))
    , mBackBtn(new QPushButton("Back"))
{
    resize(400, 300);
    setLayout(mMenus);

    // create the menus
    QFrame* startMenu = new QFrame();
    QFrame* newPuzzleMenu = new QFrame();
    mMenus->addWidget(startMenu);
    mMenus->addWidget(newPuzzleMenu);
    mMenus->setCurrentIndex(MainMenuType::MMT_Start);

    {
        // start menu menu layout
        QVBoxLayout* verticalLayout = new QVBoxLayout();
        startMenu->setLayout(verticalLayout);
        QLabel* title = new QLabel("Sudoku Setting Tool");
        QLabel* subTitle = new QLabel("by Ivan Palazzo");
        verticalLayout->addWidget(title);
        verticalLayout->addWidget(subTitle);
        verticalLayout->addSpacing(20);
        verticalLayout->addWidget(mNewPuzzleButton);
        verticalLayout->addWidget(mLoadPuzzleButton);

        // start menu tyling
        verticalLayout->setContentsMargins(50, 80, 50, 80);
        title->setFont(QFont("Segoe UI", 32, 600));
        subTitle->setFont(QFont("Segoe UI", 12));
        title->setAlignment(Qt::AlignHCenter);
        subTitle->setAlignment(Qt::AlignHCenter);
        mNewPuzzleButton->setFont(QFont("Segoe UI", 14));
        mLoadPuzzleButton->setFont(QFont("Segoe UI", 14));
    }

    {
        // new puzzle menu layout
        QVBoxLayout* verticalLayout = new QVBoxLayout();
        newPuzzleMenu->setLayout(verticalLayout);
        QWidget* form = new QWidget();
        QFormLayout* formLayout = new QFormLayout();
        form->setLayout(formLayout);
        verticalLayout->addWidget(form);
        verticalLayout->addWidget(mCreatePuzzleBtn);
        verticalLayout->addWidget(mBackBtn);

        // spinbox form field
        formLayout->addRow("Select puzzle size:", mPuzzleSizeSpinbox);
        mPuzzleSizeSpinbox->setRange(kMinPuzzleSize, kMaxPuzzleSize);

        // styling
        verticalLayout->setContentsMargins(50, 80, 50, 80);
        mCreatePuzzleBtn->setFont(QFont("Segoe UI", 14));
        mBackBtn->setFont(QFont("Segoe UI", 10));
    }

    // events
    connect(mNewPuzzleButton, SIGNAL(clicked(bool)), this, SLOT(NewPuzzleBtn_clicked()));
    connect(mLoadPuzzleButton, SIGNAL(clicked(bool)), this, SLOT(LoadPuzzleBtn_clicked()));
    connect(mCreatePuzzleBtn, SIGNAL(clicked(bool)), this, SLOT(CreatePuzzleBtn_clicked()));
    connect(mBackBtn, SIGNAL(clicked(bool)), this, SLOT(BackBtn_clicked()));
}

void StartMenu::NewPuzzleBtn_clicked()
{
    mMenus->setCurrentIndex(MainMenuType::MMT_NewPuzzle);
}

void StartMenu::LoadPuzzleBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open file", "", "*.sudoku");
    if(!fileName.isEmpty())
    {
        std::unique_ptr<PuzzleData> pd;
        if(SaveLoadManager::Get()->LoadSudoku(fileName.toStdString(), pd))
        {
            close();
            deleteLater();
            MainWindow* w = new MainWindow(pd->mSize, std::move(pd), fileName);
            w->show();
        }
    }
}

void StartMenu::CreatePuzzleBtn_clicked()
{
    close();
    deleteLater();
    MainWindow* w = new MainWindow(mPuzzleSizeSpinbox->value());
    w->show();
}

void StartMenu::BackBtn_clicked()
{
    mMenus->setCurrentIndex(MainMenuType::MMT_Start);
}
