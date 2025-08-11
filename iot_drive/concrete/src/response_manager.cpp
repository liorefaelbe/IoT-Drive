#include <cstring>                  // memcpy
#include <iostream>                 // std::cerr

#include "colors.hpp"               // RED
#include "network_utils.h"          // SocketCreate
#include "handleton.hpp"            // Handleton
#include "factory.hpp"              // Factory
#include "nbd_proxy.hpp"            // NBDProxy
#include "response_manager.hpp"     // ResponseManager

using namespace std;
namespace ilrd_166_7
{

static const int PORT = 8080;

/**************************** Implementations *********************************/
/**************************** ResponseManager *********************************/

ResponseManager::ResponseManager() : m_reactor(&m_listener) 
{
    m_udp = SocketCreate(SOCK_DGRAM, &m_masterAdd, PORT);
    SocketBind(m_udp, &m_masterAdd);

    m_reactor.Subscribe(m_udp, Reactor::MODE::READ, [this](int fd, Reactor::MODE mode) { GotResponse(fd, mode); });

    m_thread = thread([this]() { m_reactor.Run(); });
}
    
ResponseManager::~ResponseManager()
{
    SocketClose(m_udp);

    m_reactor.Stop();

    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void ResponseManager::RegisterUID(const Uid& commandUid, const char* nbdUid)
{
	unique_lock<mutex> lock(m_lock);
	
    array<char, 8> handle_array;
    memcpy(handle_array.data(), nbdUid, 8);
    m_commandUidMap.insert({commandUid, handle_array});
}

bool ResponseManager::IsUIDExists(const Uid& commandUid)
{
    unique_lock<mutex> lock(m_lock);
	auto it = m_commandUidMap.find(commandUid);
	if (it == m_commandUidMap.end())
	{
		return (false);
	}
	return (true);
}

void ResponseManager::GotResponse(int fd, Reactor::MODE mode)
{
    (void)mode;
    (void)fd;
    try 
	{
        auto msg = GetMessageObj();

        auto nbdProxy = Handleton<NBDProxy>::GetInstance();

        {
            lock_guard<mutex> lock(m_lock);
            auto nbdUid = m_commandUidMap.at(msg->GetUID()).data();

            if (msg->GetKey() == AResponseMessage::MSG_TYPE::READ)
            {
                nbdProxy->ReadResponse(nbdUid, msg->GetSize(), dynamic_pointer_cast<ResponseReadMessage>(msg)->GetBuffer());
            } 

            m_commandUidMap.erase(msg->GetUID());
        }
    }
	catch (const std::exception& e) 
	{
        cerr << RED << "GotResponse - Exception Message: " <<  e.what() << RESET << endl;
	}
}

/**************************** Private Method **********************************/

shared_ptr<AResponseMessage> ResponseManager::GetMessageObj()
{
    uint32_t size = 0;
    UDPReceiveFrom(m_udp, (char*)&size, sizeof(uint32_t), MSG_PEEK, nullptr);

    vector<char> buffer(size);
    UDPReceiveFrom(m_udp, buffer.data(), size, 0, nullptr);

    auto msgKey = *(AResponseMessage::MSG_TYPE*)(buffer.data() + sizeof(uint32_t));
    auto msg = Handleton<Factory<AResponseMessage, AResponseMessage::MSG_TYPE>>::GetInstance()->Create(msgKey);

    msg->FromBuffer(buffer.data());

    return (msg);
}
}