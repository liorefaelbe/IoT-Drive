#include <iostream>     // cout
#include <thread>       // thread
#include <chrono>       // milliseconds
#include <mutex>        // mutex
#include <vector>       // vector
#include <string>       // string
#include <sstream>      // stringstream
#include <algorithm>    // sort

#include "w_q.hpp"          // WaitableQueue
#include "pq_adapter.hpp"   // PQAdapter
#include "test_func.hpp"    // CheckValue

using namespace std;
using namespace ilrd_166_7;

mutex results_mutex;

/************************* Forward Declaration *******************************/

static int Test(void);
static int TestBasic(void);
static int TestTimeout(void);
static int TestMultiThreads(void);
static int TestStringMultiThread(void);
static int TestPQMultiThread(void);
static int TestPQStringMultiThread(void);
static void Producer(WaitableQueue<int>& queue, int start, int count);
static void Consumer(WaitableQueue<int>& queue, vector<int>& results, int count);
static void StringProducer(WaitableQueue<string>& queue, int start, int count);
static void StringConsumer(WaitableQueue<string>& queue, vector<string>& results, int count);
static void PQProducer(WaitableQueue<int, PQAdapter<int>>& queue, int start, int count);
static void PQConsumer(WaitableQueue<int, PQAdapter<int>>& queue, vector<int>& results, int count);
static void PQStringProducer(WaitableQueue<string, PQAdapter<string>>& queue, int start, int count);
static void PQStringConsumer(WaitableQueue<string, PQAdapter<string>>& queue, vector<string>& results, int count);

/******************************* Main ****************************************/

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
   
    cout << "\n\033[1m----------- Testing WaitableQueue -----------\033[0m\n";
    
    status += TestBasic();
    status += TestTimeout();
    status += TestMultiThreads();
    status += TestStringMultiThread();
    status += TestPQMultiThread();
    status += TestPQStringMultiThread();
    
    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";
 
    WaitableQueue<int> queue;

    status += CheckValue(true, queue.IsEmpty(), "IsEmpty on empty queue", __LINE__);
    
    queue.Push(42);
    status += CheckValue(false, queue.IsEmpty(), "IsEmpty after Push", __LINE__);
    
    int value = 0;
    queue.Pop(value);
    status += CheckValue(42, value, "Pop value", __LINE__);
 
    status += CheckValue(true, queue.IsEmpty(), "IsEmpty after Pop", __LINE__);
    
    return (status);
}

