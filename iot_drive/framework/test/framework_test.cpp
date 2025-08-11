#include <iostream>         // cout
#include <unistd.h>         // read

#include "framework.hpp"    // Framework 
#include "thread_pool.hpp"  // ThreadPool
#include "handleton.hpp"    // Handleton
#include "factory.hpp"      // Factory
#include "test_func.hpp"    // CheckValue

using namespace std;
using namespace ilrd_166_7;    // clist.push_back({1, &CreateWriteCommand});

/*************************** Forward Declaration ********************************/

static int Test(void);
static int TestBasic(void);

/********************************* Classes **************************************/

class ReadArgs : public IKeyArgs
{
public:
    ReadArgs(int fd, size_t offset, size_t len, char* buffer) 
        : m_fd(fd), m_offset(offset), m_len(len), m_buffer(buffer) {}
    
    int GetKey() override { return (static_cast<int>(Reactor::MODE::READ)); }
    int GetFd() const { return (m_fd); }
    size_t GetOffset() const { return (m_offset); }
    size_t GetLength() const { return (m_len); }
    char* GetBuffer() const { return (m_buffer); }

private:
    int m_fd;
    size_t m_offset;
    size_t m_len;
    char* m_buffer;
};

class WriteArgs : public IKeyArgs
{
public:
    WriteArgs(int fd, size_t offset, size_t len, char* buffer) 
        : m_fd(fd), m_offset(offset), m_len(len), m_buffer(buffer) {}
    
    int GetKey() override { return (static_cast<int>(Reactor::MODE::WRITE)); }
    int GetFd() const { return (m_fd); }
    size_t GetOffset() const { return (m_offset); }
    size_t GetLength() const { return (m_len); }
    char* GetBuffer() const { return (m_buffer); }

private:
    int m_fd;
    size_t m_offset;
    size_t m_len;    
    char* m_buffer;
};

class TestInputProxy : public IInputProxy
{
public:
    std::shared_ptr<IKeyArgs> GetKeyArgs(Reactor::fd_mode key) override
    {
        if (key.second == Reactor::MODE::READ)
        {
            char* buffer = new char[128];
            read(0, buffer, 128);
            buffer[128] = '\0';
            return (make_shared<ReadArgs>(0, 0, 128, buffer));
        }
        else
        {
            char* buffer = new char[128];
            snprintf(buffer, 128, "Test data from TestInputProxy");
            return (make_shared<WriteArgs>(1, 0, 128, buffer));
        }
    }
};

class ReadCommand : public ICommand
{
public:
    async_injection_functor Run(std::shared_ptr<IKeyArgs> args) override
    {
        ReadArgs* read_args = static_cast<ReadArgs*>(args.get());
        cout << "ReadCommand executed with fd: " << read_args->GetFd() 
             << ", offset: " << read_args->GetOffset() 
             << ", length: " << read_args->GetLength()
             << ", buffer: " << read_args->GetBuffer() << endl;

        return ([]() -> bool
        { 
            cout << "Async injection: READ" << endl; 
            return (false);
        });
    }
};

class WriteCommand : public ICommand
{
public:
    async_injection_functor Run(std::shared_ptr<IKeyArgs> args) override
    {
        WriteArgs* write_args = static_cast<WriteArgs*>(args.get());   

        cout << "WriteCommand executed with fd: " << write_args->GetFd() 
             << ", offset: " << write_args->GetOffset() 
             << ", length: " << write_args->GetLength() 
             << ", buffer: " << write_args->GetBuffer() << endl;

        return ([]() -> bool
        { 
            cout << "Async injection: WRITE" << endl; 
            return (false);
        });
    }
};

shared_ptr<ICommand> CreateReadCommand()
{
	return (make_shared<ReadCommand>());
}


shared_ptr<ICommand> CreateWriteCommand()
{
	return (make_shared<WriteCommand>());
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
   
    cout << "\n\033[1m----------- Testing Framework -----------\033[0m\n";
    
    status += TestBasic();

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";

    TestInputProxy input_proxy;
    Framework::proxy_list_t plist;
    Framework::createFunc_list_t clist;

    Reactor::fd_mode read = {0, Reactor::MODE::READ};
    plist.push_back({read, &input_proxy});
    clist.push_back({0, &CreateReadCommand});
    
    // fd_mode write = {1, MODE::WRITE};
    // plist.push_back({write, &input_proxy});
    // clist.push_back({1, &CreateWriteCommand});

    Framework framework("./plugins", plist, clist);
    framework.Run();

    status += CheckValue(true, true, "Basic Framework Test", __LINE__);
    
    return (status);
}