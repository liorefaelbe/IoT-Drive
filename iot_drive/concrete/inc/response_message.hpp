#ifndef __ILRD_RESPONSE_MESSAGE_HPP__
#define __ILRD_RESPONSE_MESSAGE_HPP__

#include <cstdint>  // uint32_t

#include "uid.hpp"  // Uid

/**
 * @class  AResponseMessage
 * @brief  A response message to send to the master ResponseMannger by the Minion by UDP
 * 
 * @author Lior Reafael Berkovits
 * @date   03/06/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class AResponseMessage
{
public:
	enum MSG_TYPE : uint32_t {READ, WRITE};

	explicit AResponseMessage();
	explicit AResponseMessage(MSG_TYPE key, const Uid& uid, uint8_t status);
	virtual ~AResponseMessage() = default;

	virtual char* ToBuffer(char* buffer) = 0;
	virtual char* FromBuffer(char* buffer) = 0;

	virtual uint32_t GetMessageSize() const = 0;
	virtual uint32_t GetSize() const = 0;
	MSG_TYPE GetKey() const;
	Uid GetUID();
	uint8_t GetStatus() const;

private:
	MSG_TYPE m_key;
	Uid m_uid;
	uint8_t m_status;
}; // AMessage

class ResponseReadMessage : public AResponseMessage
{
public:
	explicit ResponseReadMessage();
	explicit ResponseReadMessage(const Uid& uid, uint8_t status, uint32_t size, const char* buffer);
	~ResponseReadMessage();

	char* ToBuffer(char* buffer) override;
	char* FromBuffer(char* buffer) override;

	uint32_t GetMessageSize() const override;
	uint32_t GetSize() const override;
	char* GetBuffer() const;

private:
	uint32_t m_size;
	char* m_buffer;
}; // ResponseReadMessage

class ResponseWriteMessage : public AResponseMessage
{
public:
	explicit ResponseWriteMessage();
	explicit ResponseWriteMessage(const Uid& uid, uint8_t status);

	char* ToBuffer(char* buffer) override;
	char* FromBuffer(char* buffer) override;

	uint32_t GetMessageSize() const override;
	uint32_t GetSize() const override;

private:
	uint32_t m_size;
}; // ResponseWriteMessage
} // namespace ilrd_166_7

#endif // __ILRD_RESPONSE_MESSAGE_HPP__