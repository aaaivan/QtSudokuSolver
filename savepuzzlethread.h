#ifndef SAVEPUZZLETHREAD_H
#define SAVEPUZZLETHREAD_H

#include "puzzledata.h"
#include <QThread>
#include <QMutex>

class SavePuzzleThread : public QThread
{
    Q_OBJECT
public:
    explicit SavePuzzleThread(QObject *parent = nullptr);
    ~SavePuzzleThread();

signals:
    void SaveSuccessul(QString path);

private:
    std::optional<PuzzleData> mPuzzleData;
    QString mPath;
    bool mRestart;
    QMutex mSaveMutex;

protected:
    void run() override;

public:
    void SavePuzzle(const PuzzleData &&pd, QString path);

};

#endif // SAVEPUZZLETHREAD_H
