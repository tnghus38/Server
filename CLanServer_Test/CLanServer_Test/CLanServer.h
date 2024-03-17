#pragma once
#ifndef __CLanServer__
#define __CLanServer__
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib,"ws2_32")
#include <WinSock2.h>
#include <map>
#include "CRingBuff.h"
#include "CPacket.h"
using namespace std;

#define dfBUFF_SIZE 30000

struct st_ServerOption
{
	WCHAR IP[20] = L"127.0.0.1";
	int Port = 6000;
	int WorkerMax = 8;
	int RunMax = 0;
	bool nagle = true;
	int SesseionMax = 1000;  
};

struct TypeOverLapped
{
	OVERLAPPED Overlapped;
	int Type;
};

struct st_SESSION
{
	
	int m_Session_ID;
	SOCKET m_socket;
	CRingBuff m_recvRingbuff;
	CRingBuff m_sendRingbuff;
	TypeOverLapped m_recvOverlapped;
	TypeOverLapped m_sendOverlapped;
	int m_IoCount;
	bool m_bSendio;
	bool m_bSessionOn;

	int m_SendPacketCount = 0;
	st_SESSION()
	{
		m_bSessionOn = false;
	};
};



class CLanServer
{
public:
	CLanServer();
	~CLanServer();
	bool Start(st_ServerOption);// 오픈 IP / 포트 / 워커스레드 수(생성수, 러닝수) / 나글옵션 / 최대접속자 수
	void Stop();
	int GetSessionCount();

	bool Disconnect(int SessionID);// / SESSION_ID / HOST_ID
	bool SendPacket(int SessionID, CPacket*);// / SESSION_ID / HOST_ID

//	virtual bool OnConnectionRequest(WCHAR* IP, int Port) = 0; //< accept 직후 ip 차단용
	
//	virtual void OnClientJoin( int SessionID , st_SESSION* Session) = 0; // < Accept 후 접속처리 완료 후 호출.
//	virtual void OnClientLeave(int SessionID) = 0;  //< Release 후 호출


	virtual void OnRecv(int SessionID, CPacket*) = 0;// < 패킷 수신 완료 후
	// virtual void OnSend(SessionID, int sendsize) = 0;           < 패킷 송신 완료 후

	// virtual void OnWorkerThreadBegin() = 0;                    < 워커스레드 GQCS 바로 하단에서 호출
	// virtual void OnWorkerThreadEnd() = 0;                      < 워커스레드 1루프 종료 후

	//virtual void OnError(int errorcode, WCHAR*) = 0;
private:
	WCHAR m_IP[20];
	int m_Port;
	int m_WorkerMax;
	int m_RunMax;
	bool m_nagle;
	int m_SesseionMax; // 0제한없음 

	st_SESSION* m_SessionArr;
	SOCKET m_ListenSocket;
	HANDLE m_hcp;
	int m_SessionID=0;
	HANDLE hTread[10];
	CRITICAL_SECTION cs;

	static DWORD WINAPI WorkerThread(LPVOID);
	static DWORD WINAPI AcceptThread(LPVOID); 

	st_SESSION* FindSession(int);
	void RemoveSession(int);
	bool CheckCloseSock(SOCKET);

	void RecvPost(st_SESSION*);
	void SendPost(st_SESSION*);
};




#endif