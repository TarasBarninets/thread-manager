#include "ThreadManager.h"
#include <iostream>

std::mutex coutMutex;
void printThreadSafe(const std::string& str)
{
	std::lock_guard<std::mutex> lock(coutMutex);
	std::cout << str << std::endl;
}

ThreadManager::ThreadManager(QObject *parent) : QObject(parent)
{
	fillFilesId();
	createDestinationFolder();
}

void ThreadManager::startGeneralThreads(int generalThreadsCount)
{
	mStopThreads = false;
	for (int i = 0; i < generalThreadsCount; i++)
	{
		mGeneralThreads.push_back(std::thread(&ThreadManager::generalFileCreation, this));
	}
	printThreadSafe(__FUNCSIG__);
}

void ThreadManager::startRequestedThreads(int requestedFileId)
{
	bool fileGenereted = needFileCreation(requestedFileId);
	if (fileGenereted)
	{
		mRequestedThreads.push_back(std::thread(&ThreadManager::requestedFileCreation, this, requestedFileId));
	}
	printThreadSafe(__FUNCSIG__);
}

void ThreadManager::stopAllThreads()
{
	mStopThreads = true;
	joinGeneralThreads();
	mGeneralThreads.clear();
	joinRequestedThreads();
	mRequestedThreads.clear();
}

void ThreadManager::regenerateFiles(int generalThreadsCount)
{
	stopAllThreads();
	removeCreatedFiles();
	clearFilesId();
	fillFilesId();
	createDestinationFolder();
	startGeneralThreads(generalThreadsCount);
}

void ThreadManager::joinGeneralThreads()
{
	for (std::thread& thread : mGeneralThreads)
	{
		if (thread.joinable())
		{
			thread.join();

			std::stringstream ss;
			ss << "Joined thread ID - " << thread.get_id() << std::endl;
			printThreadSafe(ss.str());
		}
	}
}

void ThreadManager::joinRequestedThreads()
{
	for (std::thread& thread : mRequestedThreads)
	{
		if (thread.joinable())
		{
			thread.join();

			std::stringstream ss;
			ss << "Joined thread ID - " << thread.get_id() << std::endl;
			printThreadSafe(ss.str());
		}
	}
}

void ThreadManager::fillFilesId()
{
	mFileIdToCreate.resize(mFilesCount);
	std::iota(mFileIdToCreate.begin(), mFileIdToCreate.end(), 1);
	printThreadSafe(__FUNCSIG__);
}

void ThreadManager::clearFilesId()
{
	std::lock_guard<std::mutex> lock(mFileIdMutex);
	mFileIdToCreate.clear();
}

void ThreadManager::createDestinationFolder()
{
	const std::string folderName = "\\GeneretedTxtFiles\\";
	const std::string relativePath = std::filesystem::current_path().string();
	mPath = relativePath + folderName;
	std::filesystem::create_directory(mPath);
	printThreadSafe(__FUNCSIG__);
}

bool ThreadManager::emptyFileIdToCreate()
{
	std::lock_guard<std::mutex> lock(mFileIdMutex);
	return mFileIdToCreate.empty();
}

bool ThreadManager::needFileCreation(int fileId)
{
	std::lock_guard<std::mutex> lock(mFileIdMutex);
	auto it = std::find(mFileIdToCreate.begin(), mFileIdToCreate.end(), fileId);
	if (it == mFileIdToCreate.end())
	{
		std::stringstream ss;
		ss << "Requested file already genereted" << std::endl;
		printThreadSafe(ss.str());
		return false;
	}
	else
	{
		mFileIdToCreate.erase(it);
	}
	return true;
}

int ThreadManager::getFileIdToCreate()
{
	std::lock_guard<std::mutex> lock(mFileIdMutex);
	int fileId = mFileIdToCreate.back();
	mFileIdToCreate.pop_back();
	return fileId;
}

void ThreadManager::createFile(int fileId)
{
	std::ofstream outfile;
    outfile.open(mPath + std::to_string(fileId) + ".txt");//, std::ios::app

	int tmp = fileId;
	while (tmp > 0)
	{
		outfile << fileId << std::endl;
		--tmp;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(50 * fileId)); // 100 * fileId - range [0.1, 10] sec
	outfile.close();
}

void ThreadManager::removeCreatedFiles()
{
	joinGeneralThreads(); // wait when all threads finished
	std::filesystem::remove_all(mPath);
	printThreadSafe(__FUNCSIG__);
}

void ThreadManager::generalFileCreation() // General thread function
{
	bool empty = emptyFileIdToCreate();
	while (!empty && !mStopThreads)
	{
		int fileId = getFileIdToCreate();
		createFile(fileId);

		std::stringstream ss;
		ss << "General thread genereted file : " << fileId << ".txt , Thread ID = " << std::this_thread::get_id() 
		<< " , spent seconds - " << ((100.0 * fileId) / 1000.0);
		printThreadSafe(ss.str());

		empty = emptyFileIdToCreate();
	}

	std::stringstream ss;
	ss << "Finished general thread, Thread ID = " << std::this_thread::get_id();
	printThreadSafe(ss.str());
}

void ThreadManager::requestedFileCreation(int fileId) // Requested thread function
{
	createFile(fileId);
	std::stringstream ss1;
	ss1 << "Requested thread genereted file : " << fileId << ".txt , Thread ID = " << std::this_thread::get_id();
	printThreadSafe(ss1.str());
	
	std::stringstream ss2;
	ss2 << "Finished requested thread, Thread ID = " << std::this_thread::get_id();
	printThreadSafe(ss2.str());
}
