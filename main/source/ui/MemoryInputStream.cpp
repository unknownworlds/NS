#include "MemoryInputStream.h"
#include <memory.h>
#include <stdlib.h>

MemoryInputStream::MemoryInputStream(void) : m_pData(NULL), m_DataLen(0), m_ReadPos(0) {}
MemoryInputStream::MemoryInputStream(uchar* pData, int nLength) : m_pData(pData), m_DataLen(0), m_ReadPos(0) {}

MemoryInputStream::~MemoryInputStream(void) {}

void MemoryInputStream::setSource(uchar* pData, int nLength)
{
	m_pData = pData;
	m_DataLen = nLength;
	m_ReadPos = 0;
}

void MemoryInputStream::seekStart(bool& success)
{
	m_ReadPos = 0;
	success = (m_pData != NULL);
}

void MemoryInputStream::seekRelative(int count, bool& success)
{
	int newPos = m_ReadPos + count;
	if( m_pData != NULL && newPos >= 0 && newPos <= m_DataLen )
	{
		m_ReadPos = newPos;
		success = true;
	}
	else
	{
		success = false;
	}
}

void MemoryInputStream::seekEnd(bool& success)
{
	m_ReadPos = m_DataLen;
	success = (m_pData != NULL);
}

int MemoryInputStream::getAvailable(bool& success)
{
	success = (m_pData != NULL && m_ReadPos != m_DataLen);
	return m_DataLen - m_ReadPos;
}

uchar MemoryInputStream::readUChar(bool& success)
{
	if( m_pData != NULL && m_ReadPos != m_DataLen )
	{
		success = true;
		uchar val = m_pData[m_ReadPos];
		++m_ReadPos;
		return val;
	}
	success = false;
	return 0;
}

void MemoryInputStream::readUChar(uchar* buf, int count, bool& success)
{
	if( m_pData != NULL )
	{
		int diff = m_DataLen - m_ReadPos;
		int numToRead = count < diff ? count : diff;
		memcpy(buf,m_pData,numToRead);
		success = (numToRead == count);
		return;
	}
	success = false;
}

void MemoryInputStream::close(bool& success)
{
	m_pData = NULL;
	m_DataLen = 0;
	m_ReadPos = 0;
	success = true;
}
/*
#include "vgui_inputstream.h"

class MemoryInputStream : public vgui::InputStream
{
public:
	MemoryInputStream();
	MemoryInputStream(uchar* pData, int nLength);
	virtual MemoryInputStream(void);

	virtual void setSource(uchar* pData, int nLength);

	virtual void seekStart(bool& success);
	virtual void seekRelative(int count, bool& success);
	virtual void seekEnd(bool& success);
	virtual int  getAvailable(bool& success);
	virtual uchar readUChar(bool& success);
	virtual void readUChar(uchar* buf, int count, bool& success);
	virtual void close(bool& success);
private:
	uchar* m_pData;
	int m_DataLen;
	int m_ReadPos;
};
*/