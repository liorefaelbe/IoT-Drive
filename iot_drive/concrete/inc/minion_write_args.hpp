#ifndef __ILRD_MINION_WRITE_ARGS_HPP__
#define __ILRD_MINION_WRITE_ARGS_HPP__

#include "uid.hpp"          // Uid
#include "framework.hpp"    // IKeyArgs

/**
 * @class  MinionWriteArgs
 * @brief  Wtite args for the TPTask
 * 
 * @author Lior Reafael Berkovits
 * @date   26/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class MinionWriteArgs : public IKeyArgs
{
public:
    MinionWriteArgs(Uid m_uid, u_int32_t offset = 0, u_int32_t  = 0, const char* buffer = nullptr);
    ~MinionWriteArgs();

    int GetKey() override;
    Uid GetUid() const;
    u_int32_t GetOffset() const;
    u_int32_t GetSize() const;
    const char* GetBuffer() const;

    void SetUid(Uid uid);
    void SetOffset(u_int32_t offset);
    void SetSize(u_int32_t size);
    void SetBuffer(const char* buffer);

private:
    Uid m_uid;
    u_int32_t m_offset;
    u_int32_t m_size;    
    char* m_buffer;
}; // MinionWriteArgs
} // namespace ilrd_166_7

#endif // __ILRD_MINION_WRITE_ARGS_HPP__