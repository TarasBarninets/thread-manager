#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#pragma once
#include <thread>
#include <vector>
#include <numeric>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <QObject>
#include <QTime>

extern std::mutex coutMutex;
void printThreadSafe(const std::string& str);

class ThreadManager : public QObject
{
    Q_OBJECT
public:
    ThreadManager(QObject *parent);
    void startThreads(int generalThreadsCount, int requestThreadsCount);    // start general and requested threads
    void startGeneralThreads(int generalThreadsCount);                      // start general threads which create files one by one
    void startRequestedThreads(int requestedThreadsCount);                  // start requested threads which create files with priority
    void stopAllThreads();                                                  // stop general and requested threads
    void regenerateFiles(int generalThreadsCount, int requestThreadsCount); // remove all files and creates new one
    void joinGeneralThreads();                                              // join general threads
    void joinRequestedThreads();                                            // join requested threads
    void detachGeneralThreads();                                            // detach general threads
    void detachRequestedThreads();                                          // detach requested threads
    int getNumRuningThreads() const;                                        // return count of runing threads

signals:
    void fileCreated(int fileId, QTime time, QString path);

public slots:
     void createRequestedFile(int requestedFileId);                         // created requested File

private:
    void fillGeneralFilesId();                                              // fill vector mGeneralFile - Files ID
    void clearGeneralFilesId();                                             // clear vector mGeneralFile
    void createDestinationFolder();                                         // create folder GeneretedTxtFiles - where
    bool emptyGeneralFile();                                                // return empty of mGeneralFile
    bool needFileCreation(int fileId);                                      // return true if file not created in general thread
    int fetchGeneralFileId();                                               // return File ID that should be generated and remove from mGeneralFile
    void createFile(int fileId);                                            // create txt file
    void removeCreatedFiles();                                              // remove all created files

    void generalFileCreation();                                             // thread function for general thread
    void requestedFileCreation();                                           // thread function for requested thread

    std::vector<std::thread> mGeneralThreads;                               // vector of general threads
    std::vector<std::thread> mRequestedThreads;                             // vector of requested threads - thread pool
    std::vector<int> mGeneralFile;                                          // vector of File ID that created in general threads
    std::vector<int> mRequestedFile;                                        // vector of File ID that created in requested threads - thread pool
    std::mutex mGeneralFileMutex;                                           // mutex for synchronize access to mGeneralFile
    std::mutex mRequestedFileMutex;                                         // mutex for synchronize access to mRequestedFile
    std::atomic<bool> mStopThreads;                                         // atomic variable that signals threads to finish execution
    std::atomic<int> mNumRunningThreads;                                    // atomic variable - contain a number of running threads
    std::condition_variable mRequestedQueueConditionVariable;               // conditional variable - notify thread from pool to create requested file
    std::string mPath;                                                      // path where created files
    const size_t mFilesCount = 100;                                         // quantity of files that should be created
};

#endif // THREAD_MANAGER_H
