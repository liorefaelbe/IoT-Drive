#include <iostream>             // cout

#include "framework.hpp"        // Framework 
#include "master_proxy.hpp"     // MasterProxy
#include "handleton.hpp"        // Handleton
#include "factory.hpp"          // Factory
#include "minion_commands.hpp"  // FileWriteCommand
#include "message.hpp"          // AMessage
#include "test_func.hpp"        // CheckValue

using namespace std;
using namespace ilrd_166_7;

/*************************** Forward Declaration ********************************/

static int Test(void);
static int TestBasic(void);

shared_ptr<AMessage> CreateReadMessage()
{
	return (make_shared<ReadMessage>());
}

shared_ptr<AMessage> CreateWriteMessage()
{
	return (make_shared<WriteMessage>());
}

/********************************** Main ****************************************/

int main()
{
    int status = 0;
    
    status += Test();
 
    (status == 0) ? (cout << "\n\033[0;32m\033[1mAll Good!\033[0m\n\n") : 
                    (cout << "\n\033[0;31m\033[1m" << status << " FAILS! \033[0mTotaL\n\n");
    
    return (0);
}

/***************************** Static Functions *********************************/

static int Test(void)
{
    int status = 0;
   
    cout << "\n\033[1m----------- Testing Minion -----------\033[0m\n";
    
    status += TestBasic();

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";
    
    auto master_proxy = Handleton<MasterProxy>::GetInstance();
    Framework::proxy_list_t plist;
    Framework::createFunc_list_t clist;

    auto factory = Handleton<Factory<AMessage, AMessage::MSG_TYPE>>::GetInstance();
    
    factory->Add(AMessage::READ, &CreateReadMessage);
    factory->Add(AMessage::WRITE, &CreateWriteMessage);

    u_int16_t udp = master_proxy->GetUDP();

    Reactor::fd_mode udp_pair = {udp, Reactor::MODE::READ};
    plist.push_back({udp_pair, master_proxy});
    clist.push_back({0, &FileReadCommand::CreateFileReadCommand});
    clist.push_back({1, &FileWriteCommand::CreateFileWriteCommand});

    Framework framework("./plugins", plist, clist);
    framework.Run();
    
    return (status);
}