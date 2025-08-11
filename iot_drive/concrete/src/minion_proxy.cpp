#include "network_utils.h"      // SocketCreate
#include "minion_proxy.hpp"     // MinionProxy
#include <iostream>
using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/****************************** Minion Proxy **********************************/

MinionProxy::MinionProxy(u_int16_t port, const char* ip)
{
    m_udp = SocketCreate(SOCK_DGRAM, &m_minionAdd, port);

    ConvertIP(m_udp, ip, &m_minionAdd);
}

MinionProxy::~MinionProxy()
{
    SocketClose(m_udp);
}

void MinionProxy::Read(const Uid& uid, size_t offset, size_t size)
{ 
    ReadMessage msg_obj(uid, offset, size);
		
	SerializeSend(msg_obj);
}

void MinionProxy::Write(const Uid& uid, size_t offset, size_t size, const char* buffer)
{
    WriteMessage msg_obj(uid, offset, size, buffer);
	SerializeSend(msg_obj);
}

/**************************** Private Function ********************************/

void MinionProxy::SerializeSend(AMessage& msg_obj)
{
    auto size = msg_obj.GetMessageSize();

    char* buffer = new char[size];
    
    msg_obj.ToBuffer(buffer);

    UDPSendTo(m_udp, buffer, size, &m_minionAdd);
    
    delete[] buffer;
}
}