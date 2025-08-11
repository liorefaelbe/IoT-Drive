#ifndef __ILRD_IMINION_PROXY_HPP__
#define __ILRD_IMINION_PROXY_HPP__

#include <unistd.h> // size_t

/**
 * @class  IMinionProxy
 * @brief  Interface to MinionProxy
 * 
 * @author Lior Reafael Berkovits
 * @date   26/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class IMinionProxy
{
public:
    virtual ~IMinionProxy() = default;

    virtual void Read(const Uid& uid, size_t offset, size_t len) = 0;
    virtual void Write(const Uid& uid, size_t offset, size_t len, const char* buffer) = 0;
}; // IMinionProxy
} // namespace ilrd_166_7

#endif // __ILRD_IMINION_PROXY_HPP__