#include "ByteBuffer.h"



void suc::THROW_FAILED_MEM_ALLOC(size_t size)
{
	throw suc::SucMemoryError("Unable to allocate " + std::to_string(size) + " bytes of memory.");
}



suc::ByteBuffer::ByteBuffer(buf_size size)
	:
	_size(size),
	_buffer(calloc(size, 1))
{
	if (_buffer == nullptr)
		THROW_FAILED_MEM_ALLOC(size);
}


suc::ByteBuffer::ByteBuffer(buf_size size, void* source)
	:
	_size(size),
	_buffer(malloc(size))
{
	if (_buffer == nullptr)
		THROW_FAILED_MEM_ALLOC(size);

	memcpy(_buffer, source, size);
}


suc::ByteBuffer::ByteBuffer(const ByteBuffer& other)
	:
	ByteBuffer(other._size, other._buffer)
{
}


suc::ByteBuffer& suc::ByteBuffer::operator=(const ByteBuffer& rhs)
{
	free(_buffer);

	_size = rhs._size;
	_buffer = malloc(_size);
	if (_buffer == nullptr)
		THROW_FAILED_MEM_ALLOC(_size);

	memcpy(_buffer, rhs._buffer, _size);

	return *this;
}


suc::ByteBuffer::ByteBuffer(ByteBuffer&& other) noexcept
	:
	_size(other._size),
	_buffer(other._buffer)
{
	// Invalidate other
	other._size = 0U;
	other._buffer = nullptr;
}


suc::ByteBuffer& suc::ByteBuffer::operator=(ByteBuffer&& rhs) noexcept
{
	free(_buffer);

	_size = rhs._size;
	_buffer = rhs._buffer;

	// Invalidate other
	rhs._size = 0U;
	rhs._buffer = nullptr;

	return *this;
}


suc::ByteBuffer::~ByteBuffer() noexcept
{
	if (_buffer != nullptr) {
		free(_buffer);
	}
}


suc::ByteBuffer& suc::ByteBuffer::cpy(void* source, buf_offset offset, buf_size size)
{
	memcpy(static_cast<ubyte*>(_buffer) + offset, source, size);
	return *this;
}


suc::ByteBuffer suc::ByteBuffer::makeCopy() const
{
	return ByteBuffer(*this);
}


void* suc::ByteBuffer::makeRawCopy() const
{
	void* buf = malloc(_size);
	if (buf == nullptr) {
		THROW_FAILED_MEM_ALLOC(_size);
	}
	memcpy(buf, _buffer, _size);
	return buf;
}


suc::ByteBuffer suc::ByteBuffer::makeCopyRange(buf_offset offset, buf_size size) const
{
	if (offset > _size || offset + size > _size) {
		throw SucInvalidValueException("Specified offset and size exceed buffer capacity.");
	}

	void* newBuffer = malloc(size);
	if (newBuffer == nullptr) {
		THROW_FAILED_MEM_ALLOC(size);
	}
	memcpy(newBuffer, static_cast<ubyte*>(_buffer) + offset, size);

	ByteBuffer resultBuf{ size, newBuffer };
	free(newBuffer);

	return resultBuf;
}


void* suc::ByteBuffer::makeRawCopyRange(buf_offset offset, buf_size size) const
{
	if (offset > _size || offset + size > _size) {
		throw SucInvalidValueException("Specified offset and size exceed buffer capacity.");
	}

	void* buf = malloc(size);
	if (buf == nullptr) {
		THROW_FAILED_MEM_ALLOC(size);
	}
	memcpy(buf, static_cast<ubyte*>(_buffer) + offset, size);
	return buf;
}


void suc::ByteBuffer::resize(buf_size size)
{
	void* newPtr = realloc(_buffer, size);
	if (newPtr == nullptr) {
		THROW_FAILED_MEM_ALLOC(size);
	}
	_buffer = newPtr;
}
