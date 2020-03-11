#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#pragma once
#include <thread>
#include <vector>
#include <numeric>
#include <mutex>
#include <atomic>
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
	void startGeneralThreads(int generalThreadsCount);
	void startRequestedThreads(int requestedFileId);
	void stopAllThreads();
	void regenerateFiles(int generalThreadsCount);
	void joinGeneralThreads();
	void joinRequestedThreads();

private:
	void fillFilesId();
	void clearFilesId();
	void createDestinationFolder();
	bool emptyFileIdToCreate();
	bool needFileCreation(int fileId);
	int getFileIdToCreate();
	void createFile(int fileId);
	void removeCreatedFiles();
	void generalFileCreation();
	void requestedFileCreation(int fileId);

	std::vector<std::thread> mGeneralThreads;
	std::vector <std::thread> mRequestedThreads;
	std::vector<int> mFileIdToCreate;
	std::mutex mFileIdMutex;
	std::atomic<bool> mStopThreads;
	std::string mPath;
	const int mFilesCount = 100;
};

#endif // THREAD_MANAGER_H
