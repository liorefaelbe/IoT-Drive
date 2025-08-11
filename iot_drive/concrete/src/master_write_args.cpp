#include <iostream>                 // std::cout
#include <cstring>                  // memcpy

#include "master_write_args.hpp"    // MasterWriteArgs

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/****************************** Write Args ************************************/

MasterWriteArgs::MasterWriteArgs(const char nbdUid[S_UID_SIZE], size_t offset, size_t size, const char* buffer) : m_offset(offset), m_size(size)
{
    m_buffer = new char[size];

	if (buffer == nullptr || nbdUid == nullptr)
	{
		throw invalid_argument("MasterWriteArgs - Received null pointer");
	}

    memcpy(m_buffer, buffer, size);
    memcpy(m_nbdUid, nbdUid, S_UID_SIZE);	
}
    
MasterWriteArgs::~MasterWriteArgs()
{   
    delete[] m_buffer;
}

int MasterWriteArgs::GetKey()
{ 
    return (static_cast<int>(Reactor::MODE::WRITE)); 
}

const char* MasterWriteArgs::GetUid() const
{
    return (m_nbdUid);
}

size_t MasterWriteArgs::GetOffset() const
{ 
    return (m_offset); 
}

size_t MasterWriteArgs::GetSize() const
{ 
    return (m_size); 
}

const char* MasterWriteArgs::GetBuffer() const
{ 
    return (m_buffer); 
}

void MasterWriteArgs::SetOffset(size_t offset)
{
    m_offset = offset;
}

void MasterWriteArgs::SetSize(size_t size)
{
    m_size = size;
}

void MasterWriteArgs::SetBuffer(const char* buffer)
{
    memcpy(m_buffer, buffer, m_size);
}
}