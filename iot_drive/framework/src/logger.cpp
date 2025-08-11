#include <iostream>     // std::cerr
#include <cstring>      // std::strlen
#include <iomanip>      // std::put_time
#include <sstream>      // std::ostringstream

#include "logger.hpp"   // Logger

using namespace std;
namespace ilrd_166_7
{

const static Logger::SEVERITY END = static_cast<Logger::SEVERITY>(3);

/**************************** Implementations *********************************/
/******************************** Logger **************************************/
/***************************** Constructors ***********************************/

Logger::Logger() : m_log("Log.txt", ios::app)
{
    m_logThread = thread(&Logger::ThreadWrite, this);
    
    if (!m_log.is_open())
    {
        cerr << "Logger Ctor: Log file fail to open.\n";
    }
}

Logger::~Logger()
{
    Log("END", END);

    m_logThread.join();
}

/**************************** Private Functions *******************************/

void Logger::Log(const string& message, SEVERITY severity)
{
    lock_guard<mutex> lock(m_lock);

    LogData data{time(nullptr), severity, message};

    m_logQueue.Push(data);
}

void Logger::ThreadWrite()
{
    LogData data;
    size_t log_i = 1;
 
    do {
        m_logQueue.Pop(data);
        m_log << log_i << ": " << data() << endl;
      
        ++log_i;
    } while (data.m_severity != END);
}

/******************************** LogData *************************************/

string Logger::LogData::operator()(void) const
{
    ostringstream os;
    os << put_time(localtime(&m_timestamp), "%Y-%m-%d %H:%M:%S");
    string output(os.str());
    
    switch (m_severity)
	{
		case SEVERITY::DEBUG:
			output += " [DEBUG] ";
			break;
		case SEVERITY::WARNING:
            output += " [WARNING] ";
			break;
		case SEVERITY::ERROR:
            output += " [ERROR] ";
			break;
	}

	output += m_message;

    return (output);
}
}