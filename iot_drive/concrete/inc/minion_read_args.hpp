#ifndef __ILRD_MINION_READ_ARGS_HPP__
#define __ILRD_MINION_READ_ARGS_HPP__

#include "uid.hpp"          // Uid
#include "framework.hpp"    // IKeyArgs

/**
 * @class  MinionReadArgs
 * @brief  Read args for the TPTask
 * 
 * @author Lior Reafael Berkovits
 * @date   26/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class MinionReadArgs : public IKeyArgs
{
public:
    explicit MinionReadArgs(Uid uid, u_int32_t offset = 0, u_int32_t size = 0);
    
    int GetKey() override;
    Uid GetUid() const;
    u_int32_t GetOffset() const;
    u_int32_t GetSize() const;
    
    void SetUid(Uid uid);
    void SetOffset(u_int32_t offset);
    void SetSize(u_int32_t size);

private:
    Uid m_uid;
    u_int32_t m_offset;
    u_int32_t m_size;
}; // MinionReadArgs
} // namespace ilrd_166_7

#endif // __ILRD_MINION_READ_ARGS_HPP__