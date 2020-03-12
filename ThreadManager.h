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

void printThreadSafe(const std::string& str);

class ThreadManager : public QObject
{
    Q_OBJECT
public:
    ThreadManager(QObject *parent);
    void startThreads(int generalThreadsCount, int requestThreadsCount); // Start general and requested threads
    void startGeneralThreads(int generalThreadsCount); // start general threads which create files one by one
    void startRequestedThreads(int requestedThreadsCount); // start requested threads which create files with priority
	void stopAllThreads();
    void regenerateFiles(int generalThreadsCount, int requestThreadsCount);
	void joinGeneralThreads();
	void joinRequestedThreads();
    void addRequest(int requestedFileId); // simulation of client request
    bool threadsStopped();

private:
	void fillFilesId();
	void clearFilesId();
	void createDestinationFolder();
    bool emptyGeneralFileIdQueue();
    bool emptyRequestedFileIdQueue();
	bool needFileCreation(int fileId);
    int getGeneralFileIdToCreate();
    int getRequestedFileIdToCreate();
	void createFile(int fileId);
	void removeCreatedFiles();
	void generalFileCreation();
    void requestedFileCreation();

	std::vector<std::thread> mGeneralThreads;
    std::vector<std::thread> mRequestedThreads;
    std::vector<int> mGeneralFileIdQueue;
    std::vector<int> mRequestedFileIdQueue;
    std::mutex mGeneralFileIdMutex;
    std::mutex mRequestedFileIdMutex;
	std::atomic<bool> mStopThreads;
    std::condition_variable mRequestedQueueConditionVariable;
	std::string mPath;
	const int mFilesCount = 100;
};

#endif // THREAD_MANAGER_H
