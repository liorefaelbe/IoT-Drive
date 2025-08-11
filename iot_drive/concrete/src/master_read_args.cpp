#include <cstring>              //memcpy

#include "master_read_args.hpp" // MasterReadArgs

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/******************************* Read Args ************************************/

MasterReadArgs::MasterReadArgs(const char nbdUid[S_UID_SIZE], size_t offset, size_t size) : m_offset(offset), m_size(size)
{
	memcpy(m_nbdUid, nbdUid, S_UID_SIZE);	
}

int MasterReadArgs::GetKey()
{ 
    return (static_cast<int>(Reactor::MODE::READ)); 
}

const char* MasterReadArgs::GetUid() const
{
    return (m_nbdUid);
}

size_t MasterReadArgs::GetOffset() const
{ 
    return (m_offset); 
}

size_t MasterReadArgs::GetSize() const
{ 
    return (m_size); 
}

void MasterReadArgs::SetOffset(size_t offset)
{
    m_offset = offset;
}

void MasterReadArgs::SetSize(size_t size)
{
    m_size = size;
}
}