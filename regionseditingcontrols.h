#ifndef REGIONSEDITINGCONTROLS_H
#define REGIONSEDITINGCONTROLS_H

#include <QWidget>
#include <QComboBox>

class RegionsEditingControls : public QWidget
{
    Q_OBJECT
public:
    explicit RegionsEditingControls(QWidget *parent = nullptr);

private:
    QComboBox* mRegionSelect;
};

#endif // REGIONSEDITINGCONTROLS_H
