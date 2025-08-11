#ifndef __ILRD_MASTER_PROXY_HPP__
#define __ILRD_MASTER_PROXY_HPP__

#include <arpa/inet.h>          // sockaddr_in

#include "message.hpp"          // AMessage
#include "response_message.hpp" // AResponseMessage
#include "file_proxy.hpp"       // FileProxy
#include "framework.hpp"        // IInputProxy

/**
 * @class  MasterProxy
 * @brief  Menages operation sent from the master proccess
 * 
 * @author Lior Reafael Berkovits
 * @date   03/06/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class MasterProxy : public IInputProxy
{
public:
    explicit MasterProxy();
    ~MasterProxy();

    std::shared_ptr<IKeyArgs> GetKeyArgs(Reactor::fd_mode key) override;

    int GetUDP() const;
    FileProxy& GetFile();

    void SendResponse(std::shared_ptr<AResponseMessage> msg);

private:
    struct sockaddr_in m_minionAdd{};
    struct sockaddr_in m_masterAdd{};
    int m_udp_rec;
    int m_udp_send;

    FileProxy m_file;

    std::shared_ptr<AMessage> GetMessageObj();
}; // MasterProxy
} // namespace ilrd_166_7

#endif // __ILRD_MASTER_PROXY_HPP__