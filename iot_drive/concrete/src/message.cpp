#include <cstring>      // memcpy
#include <stdexcept>    // invalid_argument exception

#include "message.hpp"  // AMessage

using namespace std;
namespace ilrd_166_7
{
    
/**************************** Implementations *********************************/
/******************************** AMessage ************************************/

AMessage::AMessage(AMessage::MSG_TYPE key, const Uid& uid) : m_key(key), m_uid(uid) { /* empty */ }

char* AMessage::ToBuffer(char* buffer)
{
	*(MSG_TYPE*)buffer = m_key;
	buffer += sizeof(m_key);
	
	buffer = m_uid.ToBuffer(buffer);

	return (buffer);
}

char* AMessage::FromBuffer(char* buffer)
{
	m_key = *(MSG_TYPE*)buffer;
	buffer += sizeof(m_key);

	buffer = m_uid.FromBuffer(buffer);

	return (buffer);
}

AMessage::MSG_TYPE AMessage::GetKey()
{
	return (m_key);
}

uint32_t AMessage::GetMessageSize()
{
	return (sizeof(uint32_t) + m_uid.GetSize());
}

Uid AMessage::GetUID()
{
	return (m_uid);
}

/******************************* Read Message *********************************/

ReadMessage::ReadMessage() { /* empty */ }

ReadMessage::ReadMessage(const Uid& uid, uint32_t offset, uint32_t size) : AMessage(AMessage::READ, uid), m_offset(offset), m_size(size) { /* empty */ }

char* ReadMessage::ToBuffer(char* buffer)
{
	*(uint32_t*)buffer = GetMessageSize();
	buffer += sizeof(uint32_t);

	buffer = AMessage::ToBuffer(buffer);

	*(uint32_t*)buffer = m_offset;
	buffer += sizeof(m_offset);

	*(uint32_t*)buffer = m_size;
	buffer += sizeof(m_size);
	
	return (buffer);
}

char* ReadMessage::FromBuffer(char* buffer)
{
	buffer += sizeof(uint32_t);

	buffer = AMessage::FromBuffer(buffer);

	m_offset = *(uint32_t*)buffer; 
	buffer += sizeof(m_offset);

	m_size = *(uint32_t*)buffer;
	buffer+= sizeof(m_size);
	
	return (buffer); 
}

uint32_t ReadMessage::GetMessageSize()
{
	return (sizeof(uint32_t) + AMessage::GetMessageSize() + sizeof(m_offset) + sizeof(m_size));		
}

uint32_t ReadMessage::GetOffset()
{
	return (m_offset);
}

uint32_t ReadMessage::GetSize()
{
	return (m_size);
}

/****************************** Write Message *********************************/

WriteMessage::WriteMessage() { /* empty */ }

WriteMessage::WriteMessage(const Uid& uid, uint32_t offset, uint32_t size, const char* buffer) : AMessage(AMessage::WRITE, uid), m_offset(offset), m_size(size)
{
	m_buffer = new char[size];

	if (buffer == nullptr)
	{
		throw invalid_argument("Received nullptr");
	}
	
	memcpy(m_buffer, buffer, size);
}

WriteMessage::~WriteMessage()
{
	delete[] m_buffer;
}

char* WriteMessage::ToBuffer(char* buffer)
{
	*(uint32_t*)buffer = GetMessageSize();
	buffer += sizeof(uint32_t);
		
	buffer = AMessage::ToBuffer(buffer);

	*(uint32_t*)buffer = m_offset;
	buffer += sizeof(m_offset); 

	*(uint32_t*)buffer = m_size;
	buffer += sizeof(m_size);
	
	memcpy(buffer, m_buffer, m_size);
	
	return (buffer + m_size);
}

char* WriteMessage::FromBuffer(char* buffer)
{
	buffer += sizeof(uint32_t);

	buffer = AMessage::FromBuffer(buffer);

	m_offset = *(uint32_t*)buffer;
	buffer += sizeof(m_offset);

	m_size = *(uint32_t*)buffer;
	buffer+= sizeof(m_size);
	
	m_buffer = new char[m_size];
	
	memcpy(m_buffer, buffer, m_size);
	
	return (buffer + m_size);
}

uint32_t WriteMessage::GetMessageSize()
{
	return (sizeof(uint32_t) + AMessage::GetMessageSize() + sizeof(m_offset) + sizeof(m_size) + m_size);		
}

uint32_t WriteMessage::GetOffset()
{
	return (m_offset);
}

uint32_t WriteMessage::GetSize()
{
	return (m_size);
}

char* WriteMessage::GetBuffer()
{
	return (m_buffer);
}
}