#pragma once
#ifndef SUCBYTEBUFFER_H
#define SUCBYTEBUFFER_H

#include <type_traits>

#include "SocketUtility.h"

namespace suc
{
	template<typename T>
	using IsPtr = std::enable_if_t<std::is_pointer_v<T>>;

	using buf_size = size_t;
	using buf_offset = size_t;

	/* +++ ByteBuffer +++
	An abstraction for a piece of memory.
	Access the memory through casts to any type. Implicit casts are not allowed, except
	the implicit cast to (void*). */
	class ByteBuffer
	{
	public:
		/* +++ Overloaded constructor +++
		Creates a zero-initializes buffer.
		- ARG size: The buffer's storage size in bytes. */
		explicit ByteBuffer(buf_size size = 0U);

		/* +++ Overloaded constructor +++
		Creates a buffer from a source buffer.
		- ARG size: The buffer's storage size in bytes. Must match the size of buffer.
		- ARG buffer: The contents of this buffer will be copied into the new ByteBuffer.
		The memory on this buffer will not be freed. */
		ByteBuffer(buf_size size, void* source);

		/* +++ Copy constructor +++
		Copies the exact contents of the other's buffer.
		Does not modify the other buffer. */
		ByteBuffer(const ByteBuffer& other);
		/* +++ Copy assignment +++
		Copies the exact contents of the other's buffer.
		Does not modify the other buffer. */
		ByteBuffer& operator=(const ByteBuffer& rhs);
		/* +++ Move constructor +++
		Copies the exact contents of the other's buffer.
		Invalidates the other buffer. */
		ByteBuffer(ByteBuffer&& other) noexcept;
		/* +++ Move assignment +++
		Copies the exact contents of the other's buffer.
		Invalidates the other buffer. */
		ByteBuffer& operator=(ByteBuffer&& rhs) noexcept;

		/* +++ Destructor +++
		Frees allocated memory. */
		~ByteBuffer() noexcept;

		/* explicit cast operator */
		template<typename T, IsPtr<T>...>
		explicit inline constexpr operator T() const {
			return static_cast<T> (_buffer);
		}

		/* implicit cast to void* */
		inline constexpr operator void* () const {
			return static_cast<void*> (_buffer);
		}

		/* +++ Explicit cast operator +++
		Casts the buffer's contents to any type.
		RETURN: Returns a pointer to the raw data. */
		template<typename T, IsPtr<T>...>
		[[nodiscard]] inline constexpr T to() const {
			return static_cast<T> (_buffer);
		}

		/* size */
		[[nodiscard]] inline buf_size size() const {
			return _size;
		}

		/* +++ cpy() +++
		Performs a memcpy from a buffer to the ByteBuffer.
		- ARG source: The source buffer
		- ARG offset: The offset into the ByteBuffer.
		- ARG size: The size of the copied memory in bytes.
		- RETURN: Returns this buffer. */
		ByteBuffer& cpy(void* source, buf_offset offset, buf_size size);

		/* create a copy */
		[[nodiscard]] ByteBuffer makeCopy() const;

		/* create a copy and return raw data pointer */
		[[nodiscard]] void* makeRawCopy() const;

		/* create a copy from a range */
		[[nodiscard]] ByteBuffer makeCopyRange(buf_offset offset, buf_size size) const;

		/* create a copy from a range and return raw data pointer */
		[[nodiscard]] void* makeRawCopyRange(buf_offset offset, buf_size size) const;

		/* +++ resize() +++
		Resizes the buffer. All contents will be discarded. */
		void resize(buf_size size);

	private:
		buf_size _size{ 0U };
		void* _buffer{ nullptr };
	};



	inline bool operator<(const ByteBuffer& lhs, const ByteBuffer& rhs) {
		return lhs.size() < rhs.size();
	}
	inline bool operator>(const ByteBuffer& lhs, const ByteBuffer& rhs) {
		return lhs.size() > rhs.size();
	}
	inline bool operator<=(const ByteBuffer& lhs, const ByteBuffer& rhs) {
		return lhs.size() <= rhs.size();
	}
	inline bool operator>=(const ByteBuffer& lhs, const ByteBuffer& rhs) {
		return lhs.size() >= rhs.size();
	}
	inline bool operator==(const ByteBuffer& lhs, const ByteBuffer& rhs) {
		return lhs.size() == rhs.size();
	}



	void THROW_FAILED_MEM_ALLOC(size_t size);
} // namespace suc



#endif