static int TestTimeout(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting timeout functionality:\033[0m\n";
    
    WaitableQueue<int> queue;
    int value = 0;
    
    // Test timeout on empty queue
    auto start = chrono::steady_clock::now();
    bool result = queue.Pop(value, chrono::milliseconds(100));
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    status += CheckValue(false, result, "Pop with timeout on empty queue", __LINE__);
    status += CheckValue(true, duration.count() >= 90, "Timeout duration", __LINE__);
    
    // Test successful Pop with timeout
    queue.Push(123);
    result = queue.Pop(value, chrono::milliseconds(100));
    
    status += CheckValue(true, result, "Pop with timeout on non-empty queue", __LINE__);
    status += CheckValue(123, value, "Pop with timeout value", __LINE__);
    
    return (status);
}

static int TestMultiThreads(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting multi-threaded functionality:\033[0m\n";
    
    const int NUM_ITEMS = 100;

    WaitableQueue<int> queue;
    vector<int> results;
    
    // Start consumer before producer to test waiting
    thread consumer(Consumer, ref(queue), ref(results), NUM_ITEMS);
    
    // Short delay to ensure consumer starts first
    this_thread::sleep_for(chrono::milliseconds(50));
    
    thread producer(Producer, ref(queue), 1, NUM_ITEMS);
    
    producer.join();
    consumer.join();
    
    // Verify all items were consumed in the correct order
    bool all_correct = true;
    if (results.size() != NUM_ITEMS)
    {
        all_correct = false;
    }
    else
    {
        for (size_t i = 0; i < results.size(); ++i)
        {
            if (results[i] != static_cast<int>(i + 1))
            {
                all_correct = false;
                break;
            }
        }
    }
    
    status += CheckValue(true, all_correct, "Multi-threaded correctness", __LINE__);
    
    return (status);
}

static int TestStringMultiThread(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting string multi-threaded functionality:\033[0m\n";
    
    const int NUM_ITEMS = 100;

    WaitableQueue<string> queue;
    vector<string> results;
    vector<string> expected;
    
    // Generate expected results
    for (int i = 0; i < NUM_ITEMS; ++i) 
    {
        stringstream ss;
        ss << "String_" << (i + 1);
        expected.push_back(ss.str());
    }
    
    // Create multiple producer and consumer threads
    thread consumer1(StringConsumer, ref(queue), ref(results), NUM_ITEMS/2);
    thread consumer2(StringConsumer, ref(queue), ref(results), NUM_ITEMS/2);
    
    // Short delay to ensure consumers start first
    this_thread::sleep_for(chrono::milliseconds(50));
    
    thread producer1(StringProducer, ref(queue), 1, NUM_ITEMS/2);
    thread producer2(StringProducer, ref(queue), NUM_ITEMS/2 + 1, NUM_ITEMS/2);
    
    producer1.join();
    producer2.join();
    consumer1.join();
    consumer2.join();
    
    // Verify we have correct number of results
    status += CheckValue(expected.size(), results.size(), "String test size", __LINE__);

    sort(results.begin(), results.end());
    sort(expected.begin(), expected.end());
    
    // Check if all elements match
    bool all_match = true;
    for (size_t i = 0; i < NUM_ITEMS; ++i) 
    {
        if (expected[i] != results[i]) 
        {
            all_match = false;
            cout << "Mismatch at position " << i << ":  Expected: '" 
                 << expected[i] << "',  Got:  '" << results[i] << "'" << endl;
            break;
        }
        /* else
        {
            cout << "At position " << i << ":  Expected: '" 
                 << expected[i] << "',  Got:  '" << results[i] << "'" << endl;
        } */
    }
    
    status += CheckValue(true, all_match, "String test - content correctness", __LINE__);
    
    // Test large string (testing copy constructor efficiency)
    WaitableQueue<string> largeQueue;
    string largeString(10000, 'X'); // 10KB string
    string retrievedString;
    
    largeQueue.Push(largeString);
    largeQueue.Pop(retrievedString);
    
    status += CheckValue(largeString.size(), retrievedString.size(), "Large string size", __LINE__);
    status += CheckValue(true, largeString == retrievedString, "Large string content", __LINE__);
    
    return (status);
}

static int TestPQMultiThread(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting PQAdapter multi-threaded functionality:\033[0m\n";
    
    const int NUM_ITEMS = 10;

    WaitableQueue<int, PQAdapter<int>> queue;
    vector<int> results;
    
    // Start consumer before producer to test waiting
    thread consumer1(PQConsumer, ref(queue), ref(results), NUM_ITEMS/2);
    thread consumer2(PQConsumer, ref(queue), ref(results), NUM_ITEMS/2);

    // Short delay to ensure consumer starts first
    this_thread::sleep_for(chrono::milliseconds(50));
    
    thread producer1(PQProducer, ref(queue), 1, NUM_ITEMS/2);
    thread producer2(PQProducer, ref(queue), NUM_ITEMS/2 + 1, NUM_ITEMS);

    producer1.join();
    producer2.join();
    consumer1.join();
    consumer2.join();
    
    // Verify we have correct number of results
    status += CheckValue(NUM_ITEMS == results.size(), true, "Multi-threaded size", __LINE__);

    sort(results.begin(), results.end());

    // Check if all elements match
    bool all_match = true;

    if (results.size() != NUM_ITEMS)
    {
        all_match = false;
    }
    else
    {
        for (size_t i = 0; i < results.size(); ++i)
        {
            if (results[i] != static_cast<int>(i + 1))
            {
                all_match = false;
                cout << "Mismatch at position " << i << ":  Expected: '" 
                 << i + 1 << "',  Got:  '" << results[i] << "'" << endl;
            }
            /* else
            {
                cout << "At position " << i << ":  Expected: '" 
                 << i + 1 << "',  Got:  '" << results[i] << "'" << endl;
            } */
        }
    }
    
    status += CheckValue(true, all_match, "Multi-threaded correctness", __LINE__);
    
    return (status);
}

static int TestPQStringMultiThread(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting PQAdapter multi-threaded functionality:\033[0m\n";
    
    const int NUM_ITEMS = 10;

    WaitableQueue<string, PQAdapter<string>> queue;
    vector<string> results;
    vector<string> expected;
    
    // Generate expected results
    for (int i = 0; i < NUM_ITEMS; ++i) 
    {
        stringstream ss;
        ss << "String_" << (i + 1);
        expected.push_back(ss.str());
    }
    
    // Create multiple producer and consumer threads
    thread consumer1(PQStringConsumer, ref(queue), ref(results), NUM_ITEMS/2);
    thread consumer2(PQStringConsumer, ref(queue), ref(results), NUM_ITEMS/2);
    
    // Short delay to ensure consumers start first
    this_thread::sleep_for(chrono::milliseconds(50));
    
    thread producer1(PQStringProducer, ref(queue), 1, NUM_ITEMS/2);
    thread producer2(PQStringProducer, ref(queue), NUM_ITEMS/2 + 1, NUM_ITEMS/2);
    
    producer1.join();
    producer2.join();
    consumer1.join();
    consumer2.join();
    
    // Verify we have correct number of results
    status += CheckValue(expected.size(), results.size(), "String test size", __LINE__);

    sort(results.begin(), results.end());
    sort(expected.begin(), expected.end());
    
    // Check if all elements match
    bool all_match = true;
    for (size_t i = 0; i < NUM_ITEMS; ++i) 
    {
        if (expected[i] != results[i]) 
        {
            all_match = false;
            cout << "Mismatch at position " << i << ":  Expected: '" 
                 << expected[i] << "',  Got:  '" << results[i] << "'" << endl;
            break;
        }
        /* else
        {
            cout << "At position " << i << ":  Expected: '" 
                 << expected[i] << "',  Got:  '" << results[i] << "'" << endl;
        } */
    }
    
    status += CheckValue(true, all_match, "String test - content correctness", __LINE__);
    
    // Test large string (testing copy constructor efficiency)
    WaitableQueue<string> largeQueue;
    string largeString(10000, 'X'); // 10KB string
    string retrievedString;
    
    largeQueue.Push(largeString);
    largeQueue.Pop(retrievedString);
    
    status += CheckValue(largeString.size(), retrievedString.size(), "Large string size", __LINE__);
    status += CheckValue(true, largeString == retrievedString, "Large string content", __LINE__);
    
    return (status);
}

static void Producer(WaitableQueue<int>& queue, int start, int count)
{

    for (int i = 0; i < count; ++i)
    {
        queue.Push(start + i);
        
        // Add small random delay to test concurrency
        this_thread::sleep_for(chrono::milliseconds(rand() % 5));
    }
}

static void Consumer(WaitableQueue<int>& queue, vector<int>& results, int count)
{
    int value = 0;
    
    for (int i = 0; i < count; ++i)
    {
        queue.Pop(value);
        
        // Thread-safe update of results
        {
            lock_guard<mutex> lock(results_mutex);
            results.push_back(value);
        }
        
        // Add small random delay to test concurrency
        this_thread::sleep_for(chrono::milliseconds(rand() % 5));
    }
}

static void StringProducer(WaitableQueue<string>& queue, int start, int count)
{
    for (int i = 0; i < count; ++i)
    {
        stringstream ss;
        ss << "String_" << (start + i);
        string data = ss.str();
        
        queue.Push(data);
        
        // Add small random delay to test concurrency
        this_thread::sleep_for(chrono::milliseconds(rand() % 5));
    }
}

static void StringConsumer(WaitableQueue<string>& queue, vector<string>& results, int count)
{
    string value;
    
    for (int i = 0; i < count; ++i)
    {
        queue.Pop(value);
        
        // Thread-safe update of results
        {
            lock_guard<mutex> lock(results_mutex);
            results.push_back(value);
        }
        
        // Add small random delay to test concurrency
        this_thread::sleep_for(chrono::milliseconds(rand() % 5));
    }
}

static void PQProducer(WaitableQueue<int, PQAdapter<int>>& queue, int start, int count)
{

    for (int i = 0; i < count; ++i)
    {
        queue.Push(start + i);
        
        // Add small random delay to test concurrency
        this_thread::sleep_for(chrono::milliseconds(rand() % 5));
    }
}

static void PQConsumer(WaitableQueue<int, PQAdapter<int>>& queue, vector<int>& results, int count)
{
    int value = 0;
    
    for (int i = 0; i < count; ++i)
    {
        queue.Pop(value);
        
        // Thread-safe update of results
        {
            lock_guard<mutex> lock(results_mutex);
            results.push_back(value);
        }
        
        // Add small random delay to test concurrency
        this_thread::sleep_for(chrono::milliseconds(rand() % 5));
    }
}

static void PQStringProducer(WaitableQueue<string, PQAdapter<string>>& queue, int start, int count)
{
    for (int i = 0; i < count; ++i)
    {
        stringstream ss;
        ss << "String_" << (start + i);
        string data = ss.str();
        
        queue.Push(data);
        
        // Add small random delay to test concurrency
        this_thread::sleep_for(chrono::milliseconds(rand() % 5));
    }
}

static void PQStringConsumer(WaitableQueue<string, PQAdapter<string>>& queue, vector<string>& results, int count)
{
    string value;
    
    for (int i = 0; i < count; ++i)
    {
        queue.Pop(value);
        
        // Thread-safe update of results
        {
            lock_guard<mutex> lock(results_mutex);
            results.push_back(value);
        }
        
        // Add small random delay to test concurrency
        this_thread::sleep_for(chrono::milliseconds(rand() % 5));
    }
}