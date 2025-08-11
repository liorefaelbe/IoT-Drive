#define __HANDLETON_IMPLEMENTOR__

#include "handleton.hpp"        // Handleton
#include "factory.hpp"          // Factory
#include "message.hpp"          // AMessage
#include "raid_manager.hpp"     // RaidManager
#include "master_proxy.hpp"     // MasterProxy
#include "response_manager.hpp" // ResponseManager
#include "response_message.hpp" // AResponseMessage
#include "nbd_proxy.hpp"        // NBDProxy

namespace ilrd_166_7
{
    using MasterProxyFactory = Factory<AMessage, AMessage::MSG_TYPE>;
    using ResponseManagerFactory = Factory<AResponseMessage, AResponseMessage::MSG_TYPE>;

    IMPLEMENT_HANDLETON(ResponseManagerFactory);
    IMPLEMENT_HANDLETON(ResponseManager);
    IMPLEMENT_HANDLETON(NBDProxy);
    IMPLEMENT_HANDLETON(RaidManager);
    IMPLEMENT_HANDLETON(MasterProxyFactory);
    IMPLEMENT_HANDLETON(MasterProxy);
}