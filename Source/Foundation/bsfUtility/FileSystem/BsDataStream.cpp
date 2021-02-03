//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "FileSystem/BsDataStream.h"
#include "Debug/BsDebug.h"
#include "String/BsUnicode.h"
#include "Math/BsMath.h"

namespace bs
{
	const UINT32 DataStream::StreamTempSize = 128;

	/** Checks does the provided buffer has an UTF32 byte order mark in little endian order. */
	bool isUTF32LE(const UINT8* buffer)
	{
		return buffer[0] == 0xFF && buffer[1] == 0xFE && buffer[2] == 0x00 && buffer[3] == 0x00;
	}

	/** Checks does the provided buffer has an UTF32 byte order mark in big endian order. */
	bool isUTF32BE(const UINT8* buffer)
	{
		return buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0xFE && buffer[3] == 0xFF;
	}

	/** Checks does the provided buffer has an UTF16 byte order mark in little endian order. */
	bool isUTF16LE(const UINT8* buffer)
	{
		return buffer[0] == 0xFF && buffer[1] == 0xFE;
	}

	/**	Checks does the provided buffer has an UTF16 byte order mark in big endian order. */
	bool isUTF16BE(const UINT8* buffer)
	{
		return buffer[0] == 0xFE && buffer[1] == 0xFF;
	}

	/**	Checks does the provided buffer has an UTF8 byte order mark. */
	bool isUTF8(const UINT8* buffer)
	{
		return (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF);
	}

	template <typename T> DataStream& DataStream::operator>> (T& val)
	{
		read(static_cast<void*>(&val), sizeof(T));

		return *this;
	}

	void DataStream::writeString(const String& string, StringEncoding encoding)
	{
		if (encoding == StringEncoding::UTF16)
		{
			// Write BOM
			UINT8 bom[2] = { 0xFF, 0xFE };
			write(bom, sizeof(bom));

			U16String u16string = UTF8::toUTF16(string);
			write(u16string.data(), u16string.length() * sizeof(char16_t));
		}
		else
		{
			// Write BOM
			UINT8 bom[3] = { 0xEF, 0xBB, 0xBF };
			write(bom, sizeof(bom));

			write(string.data(), string.length());
		}
	}

	void DataStream::writeString(const WString& string, StringEncoding encoding)
	{
		if (encoding == StringEncoding::UTF16)
		{
			// Write BOM
			UINT8 bom[2] = { 0xFF, 0xFE };
			write(bom, sizeof(bom));

			String u8string = UTF8::fromWide(string);
			U16String u16string = UTF8::toUTF16(u8string);
			write(u16string.data(), u16string.length() * sizeof(char16_t));
		}
		else
		{
			// Write BOM
			UINT8 bom[3] = { 0xEF, 0xBB, 0xBF };
			write(bom, sizeof(bom));

			String u8string = UTF8::fromWide(string);
			write(u8string.data(), u8string.length());
		}
	}

	String DataStream::getAsString()
	{
		// Ensure read from begin of stream
		seek(0);

		// Try reading header
		UINT8 headerBytes[4];
		size_t numHeaderBytes = read(headerBytes, 4);

		size_t dataOffset = 0;
		if(numHeaderBytes >= 4)
		{
			if (isUTF32LE(headerBytes))
				dataOffset = 4;
			else if (isUTF32BE(headerBytes))
			{
				BS_LOG(Warning, Generic, "UTF-32 big endian decoding not supported");
				return u8"";
			}
		}

		if(dataOffset == 0 && numHeaderBytes >= 3)
		{
			if (isUTF8(headerBytes))
				dataOffset = 3;
		}

		if(dataOffset == 0 && numHeaderBytes >= 2)
		{
			if (isUTF16LE(headerBytes))
				dataOffset = 2;
			else if (isUTF16BE(headerBytes))
			{
				BS_LOG(Warning, Generic, "UTF-16 big endian decoding not supported");
				return u8"";
			}
		}

		seek(dataOffset);

		// Read the entire buffer - ideally in one read, but if the size of the buffer is unknown, do multiple fixed size
		// reads.
		size_t bufSize = (mSize > 0 ? mSize : 4096);
		auto tempBuffer = bs_stack_alloc<std::stringstream::char_type>((UINT32)bufSize);

		std::stringstream result;
		while (!eof())
		{
			size_t numReadBytes = read(tempBuffer, bufSize);
			result.write(tempBuffer, numReadBytes);
		}

		bs_stack_free(tempBuffer);

		std::string string = result.str();

		switch(dataOffset)
		{
		default:
		case 0: // No BOM = assumed UTF-8
		case 3: // UTF-8
			return String(string.data(), string.length());
		case 2: // UTF-16
			{
			UINT32 numElems = (UINT32)string.length() / 2;

			return UTF8::fromUTF16(U16String((char16_t*)string.data(), numElems));
			}
		case 4: // UTF-32
			{
			UINT32 numElems = (UINT32)string.length() / 4;

			return UTF8::fromUTF32(U32String((char32_t*)string.data(), numElems));
			}
		}

		// Note: Never assuming ANSI as there is no ideal way to check for it. If required I need to
		// try reading the data and if all UTF encodings fail, assume it's ANSI. For now it should be
		// fine as most files are UTF-8 encoded.
	}

