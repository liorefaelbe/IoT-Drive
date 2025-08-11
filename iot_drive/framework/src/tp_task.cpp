#include <chrono>               // std::chrono::milliseconds

#include "handleton.hpp"        // GetInstance
#include "factory.hpp"          // Factory
#include "asyncInjection.hpp"   // AsyncInjection
#include "tp_task.hpp"          // InputMediator

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/******************************** TPTask **************************************/

TPTask::TPTask(std::shared_ptr<IKeyArgs> args) : m_keyArgs(args) { /* empty */}

TPTask::~TPTask() { /* empty */}

void TPTask::Run()
{
    auto p_command = Handleton<Factory<ICommand, int>>::GetInstance()->Create(m_keyArgs->GetKey());
    
    async_injection_functor aif = p_command->Run(m_keyArgs);

    if (aif)
    {
        new AsyncInjection(aif, std::chrono::milliseconds(100));
    }
}
}