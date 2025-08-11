#ifndef __ILRD_NBD_SERVER_HPP__
#define __ILRD_NBD_SERVER_HPP__

#include <memory>           // std::shared_ptr

#include "framework.hpp"    // IKeyArgs

/**
 * @class  NBDServer
 * @brief  Manages NBD device communication and handles NBD requests
 * 
 * @author Lior Reafael Berkovits
 * @date   11/06/2025
 */

/**************************** Implementations *********************************/

namespace ilrd_166_7
{
class NBDServer
{
public:
	NBDServer();
	~NBDServer();

	int GetFD();
	std::shared_ptr<IKeyArgs> GetKeyArgs();
	void Respond(char handle[8], uint32_t size, const char* buffer);
	void Respond(char handle[8], uint8_t status);

private:
	int SendStatus(char handle[8], uint32_t status);
	int m_sp[2];
    
}; // NBDServer
} // namespace ilrd_166_7

#endif // __ILRD_NBD_SERVER_HPP__