	WString DataStream::getAsWString()
	{
		String u8string = getAsString();

		return UTF8::toWide(u8string);
	}

	MemoryDataStream::MemoryDataStream()
		: DataStream(READ | WRITE)
	{
		
	}

	MemoryDataStream::MemoryDataStream(size_t capacity)
		: DataStream(READ | WRITE)
	{
		realloc(capacity);
		mCursor = mData;
		mEnd = mCursor + capacity;
	}

	MemoryDataStream::MemoryDataStream(void* memory, size_t size)
		: DataStream(READ | WRITE), mOwnsMemory(false)
	{
		mData = mCursor = static_cast<uint8_t*>(memory);
		mSize = size;
		mEnd = mData + mSize;
	}

	MemoryDataStream::MemoryDataStream(const MemoryDataStream& sourceStream)
		: DataStream(READ | WRITE)
	{
		// Copy data from incoming stream
		mSize = sourceStream.size();

		mData = mCursor = static_cast<uint8_t*>(bs_alloc(mSize));
		mEnd = mData + sourceStream.read(mData, mSize);

		assert(mEnd >= mCursor);
	}

	MemoryDataStream::MemoryDataStream(const SPtr<DataStream>& sourceStream)
		: DataStream(READ | WRITE)
	{
		// Copy data from incoming stream
		mSize = sourceStream->size();

		mData = mCursor = static_cast<uint8_t*>(bs_alloc(mSize));
		mEnd = mData + sourceStream->read(mData, mSize);

		assert(mEnd >= mCursor);
	}

	MemoryDataStream::MemoryDataStream(MemoryDataStream&& other)
	{
		*this = std::move(other);
	}

	MemoryDataStream::~MemoryDataStream()
	{
		close();
	}

	MemoryDataStream& MemoryDataStream::operator= (const MemoryDataStream& other)
	{
		if (this == &other)
			return *this;

		this->mName = other.mName;
		this->mAccess = other.mAccess;
		
		if (!other.mOwnsMemory)
		{
			this->mSize = other.mSize;
			this->mData = other.mData;
			this->mCursor = other.mCursor;
			this->mEnd = other.mEnd;
			this->mOwnsMemory = false;
		}
		else
		{
			if (mData && mOwnsMemory)
				bs_free(mData);

			mSize = 0;
			mData = nullptr;
			mCursor = nullptr;
			mEnd = nullptr;

			this->mOwnsMemory = true;

			realloc(other.mSize);
			mEnd = mData + mSize;
			mCursor = mData + (other.mCursor - other.mData);

			if (mSize > 0)
				memcpy(mData, other.mData, mSize);
		}

		return *this;
	}

	MemoryDataStream& MemoryDataStream::operator= (MemoryDataStream&& other)
	{
		if (this == &other)
			return *this;

		if (mData && mOwnsMemory)
			bs_free(mData);

		this->mName = std::move(other.mName);
		this->mAccess = std::exchange(other.mAccess, 0);
		this->mCursor = std::exchange(other.mCursor, nullptr);
		this->mEnd = std::exchange(other.mEnd, nullptr);
		this->mData = std::exchange(other.mData, nullptr);
		this->mSize = std::exchange(other.mSize, 0);
		this->mOwnsMemory = std::exchange(other.mOwnsMemory, false);

		return *this;
	}

	size_t MemoryDataStream::read(void* buf, size_t count) const
	{
		size_t cnt = count;

		if (mCursor + cnt > mEnd)
			cnt = mEnd - mCursor;
		
		if (cnt == 0)
			return 0;

		assert (cnt <= count);

		memcpy(buf, mCursor, cnt);
		mCursor += cnt;

		return cnt;
	}

	size_t MemoryDataStream::write(const void* buf, size_t count)
	{
		size_t written = 0;
		if (isWriteable())
		{
			written = count;

			size_t numUsedBytes = (mCursor - mData);
			size_t newSize = numUsedBytes + count;
			if(newSize > mSize)
			{
				if (mOwnsMemory)
					realloc(newSize);
				else
					written = mSize - numUsedBytes;
			}

			if (written == 0)
				return 0;

			memcpy(mCursor, buf, written);
			mCursor += written;

			mEnd = std::max(mCursor, mEnd);
		}

		return written;
	}

	size_t DataStream::readBits(uint8_t* data, uint32_t count)
	{
		uint32_t numBytes = Math::divideAndRoundUp(count, 8U);
		return read(data, numBytes) * 8;
	}

	size_t DataStream::writeBits(const uint8_t* data, uint32_t count)
	{
		uint32_t numBytes = Math::divideAndRoundUp(count, 8U);
		return write(data, numBytes) * 8;
	}

