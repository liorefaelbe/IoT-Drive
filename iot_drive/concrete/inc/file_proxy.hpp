#ifndef __ILRD_FILE_PROXY_HPP__
#define __ILRD_FILE_PROXY_HPP__

#include <fcntl.h>  // open
#include <unistd.h> // lseek
#include <string>   // std::string

/**
 * @class  FileProxy
 * @brief  Mannges operations on the file
 * 
 * @author Lior Reafael Berkovits
 * @date   26/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class FileProxy
{
public:
	explicit FileProxy(const std::string& disk_name = "disk.txt");
	~FileProxy();

	bool Read(u_int32_t offset, u_int32_t size, char* out_buffer);
	bool Write(u_int32_t offset, u_int32_t size, const char* in_buffer);

private:
	int m_fd;
}; // FileProxy
} // namespace ilrd_166_7

#endif // __ILRD_FILE_PROXY_HPP__