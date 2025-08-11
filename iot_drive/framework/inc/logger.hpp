#ifndef __ILRD_LOGGER_HPP__
#define __ILRD_LOGGER_HPP__

#include <string>           // std::string
#include <thread>           // std::thread
#include <ctime>            // std::time_t
#include <fstream>          // std::ofstream
#include <atomic>           // std::atomic_bool
#include <mutex>            // std::mutex

#include "w_q.hpp"          // WaitableQueue
#include "pq_adapter.hpp"   // PQAdapter

/**
 * @class  Logger
 * @brief  Thread-safe asynchronous logging system
 * 
 * Features:
 *   - Provides an asynchronous logging mechanism that writes log
 *     messages with timestamps and severity levels to a file and/or console.
 *   - Log messages are queued and processed in a separate thread to avoid
 *     blocking the calling thread.
 * 
 * @author Lior Reafael Berkovits
 * @date   06/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class Logger
{
public:
    explicit Logger();
    ~Logger();

    Logger(const Logger& other) = delete;
    Logger& operator=(const Logger& other) = delete;

    enum class SEVERITY {DEBUG, WARNING, ERROR};
    void Log(const std::string& message, SEVERITY severity);

private:
    struct LogData
    {
        std::string operator()(void) const;

        std::time_t m_timestamp;
        Logger::SEVERITY m_severity;
        std::string m_message;
    };

    WaitableQueue<LogData> m_logQueue;
    std::thread m_logThread;
    std::ofstream m_log;
    std::mutex m_lock;

    void ThreadWrite();
}; // Logger
} // namespace ilrd_166_7

#endif // __ILRD_LOGGER_HPP__