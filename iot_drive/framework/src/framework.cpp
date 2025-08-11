#include "thread_pool.hpp"      // ThreadPool
#include "factory.hpp"          // Factory
#include "tp_task.hpp"          // TPTask
#include "handleton.hpp"        // Handleton
#include "input_mediator.hpp"   // InputMediator
#include "framework.hpp"        // Framework

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/****************************** IInputProxy ***********************************/

IInputProxy::~IInputProxy() { /* empty */ }

/******************************* Framework ************************************/

Framework::Framework(const string& pluginsPathName, proxy_list_t pList , createFunc_list_t clist) 
                    : m_reactor(Reactor(&m_listener)), m_monitor(pluginsPathName), 
                    m_loader_cb(m_loader,[](DllLoader& obj, string fileName) { obj.Load(fileName); }, nullptr)
{
	for (auto& proxy : pList)
	{
		m_reactor.Subscribe(proxy.first.first, proxy.first.second, InputMediator(proxy.second, proxy.first));
    }
	for (auto& createFunc : clist)
	{
		Handleton<Factory<ICommand, int>>::GetInstance()->Add(createFunc.first, createFunc.second);
	}

    m_monitor.Subscribe(&m_loader_cb);
};

Framework::~Framework() { /* empty */ }

void Framework::Run()
{
    Handleton<ThreadPool>::GetInstance()->Run();
    m_monitor.Run();
	m_reactor.Run();
}
}