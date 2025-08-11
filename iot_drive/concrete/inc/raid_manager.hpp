#ifndef __ILRD_RAID_MANAGER_HPP__
#define __ILRD_RAID_MANAGER_HPP__

#include <vector>               // std::vector
#include <memory>               // std::shared_ptr

#include "iminion_proxy.hpp"    // IMinionProxy

/**
 * @class  RaidManager
 * @brief  Menge the Minions that represents the disk
 * 
 * @author Lior Reafael Berkovits
 * @date   27/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class RaidManager
{
public:
    explicit RaidManager();

    struct Minions
    {
        std::shared_ptr<IMinionProxy> m_original;
        size_t m_originalOffset;
        std::shared_ptr<IMinionProxy> m_backup;
        size_t m_backupOffset;
    };

    Minions GetMinionProxy(size_t offset);
private:
    std::vector<std::shared_ptr<IMinionProxy>> m_minions;
};
} // namespace ilrd_166_7

#endif // __ILRD_RAID_MANAGER_HPP__