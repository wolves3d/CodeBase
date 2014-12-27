#ifndef __BufferObject_h_included__
#define __BufferObject_h_included__
#include "stdlib.h"

class BufferObject
{
	size_t m_bufferSize;
	void * m_bufferPointer;

public:

	BufferObject()
		: m_bufferPointer(NULL)
		, m_bufferSize(0)
	{
	}

	BufferObject(size_t size)
		: BufferObject()
	{
		Init(size);
	}

	virtual ~BufferObject()
	{
		Release();
	}

	void Release()
	{
		if (NULL != m_bufferPointer) {
			
			free(m_bufferPointer);
			m_bufferPointer = NULL;
		}

		m_bufferSize = 0;
	}

	bool Init(size_t size)
	{
		Release();

		if (0 != size)
		{
			m_bufferPointer = malloc(size);

			if (NULL != m_bufferPointer)
			{
				m_bufferSize = size;
				return true;
			}
		}

		return false;
	}

	bool Init(size_t size, const void * dataSource)
	{
		if (true == Init(size))
		{
			memcpy(m_bufferPointer, dataSource, size);
			return true;
		}

		return false;
	}

	bool Init(const BufferObject & sourceObject)
	{
		return Init(
			sourceObject.GetSize(),
			sourceObject.GetConstPointer() );
	}

	size_t			GetSize			() const	{ return m_bufferSize; }
	void *			GetPointer		()			{ return m_bufferPointer; }
	const void *	GetConstPointer	() const	{ return m_bufferPointer; }


	uint Write(uint offset, void * dataSource, uint sizeInBytes)
	{
		if (NULL != dataSource)
		{
			if (m_bufferSize > (offset + sizeInBytes))
			{
				if (sizeInBytes > 0)
				{
					memcpy(((char *)m_bufferPointer + offset), dataSource, sizeInBytes);
				}
				
				return (offset + sizeInBytes);
			}
			else
			{
				// buffer overflow!
			}
		}
		else
		{
			// invalid data source!
		}
		
		return 0;
	}

	bool Compare(uint offset, void * dataSource, uint sizeInBytes)
	{
		if (m_bufferSize > (offset + sizeInBytes))
		{
			return (0 == memcmp(((char *)m_bufferPointer + offset), dataSource, sizeInBytes));
		}
		else
		{
			// buffer overflow!
		}

		return false;
	}
};

#endif // #ifndef __BufferObject_h_included__