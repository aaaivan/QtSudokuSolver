#include "regionseditingcontrols.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>

RegionsEditingControls::RegionsEditingControls(QWidget *parent)
    : QWidget{parent},
      mRegionSelect(new QComboBox())
{
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    this->setLayout(verticalLayout);
    QWidget* formWidget = new QWidget();
    verticalLayout->addWidget(formWidget);

    QFormLayout* formLayout = new QFormLayout(formWidget);
    formWidget->setLayout(formLayout);

    QLabel* selectLabel = new QLabel("Region id:");
    formLayout->addRow(selectLabel, mRegionSelect);
}
