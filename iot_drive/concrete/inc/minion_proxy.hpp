#ifndef __ILRD_MINION_PROXY_HPP__
#define __ILRD_MINION_PROXY_HPP__

#include <arpa/inet.h>          // sockaddr_in

#include "message.hpp"          // AMessage
#include "iminion_proxy.hpp"    // IMinionProxy

/**
 * @class  MinionProxy
 * @brief  Menages operation on the remote minions
 * 
 * @author Lior Reafael Berkovits
 * @date   26/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class MinionProxy : public IMinionProxy
{
public:
    explicit MinionProxy(u_int16_t port = 8080, const char* ip = "127.0.0.1");
    ~MinionProxy();

    void Read(const Uid& uid, size_t offset, size_t size) override;
    void Write(const Uid& uid, size_t offset, size_t size, const char* buffer) override;

private:
    struct sockaddr_in m_minionAdd{};
    int m_udp;

    void SerializeSend(AMessage& msg_obj);
}; // MinionProxy
} // namespace ilrd_166_7

#endif // __ILRD_MINION_PROXY_HPP__