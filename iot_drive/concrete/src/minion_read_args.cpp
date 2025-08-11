#include <iostream>             // std::cout

#include "minion_read_args.hpp" // MinionReadArg
#include "raid_manager.hpp"     // GetMinionProxy

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/******************************* Read Args ************************************/

MinionReadArgs::MinionReadArgs(Uid uid, u_int32_t offset, u_int32_t size) : m_uid(uid), m_offset(offset), m_size(size) { /* empty */ }
    
int MinionReadArgs::GetKey()
{ 
    return (static_cast<int>(Reactor::MODE::READ)); 
}

Uid MinionReadArgs::GetUid() const
{
    return (m_uid); 
}

u_int32_t MinionReadArgs::GetOffset() const
{ 
    return (m_offset); 
}

u_int32_t MinionReadArgs::GetSize() const
{ 
    return (m_size); 
}

void MinionReadArgs::SetUid(Uid uid)
{
    m_uid = uid;
}

void MinionReadArgs::SetOffset(u_int32_t offset)
{
    m_offset = offset;
}

void MinionReadArgs::SetSize(u_int32_t size)
{
    m_size = size;
}
}