#include <cstring>      			// memcpy
#include <stdexcept>    			// invalid_argument exception

#include "response_message.hpp"  	// AMessage

using namespace std;
namespace ilrd_166_7
{
    
/**************************** Implementations *********************************/
/**************************** AResponseMessage ********************************/

AResponseMessage::AResponseMessage() { /* empty */ }

AResponseMessage::AResponseMessage(MSG_TYPE key, const Uid& uid, uint8_t status) : m_key(key), m_uid(uid), m_status(status) { /* empty */ }

char* AResponseMessage::ToBuffer(char* buffer)
{
	*(MSG_TYPE*)buffer = m_key;
	buffer += sizeof(m_key);

	buffer = m_uid.ToBuffer(buffer);

	*(uint8_t*)buffer = m_status;
	buffer += sizeof(m_status);

	return (buffer);
}

char* AResponseMessage::FromBuffer(char* buffer)
{
	m_key = *(MSG_TYPE*)buffer;
	buffer += sizeof(m_key);

	buffer = m_uid.FromBuffer(buffer);

	m_status = *(uint8_t*)buffer;
	buffer += sizeof(m_status);

	return (buffer);
}

uint32_t AResponseMessage::GetMessageSize() const
{
	return (sizeof(uint32_t) + m_uid.GetSize() + sizeof(uint8_t));
}

AResponseMessage::MSG_TYPE AResponseMessage::GetKey() const
{
	return (m_key);
}

Uid AResponseMessage::GetUID()
{
	return (m_uid);
}

uint8_t AResponseMessage::GetStatus() const
{
	return (m_status);
}

/*************************** Response Read Message ****************************/

ResponseReadMessage::ResponseReadMessage() : m_buffer(nullptr) { /* empty */ }

ResponseReadMessage::ResponseReadMessage(const Uid& uid, uint8_t status, uint32_t size, const char* buffer) 
					: AResponseMessage(MSG_TYPE::READ, uid, status), m_size(size) 
{ 
	m_buffer = new char[size];
	
	memcpy(m_buffer, buffer, size);
}

ResponseReadMessage::~ResponseReadMessage()
{
	delete[] m_buffer;
}

char* ResponseReadMessage::ToBuffer(char* buffer)
{
	*(uint32_t*)buffer = GetMessageSize();
	buffer += sizeof(uint32_t);

	buffer = AResponseMessage::ToBuffer(buffer);

	*(uint32_t*)buffer = m_size;
	buffer += sizeof(m_size);

	memcpy(buffer, m_buffer, m_size);
	
	return (buffer);
}

char* ResponseReadMessage::FromBuffer(char* buffer)
{
	buffer += sizeof(uint32_t);

	buffer = AResponseMessage::FromBuffer(buffer);

	m_size = *(uint32_t*)buffer;
	buffer+= sizeof(m_size);
	
	if (m_buffer != nullptr)
	{
		throw invalid_argument("buffer is already initialized");
	}

	m_buffer = new char[m_size];
	
	memcpy(m_buffer, buffer, m_size);
	
	return (buffer + m_size);
}

uint32_t ResponseReadMessage::GetMessageSize() const
{
	return (sizeof(uint32_t) + AResponseMessage::GetMessageSize() + sizeof(uint8_t) + sizeof(m_size) + m_size);		
}

uint32_t ResponseReadMessage::GetSize() const
{
	return (m_size);
}

char* ResponseReadMessage::GetBuffer() const
{
	return (m_buffer);
}

/************************** Response Write Message ****************************/

ResponseWriteMessage::ResponseWriteMessage() { /* empty */ }

ResponseWriteMessage::ResponseWriteMessage(const Uid& uid, uint8_t status) : AResponseMessage(MSG_TYPE::WRITE, uid, status) { /* empty */ }

char* ResponseWriteMessage::ToBuffer(char* buffer)
{
	*(uint32_t*)buffer = GetMessageSize();
	buffer += sizeof(uint32_t);
		
	buffer = AResponseMessage::ToBuffer(buffer);
	
	return (buffer);
}

char* ResponseWriteMessage::FromBuffer(char* buffer)
{
	buffer += sizeof(uint32_t);

	buffer = AResponseMessage::FromBuffer(buffer);
	
	return (buffer);
}

uint32_t ResponseWriteMessage::GetMessageSize() const
{
	return (sizeof(uint32_t) + AResponseMessage::GetMessageSize() + sizeof(uint8_t));		
}

uint32_t ResponseWriteMessage::GetSize() const
{
	return (m_size);
}
}