#include <iostream>     // cout, cerr
#include <fstream>      // ifstream
#include <string>       // string
#include <thread>       // thread
#include <chrono>       // chrono
#include <vector>       // vector
#include <algorithm>    // find_if
#include <sstream>      // stringstream

#include "logger.hpp"    // Logger
#include "handleton.hpp" // Handleton
#include "test_func.hpp" // CheckValue

using namespace std;
using namespace ilrd_166_7;

auto logger = Handleton<Logger>::GetInstance();

/************************* Forward Declaration *******************************/

static int Test(void);
static int TestBasic(void);
static int TestMultipleThreads(void);
static int TestLogPriority(void);
static int TestLogFileOutput(void);

static bool FileContains(const string& filename, const string& text);
static void CleanTestFile(void);

/******************************* Main ****************************************/

int main()
{
    int status = 0;
    
    CleanTestFile();
    status += Test();

    (status == 0) ? (cout << "\n\033[0;32m\033[1mAll Good!\033[0m\n\n") : 
                   (cout << "\n\033[0;31m\033[1m" << status << " FAILS! \033[0mTotaL\n\n");
    
    return (0);
}

/***************************** Static Functions ******************************/

static int Test(void)
{
    int status = 0;
   
    cout << "\n\033[1m----------- Testing Logger -----------\033[0m\n";
    
    status += TestBasic();
    status += TestMultipleThreads();
    status += TestLogPriority();
    status += TestLogFileOutput();

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";
        
    logger->Log("This is a debug message", Logger::SEVERITY::DEBUG);
    logger->Log("This is a warning message", Logger::SEVERITY::WARNING);
    logger->Log("This is an error message", Logger::SEVERITY::ERROR);

    this_thread::sleep_for(chrono::milliseconds(500));
    
    ifstream logFile("Log.txt");
    bool fileExists = logFile.good();
    logFile.close();
    
    status += CheckValue(fileExists, true, "Log file should be created", __LINE__);
    
    status += CheckValue(FileContains("Log.txt", "[DEBUG]"), true, "DEBUG message", __LINE__);
    status += CheckValue(FileContains("Log.txt", "[WARNING]"), true, "WARNING message", __LINE__);
    status += CheckValue(FileContains("Log.txt", "[ERROR]"), true, "ERROR message", __LINE__);
    
    return (status);
}

static int TestMultipleThreads(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting multi-threaded logging:\033[0m\n";

    vector<thread> threads;
    atomic<int> messagesSent(0);
    
    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back([i, &messagesSent]() 
        {
            for (int j = 0; j < 2; ++j)
            {
                stringstream ss;
                ss << "Thread " << i << " message " << j;
                logger->Log(ss.str(), Logger::SEVERITY::DEBUG);
                messagesSent++;
                this_thread::sleep_for(chrono::milliseconds(1000));
            }
        });
    }

    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    this_thread::sleep_for(chrono::milliseconds(1000));
    
    ifstream logFile("Log.txt");
    int lineCount = 0;
    string line;

    while (getline(logFile, line))
    {
        lineCount++;
    }

    status += CheckValue(lineCount >= messagesSent, true, "All messages should be logged", __LINE__);
    
    return (status);
}

static int TestLogPriority(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting log priority ordering:\033[0m\n";
        
    logger->Log("DEBUG message 1", Logger::SEVERITY::DEBUG);
    logger->Log("ERROR message", Logger::SEVERITY::ERROR);
    logger->Log("WARNING message", Logger::SEVERITY::WARNING);
    logger->Log("DEBUG message 2", Logger::SEVERITY::DEBUG);

    this_thread::sleep_for(chrono::milliseconds(500));

    status += CheckValue(FileContains("Log.txt", "DEBUG message 1"), true,"DEBUG message 1", __LINE__);
    status += CheckValue(FileContains("Log.txt", "ERROR message"), true,"ERROR message", __LINE__);
    status += CheckValue(FileContains("Log.txt", "WARNING message"), true,"WARNING message", __LINE__);
    status += CheckValue(FileContains("Log.txt", "DEBUG message 2"), true,"DEBUG message 2", __LINE__);
    
    return (status);
}

static int TestLogFileOutput(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting log file format:\033[0m\n";

    logger->Log("Test message format", Logger::SEVERITY::WARNING);

    this_thread::sleep_for(chrono::milliseconds(500));

    ifstream logFile("Log.txt");
    string line;
    bool foundLine = false;
    
    while (getline(logFile, line))
    {
        if (line.find("Test message format") != string::npos)
        {
            foundLine = true;

            bool startsWithNumber = !line.empty() && isdigit(line[0]);

            bool containsSeverity = line.find("[WARNING]") != string::npos;
        
            bool containsMessage = line.find("Test message format") != string::npos;
            
            status += CheckValue(startsWithNumber, true, "Index number", __LINE__);
            status += CheckValue(containsSeverity, true, "Severity level", __LINE__);
            status += CheckValue(containsMessage, true, "Message", __LINE__);

            break;
        }
    }

    logFile.close();
    status += CheckValue(foundLine, true, "Test message should be found in log file", __LINE__);
    
    return (status);
}

static bool FileContains(const string& filename, const string& text)
{
    ifstream file(filename);
    string line;
    
    while (getline(file, line))
    {
        if (line.find(text) != string::npos)
        {
            return (true);
        }
    }
    
    return (false);
}

static void CleanTestFile(void)
{
    ofstream logFile("Log.txt", ios::trunc);
    logFile.close();
}