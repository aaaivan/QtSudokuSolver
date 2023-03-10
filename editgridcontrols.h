#ifndef EDITGRIDCONTROLS_H
#define EDITGRIDCONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>

class MainWindowContent;

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

    void OnViewButtonChecked(QPushButton* btn);

private slots:
    void ViewButtonClicked(int btnId);
    void PositiveDiagonalCheckbox_OnChange(int checked);
    void NegativeDiagonalCheckbox_OnChange(int checked);
};

#endif // EDITGRIDCONTROLS_H
