#ifndef EDITGRIDCONTROLS_H
#define EDITGRIDCONTROLS_H

#include "mainwindowcontent.h"
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>

class EditGridControls : public QWidget
{
    Q_OBJECT
public:
    explicit EditGridControls(MainWindowContent* mainWindowContent, QWidget *parent = nullptr);

private:
    MainWindowContent* mMainWindowContent;
    QPushButton* mAddDigitsBtn;
    QPushButton* mDrawRegionsBtn;
    QPushButton* mDrawKillersBtn;
    QCheckBox* mPositiveDiagonalCheckbox;
    QCheckBox* mNegativeDiagonalCheckbox;

    MainWindowContent::ViewType GetViewForButton(QPushButton* btn);
    void OnViewButtonChecked(QPushButton* btn);

private slots:
    void AddDigitsBtn_Toggled(bool checked);
    void DrawRegionsBtn_Toggled(bool checked);
    void DrawKillersBtn_Toggled(bool checked);
    void PositiveDiagonalCheckbox_OnChange(int checked);
    void NegativeDiagonalCheckbox_OnChange(int checked);
};

#endif // EDITGRIDCONTROLS_H
