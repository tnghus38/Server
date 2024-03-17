#pragma once
#ifndef __CPACKET__
#define __CPACKET__

#include <stdlib.h>
#include <memory.h> 
#include <Windows.h>

#define HeaderSize 2
class CPacket
{
public:
	CPacket();
	~CPacket();
	CPacket(int size);

	CPacket& operator << (byte data);
	CPacket& operator >> (byte& data);
	CPacket& operator << (int data);
	CPacket& operator >> (int& data);
	CPacket& operator << (WORD data);
	CPacket& operator >> (WORD& data);
	CPacket& operator << (DWORD data);
	CPacket& operator >> (DWORD& data);
	CPacket& operator << (short data);
	CPacket& operator >> (short& data);
	CPacket& operator << (double data);
	CPacket& operator >> (double& data);
	int GetTotalUseSize(); //��� ������ ������ 
	int GetUseSize(); //���̷ε� ������ 
	void Clear();
	char* GetBuffer(); // ���̷ε� ������ġ 

	void SetData(char*, int); 
	void SetCur(int);  
	void GetData(char*, int);

	char* GetHeaderBuffer(); //���� ó�� (����κ�)
	void SetHeader(char*);
	void GetHeader(char*);
	
private:

	void SetHeader(short hearder);
	int m_size;
	int m_cursize;
	char* m_Buffer;
};

#endif