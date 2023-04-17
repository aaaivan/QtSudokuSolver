#include "savepuzzlethread.h"
#include "IO/SaveLoadManager.h"

SavePuzzleThread::SavePuzzleThread(QObject *parent)
    : QThread{parent}
    , mPuzzleData()
    , mPath()
    , mRestart(false)
    , mSaveMutex()
{
}

SavePuzzleThread::~SavePuzzleThread()
{
    wait();
}

void SavePuzzleThread::run()
{
    forever
    {
        QMutexLocker locker(&mSaveMutex);
        mRestart = false;
        if(!mPuzzleData.has_value())
        {
            return;
        }
        PuzzleData p = mPuzzleData.value();
        std::string path = mPath.toStdString();
        locker.unlock();

        // Do the saving
        if(SaveLoadManager::Get()->SaveSudoku(path, p))
        {
            emit SaveSuccessul(path.c_str());
        }

        locker.relock();
        if(!mRestart)
        {
            return;
        }
    }
}

void SavePuzzleThread::SavePuzzle(const PuzzleData &&pd, QString path)
{
    if(!isRunning())
    {
        QMutexLocker locker(&mSaveMutex);
        mPuzzleData = pd;
        mPath = path;
        start(HighestPriority);
    }
    else
    {
        QMutexLocker locker(&mSaveMutex);
        mPuzzleData = pd;
        mPath = path;
        mRestart = true;
    }
}
