#pragma once
#ifndef __CPacket__
#define __CPacket__
#include <stdlib.h>
#include <process.h>
#include <memory.h> 
#include <windows.h>
class CRingBuff
{
public:
	CRingBuff();
	~CRingBuff();
	CRingBuff(int BuffSize);

	void init(int size);

	void Lock(int type=0);
	void UnLock();

	int Enqueue(const char* pBuffer, int size);
	int Dequeue(char* pBuffer, int size,bool isch=false);
	int Peek(char* pBuffer, int size);


	bool IsEmpty();
	bool IsFull();

	int GetSize();//총사이즈
	int GetUseSize();//지금 사용중인 사이즈 
	int GetFreeSize();//남은 사이즈 
	int GetDirectReadSize(); //읽을수있는 사이즈 (짤려서 앞뒤 나눠 들어갔을경우 현재 한번에 읽히는 크기) 
	int GetDirectWriteSize(); //쓸수 있는 사이즈 (짤려서 앞뒤 나눠 들어갔을경우 현재 한번에 쓰이는 크기) 

	char* GetFronPos();//지금 읽고있는 위치 포인터 
	char* GetRearPos();//지금 쓰고있는 위치 포인터 

	void MoveFrontPos(int size);//해당 사이즈만큼 이동 
	void MoveRearPos(int size);//해당 사이즈만큼 이동 

	void ClearBuffer();

	char* GetBuffer();


	char* GetUseWriteBuffer();
	char* GetUseReadBuffer();
private:
	SRWLOCK m_lock;
	int LockType;
	int m_front;
	int m_rear;
	int m_size;
	int ff;
	char* m_pBuffer;
};



#endif