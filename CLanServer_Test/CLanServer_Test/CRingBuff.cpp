#include "CRingBuff.h"
#include <windows.h>

#include <process.h>
CRingBuff::CRingBuff()
{
	m_pBuffer = (char*)malloc(512);
	m_front = 0;
	m_rear = 0;
	m_size = 512;
	LockType = -1;
	InitializeSRWLock(&m_lock);
}

CRingBuff::~CRingBuff()
{
}

CRingBuff::CRingBuff(int BuffSize = 512)
{
	m_pBuffer = (char*)malloc(BuffSize);
	m_front = 0;
	m_rear = 0;
	m_size = BuffSize;
	LockType = -1;
	InitializeSRWLock(&m_lock);

}
void CRingBuff::Lock(int type)
{

	LockType = type;
	if (LockType == 0)
		AcquireSRWLockExclusive(&m_lock);
	if (LockType == 1)
		AcquireSRWLockShared(&m_lock);
}

void CRingBuff::UnLock()
{
	if (LockType == 0)
		ReleaseSRWLockExclusive(&m_lock);

	if (LockType == 1)
		ReleaseSRWLockShared(&m_lock);

}

void CRingBuff::init(int BuffSize)
{
	if (m_pBuffer != NULL)
		free(m_pBuffer);
	m_pBuffer = (char*)malloc(BuffSize);
	m_front = 0;
	m_rear = 0;
	m_size = BuffSize;
	LockType = -1;
}

int CRingBuff::Enqueue(const char* pBuffer, int size)
{
	if (size == 0 || pBuffer == NULL || IsFull())
		return 0;

	int writeSize = GetDirectWriteSize();

	if (writeSize >= size)//�ѹ��� ���������� 
	{
		int startindex = (m_rear) % (m_size); //tail+1 �� �ش� ���� ������ �Ѿ�������� %m_size

		memcpy_s(&m_pBuffer[startindex], size, pBuffer, size);
		MoveRearPos(size);// ���� ��ġ �ű� 
		return size; // ������ ���� 
	}
	else
	{
		int startindex = (m_rear) % (m_size); //tail+1 �� �ش� ���� ������ �Ѿ�������� %m_size


		//MoveRearPos(writeSize);// ���� ��ġ �ű� 

		memcpy_s(&m_pBuffer[startindex], writeSize, pBuffer, writeSize);

		int tmp_rear = (m_rear + writeSize) % (m_size);
		if ((m_front == tmp_rear + 1) || (m_front == 0 && tmp_rear == m_size - 1))
		{
			MoveRearPos(writeSize);

			return writeSize;
		}

		int curSize = size - writeSize;//���� ������ϴ� ������ 
		int curWriteSize = m_front - 1;// �� �����ִ»����� 

		if (curWriteSize >= curSize) // ������ϴ� ��ŭ �ڸ��� ���������� 
			curWriteSize = curSize; // ������� 
		//�ƴϸ� ���� �������ִ¸�ŭ�� ������ �󸶳� ����� �������ָ�� 

		int curindex = (tmp_rear) % (m_size);

		memcpy_s(&m_pBuffer[curindex], curWriteSize, &pBuffer[writeSize], curWriteSize);

		MoveRearPos(curWriteSize + writeSize);// ���� ��ġ �ű� 

		return writeSize + curWriteSize;

	}

}
int CRingBuff::Dequeue(char* pBuffer, int size, bool isch)
{
	int readSize = Peek(pBuffer, size);
	if (isch)
		pBuffer[size] = '\0';
	MoveFrontPos(readSize);

	return readSize;
}
int CRingBuff::Peek(char* pBuffer, int size)
{
	if (size == 0 || pBuffer == NULL || IsEmpty())
		return 0;

	int readSize = GetDirectReadSize();

	if (readSize >= size)
	{
		int startindex = (m_front) % (m_size);
		memcpy_s(pBuffer, size, &m_pBuffer[startindex], size);
		//pBuffer[size] = '\0';
		return size;
	}
	else
	{
		int startindex = (m_front) % (m_size);
		memcpy_s(pBuffer, readSize, &m_pBuffer[startindex], readSize);

		int curSize = size - readSize;//���� �� �о�� �ϴ� ������ 
		int curReadSize = GetUseSize();// �� ���� �� �ִ»����� 

		if (curReadSize >= curSize) // �� �о���ϴ� ��ŭ �ڸ��� ���������� 
			curReadSize = curSize; // ������� 



		int curindex = (m_front + readSize) % (m_size);
		memcpy_s(pBuffer + readSize, curReadSize, &m_pBuffer[curindex], curReadSize);
		//pBuffer[readSize + curReadSize] = '\0';

		return readSize + curReadSize;
	}
}

