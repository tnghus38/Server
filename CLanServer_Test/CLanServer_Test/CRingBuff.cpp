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

	if (writeSize >= size)//한번에 쓸수있으면 
	{
		int startindex = (m_rear) % (m_size); //tail+1 이 해당 버퍼 범위를 넘어갈수있으니 %m_size

		memcpy_s(&m_pBuffer[startindex], size, pBuffer, size);
		MoveRearPos(size);// 꼬리 위치 옮김 
		return size; // 넣은값 리턴 
	}
	else
	{
		int startindex = (m_rear) % (m_size); //tail+1 이 해당 버퍼 범위를 넘어갈수있으니 %m_size


		//MoveRearPos(writeSize);// 꼬리 위치 옮김 

		memcpy_s(&m_pBuffer[startindex], writeSize, pBuffer, writeSize);

		int tmp_rear = (m_rear + writeSize) % (m_size);
		if ((m_front == tmp_rear + 1) || (m_front == 0 && tmp_rear == m_size - 1))
		{
			MoveRearPos(writeSize);

			return writeSize;
		}

		int curSize = size - writeSize;//지금 더써야하는 사이즈 
		int curWriteSize = m_front - 1;// 더 쓸수있는사이즈 

		if (curWriteSize >= curSize) // 더써야하는 만큼 자리가 남아있으면 
			curWriteSize = curSize; // 더쓰면됨 
		//아니면 지금 더쓸수있는만큼만 더쓰고 얼마나 썼는지 리턴해주면됨 

		int curindex = (tmp_rear) % (m_size);

		memcpy_s(&m_pBuffer[curindex], curWriteSize, &pBuffer[writeSize], curWriteSize);

		MoveRearPos(curWriteSize + writeSize);// 꼬리 위치 옮김 

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

		int curSize = size - readSize;//지금 더 읽어야 하는 사이즈 
		int curReadSize = GetUseSize();// 더 읽을 수 있는사이즈 

		if (curReadSize >= curSize) // 더 읽어야하는 만큼 자리가 남아있으면 
			curReadSize = curSize; // 읽으면됨 



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

//읽을수있는 사이즈 (짤려서 앞뒤 나눠 들어갔을경우 현재 한번에 읽히는 크기) 
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

//쓸수 있는 사이즈 (짤려서 앞뒤 나눠 들어갔을경우 현재 한번에 쓰이는 크기) 
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

//지금 읽고있는 위치 포인터 
char* CRingBuff::GetFronPos()
{
	return &m_pBuffer[m_front];
}

//지금 쓰고있는 위치 포인터 
char* CRingBuff::GetRearPos()
{
	return &m_pBuffer[m_rear];
}

//해당 사이즈만큼 이동 
void CRingBuff::MoveFrontPos(int size)
{
	m_front = (m_front + size) % (m_size);
}

//해당 사이즈만큼 이동 
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