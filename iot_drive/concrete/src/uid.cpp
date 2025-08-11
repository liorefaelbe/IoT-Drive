#include <unistd.h> // getpid

#include "uid.hpp"  // Uid
#include <iostream> // ostream, istream

using namespace std;
namespace ilrd_166_7
{

atomic_uint32_t Uid::s_counter{0};

/**************************** Implementations *********************************/
/****************************** Minion Proxy **********************************/

Uid::Uid() : m_pid(getpid()), m_timestamp(time(NULL)), m_counter(s_counter++) { /* empty */ }

bool Uid::operator == (const Uid& other) const
{
	return ((other.m_pid == m_pid) && (other.m_timestamp == m_timestamp) && (other.m_counter == m_counter));
}

bool Uid::operator != (const Uid& other) const
{
	return ((other.m_pid != m_pid) || (other.m_timestamp != m_timestamp) || (other.m_counter != m_counter));
}

char* Uid::ToBuffer(char* buffer)
{
	*(pid_t*)buffer = m_pid;
	buffer += sizeof(pid_t);

	*(time_t*)buffer = m_timestamp;
	buffer += sizeof(time_t);

	*(uint32_t*)buffer = m_counter;
	buffer += sizeof(uint32_t);
	
	return (buffer);		
}

char* Uid::FromBuffer(char* buffer)
{
	m_pid = *(pid_t*)buffer;
	buffer += sizeof(pid_t);

	m_timestamp = *(time_t*)buffer;
	buffer += sizeof(time_t);

	m_counter = *(uint32_t*)buffer;
	buffer += sizeof(uint32_t);
		
	return (buffer);		
}

uint32_t Uid::GetSize() const
{
	return (sizeof(m_pid) + sizeof(m_timestamp) + sizeof(m_counter));
}

pid_t Uid::GetPid() const
{
	return m_pid;
}

time_t Uid::GetTimestamp() const
{
	return m_timestamp;
}

uint32_t Uid::GetCounter() const
{
	return m_counter;
}

ostream& operator<<(ostream& os, const Uid& src)
{
	os << src.GetPid() << " | " << src.GetTimestamp() << " | " << src.GetCounter();

	return (os);
}
}