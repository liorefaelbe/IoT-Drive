#include <stdexcept>                // std::runtime_error 

#include "network_utils.h"          // SocketCreate
#include "handleton.hpp"            // Handleton
#include "factory.hpp"              // Factory
#include "minion_read_args.hpp"     // MinionReadArg
#include "minion_write_args.hpp"    // MinionWriteArgs
#include "master_proxy.hpp"         // MasterProxy
#include <iostream>
using namespace std;
namespace ilrd_166_7
{

static const int PORT_SEND = 8080;

#ifndef PORT_REC
    #define PORT_REC 8000
#endif

#ifndef FILE_PATH
    #define FILE_PATH "./minions/minion_1.txt"
#endif

/**************************** Implementations *********************************/
/****************************** MasterProxy ***********************************/

MasterProxy::MasterProxy() : m_file(FILE_PATH)
{
    m_udp_rec = SocketCreate(SOCK_DGRAM, &m_minionAdd, PORT_REC);
    SocketBind(m_udp_rec, &m_minionAdd);

    m_udp_send = SocketCreate(SOCK_DGRAM, &m_masterAdd, PORT_SEND);
    ConvertIP(m_udp_send, "127.0.0.1", &m_masterAdd);
}

MasterProxy::~MasterProxy()
{
    SocketClose(m_udp_rec);
    SocketClose(m_udp_send);
}

shared_ptr<IKeyArgs> MasterProxy::GetKeyArgs(Reactor::fd_mode key)
{
    (void)key;

    auto msg = GetMessageObj();

    if (msg->GetKey() == AMessage::MSG_TYPE::READ)
    {
        return (make_shared<MinionReadArgs>(msg->GetUID() ,msg->GetOffset(), msg->GetSize()));
    }
    else
    {
        return (make_shared<MinionWriteArgs>(msg->GetUID() ,msg->GetOffset(), msg->GetSize(), dynamic_pointer_cast<WriteMessage>(msg)->GetBuffer()));
    }
}

int MasterProxy::GetUDP() const
{
    return (m_udp_rec);
}

FileProxy& MasterProxy::GetFile()
{
    return (m_file);
}

void MasterProxy::SendResponse(shared_ptr<AResponseMessage> msg)
{
    auto size = msg->GetMessageSize();

    char* buffer = new char[size];
    
    msg->ToBuffer(buffer);

    UDPSendTo(m_udp_send, buffer, size, &m_masterAdd);
    
    delete[] buffer;
}
/**************************** Private Method **********************************/

shared_ptr<AMessage>  MasterProxy::GetMessageObj()
{
    uint32_t size = 0;
    UDPReceiveFrom(m_udp_rec, (char*)&size, sizeof(uint32_t), MSG_PEEK, nullptr);

    char* buffer = new char[size];
    UDPReceiveFrom(m_udp_rec, buffer, size, 0, nullptr);
    
    auto msgKey = *(AMessage::MSG_TYPE*)(buffer + sizeof(uint32_t));
    auto msg = Handleton<Factory<AMessage, AMessage::MSG_TYPE>>::GetInstance()->Create(msgKey);

    msg->FromBuffer(buffer);

    delete[] buffer;

    return (msg);
}
}