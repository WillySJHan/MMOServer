#pragma once
#include <memory>
#include <vector>



class SendBuffer : public std::enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(__int32 bufferSize);
	~SendBuffer();

	unsigned char* Buffer() { return _buffer.data(); }
	unsigned __int32 WriteSize() { return _writeSize; }
	unsigned __int32 Capacity() { return static_cast<__int32>(_buffer.size()); }

	void CopyData(void* data, __int32 len);

	/////////////////////////////
	void SetWriteSize(unsigned __int32 size) { _writeSize = size; }
	unsigned __int32 GetBufferSize() { return _bufferSize; }

private:
	std::vector<unsigned char>	_buffer;
	unsigned __int32			_writeSize = 0;
	unsigned __int32			_bufferSize;
};
