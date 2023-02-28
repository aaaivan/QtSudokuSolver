#ifndef EDITGRIDCONTROLS_H
#define EDITGRIDCONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>

class EditGridControls : public QWidget
{
    Q_OBJECT
public:
    explicit EditGridControls(QWidget *parent = nullptr);

public slots:
    void OnPositiveDiagonalChanged(int checked);
    void OnNegativeDiagonalChanged(int checked);

private:
    QPushButton* addDigitsBtn;
    QPushButton* drawRegionsBtn;
    QPushButton* drawKillersBtn;
    QCheckBox* positiveDiagonalCheckbox;
    QCheckBox* negativeDiagonalCheckbox;
};

#endif // EDITGRIDCONTROLS_H
