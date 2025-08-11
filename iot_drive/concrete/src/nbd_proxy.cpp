#include <iostream>					// std::cout
#include <mutex>					// std::mutex

#include "network_utils.h"          // SocketCreate
#include "nbd_proxy.hpp"            // NBDProxy
#include "colors.hpp"				// RESET

using namespace std;
namespace ilrd_166_7
{

// static int read_req_num = 1;
// static int write_req_num = 1;

/**************************** Implementations *********************************/
/******************************* NBDProxy *************************************/

shared_ptr<IKeyArgs> NBDProxy::GetKeyArgs(Reactor::fd_mode key)
{
	(void)key;

    return (m_nbd.GetKeyArgs());
}

int NBDProxy::GetFD()
{
	return (m_nbd.GetFD());
}

void NBDProxy::ReadResponse(char nbdUid[8], uint32_t size, const char* buffer) 
{
	// cout << MAGENTA << "NBDProxy::ReadResponse #" << read_req_num << " | nbdUid: " << *(size_t*)nbdUid << RESET << endl;
	// ++read_req_num;
	m_nbd.Respond(nbdUid, size, buffer);
}

void NBDProxy::StatusResponse(char nbdUid[8], uint8_t status) 
{
	// cout << MAGENTA << "NBDProxy::StatusResponse - #" << write_req_num << " | nbdUid: " << *(size_t*)nbdUid << RESET << endl;
	// ++write_req_num;
	m_nbd.Respond(nbdUid, status);
} 
}