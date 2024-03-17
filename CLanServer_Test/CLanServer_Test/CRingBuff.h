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

	int GetSize();//�ѻ�����
	int GetUseSize();//���� ������� ������ 
	int GetFreeSize();//���� ������ 
	int GetDirectReadSize(); //�������ִ� ������ (©���� �յ� ���� ������� ���� �ѹ��� ������ ũ��) 
	int GetDirectWriteSize(); //���� �ִ� ������ (©���� �յ� ���� ������� ���� �ѹ��� ���̴� ũ��) 

	char* GetFronPos();//���� �а��ִ� ��ġ ������ 
	char* GetRearPos();//���� �����ִ� ��ġ ������ 

	void MoveFrontPos(int size);//�ش� �����ŭ �̵� 
	void MoveRearPos(int size);//�ش� �����ŭ �̵� 

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