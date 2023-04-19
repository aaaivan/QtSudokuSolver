#ifndef EDITGRIDCONTROLS_H
#define EDITGRIDCONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>

class MainWindowContent;
class PuzzleData;

class EditGridControls : public QWidget
{
    Q_OBJECT
public:
    explicit EditGridControls(MainWindowContent* mainWindowContent, const PuzzleData* loadedGrid = nullptr, QWidget *parent = nullptr);

private:


    MainWindowContent* mMainWindowContent;
    QButtonGroup* mButtonsGroup;
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

public:
    int SelectedButtonIdGet() const;
};

#endif // EDITGRIDCONTROLS_H
