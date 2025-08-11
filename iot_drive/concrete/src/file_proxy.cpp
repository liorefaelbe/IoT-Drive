#include <iostream>         // std::cerr

#include "file_proxy.hpp"   // WriteArgs

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/******************************* FileProxy ************************************/

FileProxy::FileProxy(const string& file_name)
{
    m_fd = open(file_name.c_str() , O_RDWR | O_CREAT, 0666);
    if (m_fd == -1)
    {
        cerr << "FileProxy::Ctor- open fail.\n"; 
    }
}

FileProxy::~FileProxy()
{
	close(m_fd);
}

bool FileProxy::Read(u_int32_t offset, u_int32_t size, char* out_buffer) 
{
	auto status = lseek(m_fd, offset, SEEK_SET);
	if (status == -1)
    {
        cerr << "FileProxy::Read - lseek fail.\n";
        return (false);
    }
	
	u_int32_t b_read = read(m_fd, out_buffer, size);
	if (b_read != size)
    {
        cerr << "FileProxy::Read - read fail.\n";
        return (false);
    }
	
	return (true);
}

bool FileProxy::Write(u_int32_t offset, u_int32_t size, const char* in_buffer) 
{
	auto status = lseek(m_fd, offset, SEEK_SET);
	if (status == -1)
    {
        cerr << "FileProxy::Write - lseek fail.\n";
        return (false);
    }

	u_int32_t b_read = write(m_fd, in_buffer, size);
    if (b_read != size)
    {
        cerr << "FileProxy::Write - write fail.\n";
        return (false);
    }
	
	return (true);
}
}