#include <iostream>                 // std::cout
#include <cstring>                  // memcpy

#include "minion_write_args.hpp"    // MinionWriteArgs

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/****************************** Write Args ************************************/

MinionWriteArgs::MinionWriteArgs(Uid uid, u_int32_t offset, u_int32_t size, const char* buffer) : m_uid(uid), m_offset(offset), m_size(size)
{
    m_buffer = new char[size];

	if (buffer == nullptr)
	{
		throw invalid_argument("Received null pointer");
	}

    memcpy(m_buffer, buffer, size);
}
    
MinionWriteArgs::~MinionWriteArgs()
{   
    delete[] m_buffer;
}

int MinionWriteArgs::GetKey()
{ 
    return (static_cast<int>(Reactor::MODE::WRITE)); 
}

Uid MinionWriteArgs::GetUid() const
{
    return (m_uid); 
}

u_int32_t MinionWriteArgs::GetOffset() const
{ 
    return (m_offset); 
}

u_int32_t MinionWriteArgs::GetSize() const
{ 
    return (m_size); 
}

const char* MinionWriteArgs::GetBuffer() const
{ 
    return (m_buffer); 
}

void MinionWriteArgs::SetUid(Uid uid)
{
    m_uid = uid;
}

void MinionWriteArgs::SetOffset(u_int32_t offset)
{
    m_offset = offset;
}

void MinionWriteArgs::SetSize(u_int32_t size)
{
    m_size = size;
}

void MinionWriteArgs::SetBuffer(const char* buffer)
{
    memcpy(m_buffer, buffer, m_size);
}
}