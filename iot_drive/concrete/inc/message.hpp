#ifndef __ILRD_MESSAGE_HPP__
#define __ILRD_MESSAGE_HPP__

#include <cstdint>  // uint32_t

#include "uid.hpp"  // Uid

/**
 * @class  AMessage
 * @brief  A operation message sent by the naster's MinionProxy to the minion's MasterProxy by UDP
 * 
 * @author Lior Reafael Berkovits
 * @date   03/06/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class AMessage
{
public:
	enum MSG_TYPE : uint32_t {READ, WRITE};

	explicit AMessage() = default;
	explicit AMessage(AMessage::MSG_TYPE key, const Uid& uid);
	virtual ~AMessage() = default;

	virtual char* ToBuffer(char* buffer) = 0;
	virtual char* FromBuffer(char* buffer) = 0;
	virtual uint32_t GetMessageSize() = 0;
	
	virtual uint32_t GetOffset() = 0;
	virtual uint32_t GetSize() = 0;
	MSG_TYPE GetKey();
	Uid GetUID();

private:
	MSG_TYPE m_key;
	Uid m_uid;
}; // AMessage

class ReadMessage : public AMessage
{
public:
	explicit ReadMessage();
	explicit ReadMessage(const Uid& uid, uint32_t offset = 0, uint32_t size = 0);

	char* ToBuffer(char* buffer) override;
	char* FromBuffer(char* buffer) override;
	uint32_t GetMessageSize() override;

	uint32_t GetOffset() override;
	uint32_t GetSize() override;

private:
	uint32_t m_offset;
	uint32_t m_size;
}; // ReadMessage

class WriteMessage : public AMessage
{
public:
	explicit WriteMessage();
	explicit WriteMessage(const Uid& uid, uint32_t offset, uint32_t size, const char* buffer);
	~WriteMessage();
	
	char* ToBuffer(char* buffer) override;
	char* FromBuffer(char* buffer) override;
	uint32_t GetMessageSize() override;

	uint32_t GetOffset() override;
	uint32_t GetSize() override;
	char* GetBuffer();

private:
	uint32_t m_offset;
	uint32_t m_size;
	char* m_buffer;
}; // WriteMessage
} // namespace ilrd_166_7

#endif // __ILRD_MESSAGE_HPP__