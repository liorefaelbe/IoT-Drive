#ifndef __ILRD_UDP_HPP__
#define __ILRD_UDP_HPP__

#include <atomic>       // atomic_uint32_t
#include <time.h>       // time_t
#include <sys/types.h>  // pid_t
#include <functional> 	// hash
#include <iosfwd> 		// ostream, istream

/**
 * @class  Uid
 * @brief  Uid for the AMessage class
 * 
 * @author Lior Reafael Berkovits
 * @date   03/06/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class Uid
{
public:
	explicit Uid();
	~Uid() = default;

	Uid(const Uid& other) = default;
	Uid(Uid&& other) = default;

	Uid& operator=(const Uid& other) = default;
	Uid& operator=(Uid&& other) = default;
	bool operator==(const Uid& other) const;
	bool operator!=(const Uid& other) const;

	char* ToBuffer(char* buffer);
	char* FromBuffer(char* buffer);
	uint32_t GetSize() const;

	pid_t GetPid() const;
	time_t GetTimestamp() const;
	uint32_t GetCounter() const;

private:
	pid_t m_pid;
	time_t m_timestamp;
	uint32_t m_counter;

	static std::atomic_uint32_t s_counter;
};

std::ostream& operator<<(std::ostream& os, const Uid& src);

} // namespace ilrd_166_7

namespace std
{
template<>
struct hash<ilrd_166_7::Uid>
{
	std::size_t operator()(const ilrd_166_7::Uid& uid) const
	{
		std::size_t h1 = std::hash<pid_t>()(uid.GetPid());
		std::size_t h2 = std::hash<time_t>()(uid.GetTimestamp());
		std::size_t h3 = std::hash<uint32_t>()(uid.GetCounter());
		return (h1 ^ (h2 << 1) ^ (h3 << 2));
	}
};
} // namespace std

#endif // __ILRD_UDP_HPP__