bool CRingBuff::IsEmpty()
{
	if (m_front == m_rear)
		return true;
	else
		return false;
	//return m_front == m_rear;
}
bool CRingBuff::IsFull()
{
	if ((m_front == m_rear + 1) || (m_front == 0 && m_rear == m_size - 1))
		return true;
	else
		return false;
}

int CRingBuff::GetSize()
{
	return m_size - 1;
}
int CRingBuff::GetUseSize()
{
	if (IsEmpty())
		return 0;
	if (IsFull())
		return m_size - 1;

	if (m_front < m_rear)
		return m_rear - m_front;
	else
	{

		return m_size - (m_front - m_rear);
	}

}
int CRingBuff::GetFreeSize()
{

	return (m_size - 1) - GetUseSize();
}

//�������ִ� ������ (©���� �յ� ���� ������� ���� �ѹ��� ������ ũ��) 
int CRingBuff::GetDirectReadSize()
{
	if (IsEmpty())
	{
		return 0;
	}

	int sub = m_rear - m_front;
	if (sub > 0)
		//	if (m_front < m_rear)
	{
		//int s = m_rear - m_front;
		int s = sub;
		if (s < 0)
		{
			return 0;
		}
		return s;
	}
	else
	{
		int s = m_size - m_front;
		if (s < 0)
		{
			return 0;
		}
		return	m_size - m_front;
	}

}

//���� �ִ� ������ (©���� �յ� ���� ������� ���� �ѹ��� ���̴� ũ��) 
int CRingBuff::GetDirectWriteSize()
{
	if (IsFull())
	{
		return 0;
	}

	if (IsEmpty())
	{
		int s = m_size - m_rear;
		return s;
	}

	int front = m_front;
	int rear = m_rear;
	if (front < rear)
	{
		if (front == 0)
		{
			int tmp = (rear + 1) % m_size;
			int s = m_size - (tmp);
			if (s < 0)
			{
				return 0;
			}
			return s;
		}
		else
		{
			int s = m_size - rear;
			if (s < 0)
			{
				return 0;
			}
			return s;
		}
	}
	else
	{
		int size = (front - 1) - (rear);

		if (size < 0)
		{
			if (IsEmpty())
			{

				int s = m_size - m_rear;
				return s;
			}
			return 0;
		}
		return	size;
	}

}

//���� �а��ִ� ��ġ ������ 
char* CRingBuff::GetFronPos()
{
	return &m_pBuffer[m_front];
}

//���� �����ִ� ��ġ ������ 
char* CRingBuff::GetRearPos()
{
	return &m_pBuffer[m_rear];
}

//�ش� �����ŭ �̵� 
void CRingBuff::MoveFrontPos(int size)
{
	m_front = (m_front + size) % (m_size);
}

//�ش� �����ŭ �̵� 
void CRingBuff::MoveRearPos(int size)
{
	m_rear = (m_rear + size) % (m_size);
}

void CRingBuff::ClearBuffer()
{
	m_rear = m_front;
}

char* CRingBuff::GetBuffer()
{
	return m_pBuffer;
}
char* CRingBuff::GetUseWriteBuffer()
{
	int startindex = (m_rear) % (m_size);

	return &m_pBuffer[startindex];
}

char* CRingBuff::GetUseReadBuffer()
{
	int startindex = (m_front) % (m_size);

	return &m_pBuffer[startindex];
}