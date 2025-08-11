#ifndef __ILRD_NBD_PROXY_HPP__
#define __ILRD_NBD_PROXY_HPP__

#include <memory>           // std::shared_ptr
#include <arpa/inet.h>      // socket

#include "uid.hpp"          // Uid
#include "framework.hpp"    // IInputProxy
#include "nbd_server.hpp"   // NBDServer, NBDTaskArgs

/**
 * @class  NBDProxy
 * @brief  Gets NBD requests and creates IKeyArgs* for the ICommand
 *         Manages communication between NBD driver and the master framework
 * 
 * @author Lior Reafael Berkovits
 * @date   27/05/2025
 */

namespace ilrd_166_7
{

class NBDProxy : public IInputProxy
{
public:
    explicit NBDProxy() = default;
    ~NBDProxy() = default;

    int GetFD();
    std::shared_ptr<IKeyArgs> GetKeyArgs(Reactor::fd_mode key) override;

    void ReadResponse(char nbdUid[8], u_int32_t size, const char* buffer);
    void StatusResponse(char nbdUid[8], uint8_t status);
    
private:
	NBDServer m_nbd;
}; // NBDProxy
} // namespace ilrd_166_7

#endif // __ILRD_NBD_PROXY_HPP__