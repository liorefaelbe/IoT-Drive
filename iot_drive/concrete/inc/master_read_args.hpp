#ifndef __ILRD_MASTER_READ_ARGS_HPP__
#define __ILRD_MASTER_READ_ARGS_HPP__

#include "framework.hpp"    // IKeyArgs
#include "uid.hpp"          // Uid

/**
 * @class  MasterReadArgs
 * @brief  Read args for the TPTask
 * 
 * @author Lior Reafael Berkovits
 * @date   26/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class MasterReadArgs : public IKeyArgs
{
public:
    static const int S_UID_SIZE = 8;

    explicit MasterReadArgs(const char nbdUid[S_UID_SIZE], size_t offset, size_t size);
    ~MasterReadArgs() = default;

    int GetKey() override;
    const char* GetUid() const;
    size_t GetOffset() const;
    size_t GetSize() const;
    
    void SetOffset(size_t offset);
    void SetSize(size_t size);

private:
    size_t m_offset;
    size_t m_size;
    char m_nbdUid[S_UID_SIZE];
}; // MasterReadArgs
} // namespace ilrd_166_7

#endif // __ILRD_MASTER_READ_ARGS_HPP__