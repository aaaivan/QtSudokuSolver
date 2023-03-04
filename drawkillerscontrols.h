#ifndef DRAWKILLERSCONTROLS_H
#define DRAWKILLERSCONTROLS_H

#include <QWidget>
#include <QStackedLayout>

class DrawKillersControls : public QWidget
{
    Q_OBJECT
public:
    explicit DrawKillersControls(QWidget *parent = nullptr);

private slots:
    void NewKillerBtn_Clicked();
    void EditKillerBtn_Clicked();
    void ClearKillersBtn_Clicked();

private:
    enum MenuView: int
    {
        ControlButtons,
        AddKiller,
        EditKiller
    };

    QStackedLayout* mStackedLayout;

    void showEvent(QShowEvent* event) override;
};

#endif // DRAWKILLERSCONTROLS_H
