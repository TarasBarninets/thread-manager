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

void ThreadManager::startThreads(int generalThreadsCount, int requestThreadsCount)
{
    startGeneralThreads(generalThreadsCount);
    startRequestedThreads(requestThreadsCount);
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

void ThreadManager::startRequestedThreads(int requestedThreadsCount)
{
    mStopThreads = false;
    for (int i = 0; i < requestedThreadsCount; i++)
    {
        mRequestedThreads.push_back(std::thread(&ThreadManager::requestedFileCreation, this));
    }
	printThreadSafe(__FUNCSIG__);
}

void ThreadManager::stopAllThreads()
{
	mStopThreads = true;
    joinGeneralThreads();
	mGeneralThreads.clear();
    mRequestedQueueConditionVariable.notify_all();
	joinRequestedThreads();
	mRequestedThreads.clear();
}

void ThreadManager::regenerateFiles(int generalThreadsCount, int requestThreadsCount)
{
    if(mStopThreads)
    {
        stopAllThreads();
        clearFilesId();
    }

	removeCreatedFiles();
	fillFilesId();
	createDestinationFolder();
	startGeneralThreads(generalThreadsCount);
    startRequestedThreads(requestThreadsCount);
}

void ThreadManager::joinGeneralThreads()
{
	for (std::thread& thread : mGeneralThreads)
	{
		if (thread.joinable())
		{
			thread.join();

			std::stringstream ss;
            ss << "Joined general thread ID - " << thread.get_id() << std::endl;
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
            ss << "Joined requested thread ID - " << thread.get_id();
			printThreadSafe(ss.str());
		}
	}
}

void ThreadManager::addRequest(int requestedFileId) // executed in Main thread
{
    bool isNeeded = needFileCreation(requestedFileId);
    if(isNeeded)
    {
        mRequestedQueueConditionVariable.notify_one();
    }
}

bool ThreadManager::threadsStopped()
{
    return mStopThreads;
}

void ThreadManager::fillFilesId()
{
    mGeneralFileIdQueue.resize(mFilesCount);
    std::iota(mGeneralFileIdQueue.begin(), mGeneralFileIdQueue.end(), 1);
	printThreadSafe(__FUNCSIG__);
}

void ThreadManager::clearFilesId()
{
    std::lock_guard<std::mutex> lock(mGeneralFileIdMutex);
    mGeneralFileIdQueue.clear();
}

void ThreadManager::createDestinationFolder()
{
	const std::string folderName = "\\GeneretedTxtFiles\\";
	const std::string relativePath = std::filesystem::current_path().string();
	mPath = relativePath + folderName;
	std::filesystem::create_directory(mPath);
	printThreadSafe(__FUNCSIG__);
}

bool ThreadManager::emptyGeneralFileIdQueue()
{
    std::lock_guard<std::mutex> lock(mGeneralFileIdMutex);
    if(mGeneralFileIdQueue.empty())
    {
        mStopThreads = true;
        mRequestedQueueConditionVariable.notify_all();
        return true;
    }
    return false;
}

bool ThreadManager::emptyRequestedFileIdQueue()
{
    std::lock_guard<std::mutex> lock(mRequestedFileIdMutex);
    return mRequestedFileIdQueue.empty();
}

bool ThreadManager::needFileCreation(int fileId)
{
    {
        std::lock_guard<std::mutex> lockGeneral(mGeneralFileIdMutex);
        auto it = std::find(mGeneralFileIdQueue.begin(), mGeneralFileIdQueue.end(), fileId);
        if (it == mGeneralFileIdQueue.end()) // check if file already created with this fileId
        {
            std::stringstream ss;
            ss << "Requested file already genereted" << std::endl;
            printThreadSafe(ss.str());

            //std::lock_guard<std::mutex> lockRequested(mRequestedFileIdMutex);
            //mRequestedFileIdQueue.pop_back(); // remove from requested queue - don't need create

            return false;
        }
        else
        {
            mGeneralFileIdQueue.erase(it); // prevent creation in general thread
        }
    }

    std::lock_guard<std::mutex> lockRequested(mRequestedFileIdMutex);
    mRequestedFileIdQueue.push_back(fileId); // add fileId to requested queue
	return true;
}

int ThreadManager::getGeneralFileIdToCreate()
{
    std::lock_guard<std::mutex> lock(mGeneralFileIdMutex);
    int fileId = mGeneralFileIdQueue.back();
    mGeneralFileIdQueue.pop_back();
	return fileId;
}

int ThreadManager::getRequestedFileIdToCreate()
{
    std::lock_guard<std::mutex> lock(mRequestedFileIdMutex);
    int fileId = mRequestedFileIdQueue.back();
    mRequestedFileIdQueue.pop_back();
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
    std::this_thread::sleep_for(std::chrono::milliseconds(10 * fileId)); // 100 * fileId - range [0.1, 10] sec
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
    bool empty = emptyGeneralFileIdQueue();
	while (!empty && !mStopThreads)
	{
        int fileId = getGeneralFileIdToCreate();
		createFile(fileId);

		std::stringstream ss;
		ss << "General thread genereted file : " << fileId << ".txt , Thread ID = " << std::this_thread::get_id() 
		<< " , spent seconds - " << ((100.0 * fileId) / 1000.0);
		printThreadSafe(ss.str());

        empty = emptyGeneralFileIdQueue();
	}

	std::stringstream ss;
	ss << "Finished general thread, Thread ID = " << std::this_thread::get_id();
	printThreadSafe(ss.str());
}

void ThreadManager::requestedFileCreation() // Requested THREAD function
{
    while(!mStopThreads)
    {   
        std::unique_lock<std::mutex> lock(mRequestedFileIdMutex);
        mRequestedQueueConditionVariable.wait(lock, [&]
        {
            return !mRequestedFileIdQueue.empty() || mStopThreads;
        });

        if(!mRequestedFileIdQueue.empty())
        {
            int requestedFileId = mRequestedFileIdQueue.back();
            mRequestedFileIdQueue.pop_back();
            createFile(requestedFileId);

            std::stringstream ss;
            ss << "Requested thread genereted file : " << requestedFileId << ".txt , Thread ID = " << std::this_thread::get_id();
            printThreadSafe(ss.str());
        }
    }
	
    std::stringstream ss;
    ss << "Finished requested thread, Thread ID = " << std::this_thread::get_id();
    printThreadSafe(ss.str());
}
