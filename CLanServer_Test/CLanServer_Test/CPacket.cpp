#include "CPacket.h"


CPacket::CPacket()
{
	m_size = 512;
	m_cursize = HeaderSize;
	m_Buffer = (char*)malloc(512);
}

CPacket::CPacket(int size)
{
	m_size = size;
	m_cursize = HeaderSize;
	m_Buffer = (char*)malloc(size);
}

CPacket::~CPacket()
{
	free(m_Buffer);
}


void CPacket::SetHeader(short hearder)
{
	memcpy_s(&m_Buffer[0], sizeof(short), (char*)hearder, sizeof(short));
}

void CPacket::Clear()
{
	m_cursize = 0;
}

int CPacket::GetTotalUseSize()
{
	return m_cursize;
}

int CPacket::GetUseSize()
{
	return m_cursize - HeaderSize;
}
char* CPacket::GetBuffer()
{
	return &m_Buffer[HeaderSize];
}
void CPacket::SetCur(int size)
{
	m_cursize = size;
}
void CPacket::SetData(char* buff, int size)
{

	if (m_cursize + size <= m_size)
	{
		memcpy_s(&m_Buffer[m_cursize], size, buff, size);
		m_cursize += size;
	}


}

void CPacket::GetData(char* data, int size)
{

	if (m_cursize + size <= m_size)
	{
		memcpy_s(data, size, &m_Buffer[HeaderSize], size);
		memcpy_s(&m_Buffer[HeaderSize], m_cursize - HeaderSize, &m_Buffer[size], m_cursize - HeaderSize);
		m_cursize -= size;
	}


}

char* CPacket::GetHeaderBuffer()
{
	return m_Buffer;
}

void CPacket::SetHeader(char* buff)
{

	memcpy_s(&m_Buffer[0], HeaderSize, buff, HeaderSize);
}

void CPacket::GetHeader(char* data)
{

	memcpy_s(data, HeaderSize, &m_Buffer[0], HeaderSize);
}

CPacket& CPacket::operator<<(byte data)
{
	if (m_cursize + sizeof(byte) <= m_size)
	{
		memcpy_s(&m_Buffer[m_cursize], sizeof(byte), (char*)&data, sizeof(byte));
		m_cursize += sizeof(byte);
	}
	return *this;
}

CPacket& CPacket::operator>>(byte& data)
{
	if (m_cursize >= sizeof(byte))
	{
		memcpy_s(&data, sizeof(byte), &m_Buffer[HeaderSize], sizeof(byte));
		memcpy_s(&m_Buffer[HeaderSize], m_cursize - HeaderSize, &m_Buffer[sizeof(byte)], m_cursize - HeaderSize);
		m_cursize -= sizeof(byte);
	}
	return *this;
}

CPacket& CPacket::operator<<(int data)
{
	if (m_cursize + sizeof(int) <= m_size)
	{
		memcpy_s(&m_Buffer[m_cursize], sizeof(int), (char*)&data, sizeof(int));
		m_cursize += sizeof(int);
	}
	return *this;
}

CPacket& CPacket::operator>>(int& data)
{
	if (m_cursize >= sizeof(int))
	{
		memcpy_s(&data, sizeof(int), &m_Buffer[HeaderSize], sizeof(int));
		memcpy_s(&m_Buffer[HeaderSize], m_cursize - HeaderSize, &m_Buffer[sizeof(int)], m_cursize - HeaderSize);
		m_cursize -= sizeof(int);
	}
	return *this;
}

CPacket& CPacket::operator<<(WORD data)
{
	if (m_cursize + sizeof(WORD) <= m_size)
	{
		memcpy_s(&m_Buffer[m_cursize], sizeof(WORD), (char*)&data, sizeof(WORD));
		m_cursize += sizeof(WORD);
	}
	return *this;
}

CPacket& CPacket::operator>>(WORD& data)
{
	if (m_cursize >= sizeof(WORD))
	{
		memcpy_s(&data, sizeof(WORD), &m_Buffer[HeaderSize], sizeof(WORD));
		memcpy_s(&m_Buffer[HeaderSize], m_cursize - HeaderSize, &m_Buffer[sizeof(WORD)], m_cursize - HeaderSize);
		m_cursize -= sizeof(WORD);
	}
	return *this;
}
CPacket& CPacket::operator<<(DWORD data)
{
	if (m_cursize + sizeof(DWORD) <= m_size)
	{
		memcpy_s(&m_Buffer[m_cursize], sizeof(DWORD), (char*)&data, sizeof(DWORD));
		m_cursize += sizeof(DWORD);
	}
	return *this;
}

CPacket& CPacket::operator>>(DWORD& data)
{
	if (m_cursize >= sizeof(DWORD))
	{
		memcpy_s(&data, sizeof(DWORD), &m_Buffer[HeaderSize], sizeof(DWORD));
		memcpy_s(&m_Buffer[HeaderSize], m_cursize - HeaderSize, &m_Buffer[sizeof(DWORD)], m_cursize - HeaderSize);
		m_cursize -= sizeof(DWORD);
	}
	return *this;
}
CPacket& CPacket::operator<<(short data)
{
	if (m_cursize + sizeof(short) <= m_size)
	{
		memcpy_s(&m_Buffer[m_cursize], sizeof(short), (char*)&data, sizeof(short));
		m_cursize += sizeof(short);
	}
	return *this;
}

CPacket& CPacket::operator>>(short& data)
{
	if (m_cursize >= sizeof(short))
	{
		memcpy_s(&data, sizeof(short), &m_Buffer[HeaderSize], sizeof(short));
		memcpy_s(&m_Buffer[HeaderSize], m_cursize - HeaderSize, &m_Buffer[sizeof(short)], m_cursize - HeaderSize);
		m_cursize -= sizeof(short);
	}
	return *this;
}


CPacket& CPacket::operator<<(double data)
{
	if (m_cursize + sizeof(double) <= m_size)
	{
		memcpy_s(&m_Buffer[m_cursize], sizeof(double), (char*)&data, sizeof(double));
		m_cursize += sizeof(double);
	}
	return *this;
}

CPacket& CPacket::operator>>(double& data)
{
	if (m_cursize >= sizeof(double))
	{
		memcpy_s(&data, sizeof(double), &m_Buffer[HeaderSize], sizeof(double));
		memcpy_s(&m_Buffer[HeaderSize], m_cursize - HeaderSize, &m_Buffer[sizeof(double)], m_cursize - HeaderSize);
		m_cursize -= sizeof(double);
	}
	return *this;
}