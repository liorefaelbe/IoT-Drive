#include "minion_proxy.hpp"    // MinionProxy 
#include "raid_manager.hpp"    // RaidManager

using namespace std;
namespace ilrd_166_7
{

static const size_t MB = 1 << 20;

static const int MINION_NUM = 3;
static const int MINION_SIZE = 8 * MB;
static const int BLOCK_SIZE = 4 * MB;
static const int PORT_0 = 8000;
// static const char* PI_IP = "10.1.0.19";

/**************************** Implementations *********************************/
/****************************** Raid Manager **********************************/

RaidManager::RaidManager()
{
    for (int i = 0; i < MINION_NUM; ++i)
    {
        m_minions.push_back(make_shared<MinionProxy>(PORT_0 + (i * 100)));
    }

    // m_minions.push_back(make_shared<MinionProxy>(8200, PI_IP));
}

RaidManager::Minions RaidManager::GetMinionProxy(size_t offset)
{
    Minions result = {};

    result.m_original = m_minions[(offset / BLOCK_SIZE)];
    result.m_originalOffset = offset % BLOCK_SIZE;

    result.m_backup = m_minions[((offset / BLOCK_SIZE) + 1) % MINION_NUM];
    result.m_backupOffset = (offset % BLOCK_SIZE) + BLOCK_SIZE;

    return result;
}
}