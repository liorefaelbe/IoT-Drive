#ifndef __ILRD_RESPONSE_MANAGER_HPP__
#define __ILRD_RESPONSE_MANAGER_HPP__

#include <unordered_map>        // std::unordered_map
#include <array>                // std::array
#include <arpa/inet.h>          // sockaddr_in
#include <mutex>                // std::mutex

#include "uid.hpp"              // Uid
#include "reactor.hpp"          // Reactor 
#include "nbd_proxy.hpp"        // NBDProxy
#include "response_message.hpp" // AResponseMessage

/**
 * @class  ResponseManager
 * @brief  Keeps track of execution of minions operations while informing the NBDProxy of request completion
 * 
 * @author Lior Reafael Berkovits
 * @date   05/06/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class ResponseManager
{
public:
    explicit ResponseManager();
    ~ResponseManager();

    void RegisterUID(const Uid& commandUid, const char* nbdUid);
    bool IsUIDExists(const Uid& commandUid);

private:
    std::mutex m_lock;
    std::unordered_map<Uid, std::array<char,8>> m_commandUidMap;

    struct sockaddr_in m_masterAdd{};
    int m_udp;

    LinuxListener m_listener;
    Reactor m_reactor;
    std::thread m_thread;

    std::shared_ptr<AResponseMessage> GetMessageObj();
    void GotResponse(int, Reactor::MODE);
    
}; // ResponseManager
} // namespace ilrd_166_7

#endif // __ILRD_RESPONSE_MANAGER_HPP__