	void MemoryDataStream::skip(size_t count)
	{
		assert((mCursor + count) <= mEnd);
		mCursor = std::min(mCursor + count, mEnd);
	}

	void MemoryDataStream::seek(size_t pos)
	{
		assert((mData + pos) <= mEnd);
		mCursor = std::min(mData + pos, mEnd);
	}

	void DataStream::align(uint32_t count)
	{
		if (count <= 1)
			return;

		UINT32 alignOffset = (count - (tell() & (count - 1))) & (count - 1);
		skip(alignOffset);
	}

	size_t MemoryDataStream::tell() const
	{
		return mCursor - mData;
	}

	bool MemoryDataStream::eof() const
	{
		return mCursor >= mEnd;
	}

	SPtr<DataStream> MemoryDataStream::clone(bool copyData) const
	{
		if (!copyData)
			return bs_shared_ptr_new<MemoryDataStream>(mData, mSize);

		return bs_shared_ptr_new<MemoryDataStream>(*this);
	}

	void MemoryDataStream::close()
	{
		if (mData != nullptr)
		{
			if(mOwnsMemory)
				bs_free(mData);

			mData = nullptr;
		}
	}

	void MemoryDataStream::realloc(size_t numBytes)
	{
		if (numBytes != mSize)
		{
			assert(numBytes > mSize);

			// Note: Eventually add support for custom allocators
			auto buffer = bs_allocN<uint8_t>(numBytes);
			if (mData)
			{
				mCursor = buffer + (mCursor - mData);
				mEnd = buffer + (mEnd - mData);

				memcpy(buffer, mData, mSize);
				bs_free(mData);
			}
			else
			{
				mCursor = buffer;
				mEnd = buffer;
			}

			mData = buffer;
			mSize = numBytes;
		}
	}

	FileDataStream::FileDataStream(const Path& path, AccessMode accessMode, bool freeOnClose)
		: DataStream(accessMode), mPath(path), mFreeOnClose(freeOnClose)
	{
		// Always open in binary mode
		// Also, always include reading
		std::ios::openmode mode = std::ios::binary;

		if ((accessMode & READ) != 0)
			mode |= std::ios::in;

		if (((accessMode & WRITE) != 0))
		{
			mode |= std::ios::out;
			mFStream = bs_shared_ptr_new<std::fstream>();
			mFStream->open(path.toPlatformString().c_str(), mode);
			mInStream = mFStream;
		}
		else
		{
			mFStreamRO = bs_shared_ptr_new<std::ifstream>();
			mFStreamRO->open(path.toPlatformString().c_str(), mode);
			mInStream = mFStreamRO;
		}

		// Should check ensure open succeeded, in case fail for some reason.
		if (mInStream->fail())
		{
			BS_LOG(Warning, FileSystem, "Cannot open file: " + path.toString());
			return;
		}

		mInStream->seekg(0, std::ios_base::end);
		mSize = (size_t)mInStream->tellg();
		mInStream->seekg(0, std::ios_base::beg);
	}

	FileDataStream::~FileDataStream()
	{
		close();
	}

	size_t FileDataStream::read(void* buf, size_t count) const
	{
		mInStream->read(static_cast<char*>(buf), static_cast<std::streamsize>(count));

		return (size_t)mInStream->gcount();
	}

	size_t FileDataStream::write(const void* buf, size_t count)
	{
		size_t written = 0;
		if (isWriteable() && mFStream)
		{
			mFStream->write(static_cast<const char*>(buf), static_cast<std::streamsize>(count));
			written = count;
		}

		return written;
	}
	void FileDataStream::skip(size_t count)
	{
		mInStream->clear(); // Clear fail status in case eof was set

		if (((mAccess & WRITE) != 0))
			mFStream->seekp(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
		else
			mInStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
	}

	void FileDataStream::seek(size_t pos)
	{
		mInStream->clear(); // Clear fail status in case eof was set

		if (((mAccess & WRITE) != 0))
			mFStream->seekp(static_cast<std::ifstream::pos_type>(pos), std::ios::beg);
		else
			mInStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
	}

	size_t FileDataStream::tell() const
	{
		mInStream->clear(); // Clear fail status in case eof was set

		if (((mAccess & WRITE) != 0))
			return (size_t)mFStream->tellp();

		return (size_t)mInStream->tellg();
	}

	bool FileDataStream::eof() const
	{
		return mInStream->eof();
	}

	SPtr<DataStream> FileDataStream::clone(bool copyData) const
	{
		return bs_shared_ptr_new<FileDataStream>(mPath, (AccessMode)getAccessMode(), true);
	}

	void FileDataStream::close()
	{
		if (mInStream)
		{
			if (mFStreamRO)
				mFStreamRO->close();

			if (mFStream)
			{
				mFStream->flush();
				mFStream->close();
			}

			if (mFreeOnClose)
			{
				mInStream = nullptr;
				mFStreamRO = nullptr;
				mFStream = nullptr;
			}
		}
	}
}
