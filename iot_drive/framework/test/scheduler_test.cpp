#include <iostream>             // cout, endl
#include <mutex>                // mutex
#include <condition_variable>   // condition_variable
#include <thread>               // this_thread::sleep_for
#include <atomic>               // atomic
#include <vector>               // vector

#include "scheduler.hpp"        // Scheduler
#include "handleton.hpp"        // Handlton
#include "test_func.hpp"        // CheckValue
#include "colors.hpp"           // colors

using namespace std;
using namespace ilrd_166_7;

/************************* Forward Declaration *******************************/

static int Test(void);
static int TestBasic(void);

/***************************** Test Classes **********************************/

class My_Print_Task : public Scheduler::ISchedulerTask
{
public:
   void Execute() override 
   {
   		cout << "hello world" << endl;
   }
};

class CompareStartandEnd : public Scheduler::ISchedulerTask
{
public:
	CompareStartandEnd(chrono::milliseconds expected_dur) : 
	m_create_time(chrono::steady_clock::now()), m_expected_dur(expected_dur) {/*ctor*/}
	
	void Execute() override
	{
		auto execution_time = chrono::steady_clock::now();
		auto actual_duration = execution_time - m_create_time;
		auto duration_diff = chrono::duration_cast<chrono::milliseconds>(actual_duration) - m_expected_dur;
		
        if (abs(duration_diff.count()) > FAULT_TOLERANCE.count())
        {
            cout << RED << "FAIL: Timer execution off by " << duration_diff.count()
                      << "ms (expected: " << m_expected_dur.count()
                      << "ms, actual: " << chrono::duration_cast<chrono::milliseconds>(actual_duration).count() 
                      << "ms)" << RESET << endl;
        }
        else
        {
            cout << GREEN << "SUCCESS: Timer executed within tolerance (diff: " 
                      << duration_diff.count() << "ms)" << RESET << endl;
        }

	}
private:
    chrono::time_point<chrono::steady_clock> m_create_time;
	chrono::milliseconds m_expected_dur;
    static const chrono::milliseconds FAULT_TOLERANCE;

};

const chrono::milliseconds CompareStartandEnd::FAULT_TOLERANCE(100);

/********************************* Main **************************************/

int main()
{
    int status = 0;
    
    status += Test();
 
    (status == 0) ? (cout << "\n\033[0;32m\033[1mAll Good!\033[0m\n\n") : 
                    (cout << "\n\033[0;31m\033[1m" << status << " FAILS! \033[0mTotaL\n\n");
    
    return (0);
}

/***************************** Static Functions ******************************/

static int Test(void)
{
    int status = 0;
   
    cout << "\n\033[1m----------- Testing Scheduler -----------\033[0m\n";
    
    status += TestBasic();

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";
    
    Scheduler* sched = Handleton<Scheduler>::GetInstance();
	auto task = make_shared<My_Print_Task>();
	
	sched->AddTask(task, chrono::milliseconds(1000));
	sched->AddTask(task, chrono::milliseconds(2000));
	sched->AddTask(task, chrono::milliseconds(3000));
	sched->AddTask(task, chrono::milliseconds(4000));
	sched->AddTask(task, chrono::milliseconds(5000));
	
    auto task2 = make_shared<CompareStartandEnd>(chrono::milliseconds(10));
		sched->AddTask(task2, chrono::milliseconds(10));
	
	auto task3 = make_shared<CompareStartandEnd>(chrono::milliseconds(1000));
		sched->AddTask(task3, chrono::milliseconds(1000));
	
	auto task4 = make_shared<CompareStartandEnd>(chrono::milliseconds(10000));
		sched->AddTask(task4, chrono::milliseconds(10000));
	
	this_thread::sleep_for(chrono::seconds(20));

    return (status);
}