#ifndef MEMORYINPUTSTREAM_H
#define MEMORYINPUTSTREAM_H

#include "vgui_inputstream.h"

class MemoryInputStream : public vgui::InputStream
{
public:
	MemoryInputStream(void);
	MemoryInputStream(uchar* pData, int nLength);
	virtual ~MemoryInputStream(void);

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

#endif