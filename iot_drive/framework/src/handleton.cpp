#define __HANDLETON_IMPLEMENTOR__

#include "handleton.hpp"    // Handleton
#include "factory.hpp"      // Factory
#include "thread_pool.hpp"  // ThreadPool
#include "logger.hpp"       // Logger
#include "scheduler.hpp"    // Schduler
#include "reactor.hpp"      // fd_mode
#include "framework.hpp"    // ICommand

namespace ilrd_166_7
{
    using TestFactory = Factory<int, int, int>;
    using FWFactory = Factory<ICommand, int>;

    IMPLEMENT_HANDLETON(Scheduler);
    IMPLEMENT_HANDLETON(Logger);
    IMPLEMENT_HANDLETON(TestFactory);
    IMPLEMENT_HANDLETON(FWFactory);
    IMPLEMENT_HANDLETON(ThreadPool);
}