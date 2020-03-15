#include "ThreadManager.h"
#include <iostream>

std::mutex coutMutex;
void printThreadSafe(const std::string& str) // thread safe function - to print logs
{
	std::lock_guard<std::mutex> lock(coutMutex);
	std::cout << str << std::endl;
}

ThreadManager::ThreadManager(QObject *parent) : QObject(parent)
{
    fillGeneralFilesId();
	createDestinationFolder();
}

void ThreadManager::startThreads(int generalThreadsCount, int requestThreadsCount)
{
    startGeneralThreads(generalThreadsCount);
    startRequestedThreads(requestThreadsCount);
    printThreadSafe(__FUNCSIG__);
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

    // MAIN THREAD
    if(!mStopThreads)
    {
        //...
        mStopThreads = true;
        // ... MAIN THREAD IS WAITING std::thread -
        joinGeneralThreads();
        mGeneralThreads.clear();
        mRequestedQueueConditionVariable.notify_all();
        joinRequestedThreads();
        mRequestedThreads.clear();
    }
    printThreadSafe(__FUNCSIG__);
}

void ThreadManager::regenerateFiles(int generalThreadsCount, int requestThreadsCount)
{
    if(mStopThreads)
    {
        stopAllThreads();
        clearGeneralFilesId();
    }

	removeCreatedFiles();
    fillGeneralFilesId();
	createDestinationFolder();
	startGeneralThreads(generalThreadsCount);
    startRequestedThreads(requestThreadsCount);
    printThreadSafe(__FUNCSIG__);
}

void ThreadManager::joinGeneralThreads()
{
	for (std::thread& thread : mGeneralThreads)
	{
		if (thread.joinable())
		{
            thread.detach();

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
            thread.detach();

			std::stringstream ss;
            ss << "Joined requested thread ID - " << thread.get_id();
			printThreadSafe(ss.str());
		}
	}
}

void ThreadManager::createRequestedFile(int requestedFileId) // executed in Main thread
{
    bool isNeeded = needFileCreation(requestedFileId);
    if(isNeeded)
    {
        mRequestedQueueConditionVariable.notify_one();
    }
    printThreadSafe(__FUNCSIG__);
}

int ThreadManager::getNumRuningThreads() const
{
    return mNumRunningThreads;
}

void ThreadManager::fillGeneralFilesId()
{
    mGeneralFile.resize(mFilesCount);
    std::iota(mGeneralFile.begin(), mGeneralFile.end(), 1);
	printThreadSafe(__FUNCSIG__);
}

void ThreadManager::clearGeneralFilesId()
{
    std::lock_guard<std::mutex> lock(mGeneralFileMutex);
    mGeneralFile.clear();
    printThreadSafe(__FUNCSIG__);
}

void ThreadManager::createDestinationFolder()
{
	const std::string folderName = "\\GeneretedTxtFiles\\";
	const std::string relativePath = std::filesystem::current_path().string();
	mPath = relativePath + folderName;
	std::filesystem::create_directory(mPath);
	printThreadSafe(__FUNCSIG__);
}

bool ThreadManager::emptyGeneralFile()
{
    std::lock_guard<std::mutex> lock(mGeneralFileMutex);
    if(mGeneralFile.empty())
    {
        mStopThreads = true;
        mRequestedQueueConditionVariable.notify_all();
        return true;
    }
    return false;
}

bool ThreadManager::needFileCreation(int fileId)
{
    {
        std::lock_guard<std::mutex> lockGeneral(mGeneralFileMutex);
        auto it = std::find(mGeneralFile.begin(), mGeneralFile.end(), fileId);
        if (it == mGeneralFile.end()) // check if file already created with this File ID
        {
            std::stringstream ss;
            ss << "Requested file already genereted" << std::endl;
            printThreadSafe(ss.str());
            return false;
        }
        else
        {
            mGeneralFile.erase(it); // prevent creation in general thread
        }
    }

    std::lock_guard<std::mutex> lockRequested(mRequestedFileMutex);
    mRequestedFile.push_back(fileId); // add File ID to mRequestedFile vector
	return true;
}

int ThreadManager::getGeneralFileId()
{
    std::lock_guard<std::mutex> lock(mGeneralFileMutex);
    int fileId = mGeneralFile.back();
    mGeneralFile.pop_back();
	return fileId;
}

void ThreadManager::createFile(int fileId)
{
	std::ofstream outfile;
    outfile.open(mPath + std::to_string(fileId) + ".txt");

	int tmp = fileId;
    while (tmp > 0)
	{
		outfile << fileId << std::endl;
		--tmp;
	}
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * fileId)); // range [0.1; 10]
	outfile.close();
}

void ThreadManager::removeCreatedFiles()
{
	joinGeneralThreads(); // wait when all threads finished
	std::filesystem::remove_all(mPath);
	printThreadSafe(__FUNCSIG__);
}

void ThreadManager::generalFileCreation() // thread function for general thread
{
    mNumRunningThreads++;

    std::stringstream ss1;
    ss1 << "General thread started Thread ID = " << std::this_thread::get_id();
    printThreadSafe(ss1.str());

    bool empty = emptyGeneralFile();
	while (!empty && !mStopThreads)
	{
        int fileId = getGeneralFileId();
		createFile(fileId);

		std::stringstream ss;
		ss << "General thread genereted file : " << fileId << ".txt , Thread ID = " << std::this_thread::get_id() 
		<< " , spent seconds - " << ((100.0 * fileId) / 1000.0);
		printThreadSafe(ss.str());

        empty = emptyGeneralFile();
	}

	std::stringstream ss;
	ss << "Finished general thread, Thread ID = " << std::this_thread::get_id();
	printThreadSafe(ss.str());

    mNumRunningThreads--;
}

void ThreadManager::requestedFileCreation() // thread function for requested thread
{
    mNumRunningThreads++;

    while(!mStopThreads)
    {   
        std::unique_lock<std::mutex> lock(mRequestedFileMutex);
        mRequestedQueueConditionVariable.wait(lock, [&]
        {
            return !mRequestedFile.empty() || mStopThreads;
        });

        if(!mRequestedFile.empty())
        {
            int requestedFileId = mRequestedFile.back();
            mRequestedFile.pop_back();
            createFile(requestedFileId);

            std::stringstream ss;
            ss << "Requested thread genereted file : " << requestedFileId << ".txt , Thread ID = " << std::this_thread::get_id();
            printThreadSafe(ss.str());
        }
    }
	
    std::stringstream ss;
    ss << "Finished requested thread, Thread ID = " << std::this_thread::get_id();
    printThreadSafe(ss.str());

    mNumRunningThreads--;
}
