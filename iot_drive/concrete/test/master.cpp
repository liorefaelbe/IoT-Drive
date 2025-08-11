#include <iostream>             // cout

#include "framework.hpp"        // Framework 
#include "nbd_proxy.hpp"        // NBDProxy
#include "raid_manager.hpp"     // RaidManager
#include "master_commands.hpp"  // ReadCommand, WriteCommand
#include "handleton.hpp"        // Handleton
#include "factory.hpp"          // Factory
#include "response_message.hpp" // AResponseMessage
#include "test_func.hpp"        // CheckValue

using namespace std;
using namespace ilrd_166_7;

/*************************** Forward Declaration ********************************/

static int Test(void);
static int TestBasic(void);

shared_ptr<AResponseMessage> CreateResponseReadMessage()
{
	return (make_shared<ResponseReadMessage>());
}

shared_ptr<AResponseMessage> CreateResponseWriteMessage()
{
	return (make_shared<ResponseWriteMessage>());
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
   
    cout << "\n\033[1m----------- Testing Master -----------\033[0m\n";
    
    status += TestBasic();

    cout << "\n\033[1m------------- Master End -------------\033[0m\n";

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";

    auto nbd_proxy = Handleton<NBDProxy>::GetInstance();

    Framework::proxy_list_t plist;
    Framework::createFunc_list_t clist;
    
    int nbd_fd = nbd_proxy->GetFD();
    
    plist.push_back({{nbd_fd, Reactor::MODE::READ}, nbd_proxy});
    plist.push_back({{nbd_fd, Reactor::MODE::WRITE}, nbd_proxy});
    
    clist.push_back({0, &ReadCommand::CreateReadCommand});
    clist.push_back({1, &WriteCommand::CreateWriteCommand});
    
    auto factory = Handleton<Factory<AResponseMessage, AResponseMessage::MSG_TYPE>>::GetInstance();
        
    factory->Add(AResponseMessage::READ, &CreateResponseReadMessage);
    factory->Add(AResponseMessage::WRITE, &CreateResponseWriteMessage);
    
    Framework framework("/home/lior/git/projects/final_project/framework/plugins", plist, clist);
    framework.Run();

    return (status);
}