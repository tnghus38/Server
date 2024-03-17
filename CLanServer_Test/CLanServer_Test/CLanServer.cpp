#include "CLanServer.h"


CLanServer::CLanServer()
{
}

CLanServer::~CLanServer()
{
}


DWORD WINAPI CLanServer::WorkerThread(LPVOID arg)
{
	CLanServer* server=(CLanServer*)arg;
	
	int retavl;
	HANDLE hcp = server->m_hcp;

	while (1)
	{
		DWORD cbTransferred = 0;
		st_SESSION* client_Session = NULL;
		TypeOverLapped* ptr = NULL;

		GetQueuedCompletionStatus(hcp, &cbTransferred, (PULONG_PTR)&client_Session, (LPOVERLAPPED*)&ptr, INFINITE);

		if (client_Session == NULL)
		{
			printf("���¼��� �����Դϴ�?\n");
			continue;
		}

		if (ptr == NULL)
		{
			printf("�۾����� or Ÿ�Ӿƿ�???\n");
			//�������
			server->CheckCloseSock(client_Session->m_socket);
			server->RemoveSession(client_Session->m_Session_ID);
			continue;

		}






		if (cbTransferred == 0)
		{
			//�������

			if (ptr->Type == 0)
				printf("������ 0 \n");
			if (ptr->Type == 1)
				printf("������ 0 \n");

			InterlockedDecrement((LONG*)&client_Session->m_IoCount);
			server->CheckCloseSock(client_Session->m_socket);
			server->RemoveSession(client_Session->m_Session_ID);
			continue;
		}


		if (ptr->Type == 0)
		{
			client_Session->m_recvRingbuff.MoveRearPos(cbTransferred);

			
			while (client_Session->m_recvRingbuff.GetUseSize()>=HeaderSize)
			{
				CPacket _pack(2000);
				short header;
				client_Session->m_recvRingbuff.Peek((char*)&header, HeaderSize);
				
				int buffsize = client_Session->m_recvRingbuff.GetUseSize();
				if (header != 8)
				{
					client_Session->m_recvRingbuff.Peek((char*)&header, HeaderSize);
					break;
				}
				if (header + HeaderSize <= buffsize)
				{
					client_Session->m_recvRingbuff.Dequeue(_pack.GetHeaderBuffer(), HeaderSize+header);
					_pack.SetCur(header + HeaderSize);
					
					server->OnRecv(client_Session->m_Session_ID, &_pack);
				}
				else
				{
					//printf("[%d] ������ũ��: %d \n", client_Session->m_Session_ID, client_Session->m_recvRingbuff.GetUseSize());
					break;
				}
			}
			

			

			server->RecvPost(client_Session);
		}

		if (ptr->Type == 1)
		{
			//printf("��ȣ��2 \n");
			
			for (int i = 0; i < client_Session->m_SendPacketCount; i++)
			{
				CPacket* c=NULL;

				if (client_Session->m_sendRingbuff.GetUseSize() < sizeof(CPacket*))
				{
					printf("�̰� �߸�ȵǴµ�;; ��ť �̻��ѵ�?\n");
				}
				client_Session->m_sendRingbuff.Dequeue((char*)&c, sizeof(CPacket*));
				if(c !=NULL)
					delete c;
			}
			client_Session->m_bSendio = false;
			if (client_Session->m_sendRingbuff.GetUseSize() > 0 && client_Session->m_bSendio == false)
			{
				//printf("���峲���ź��� ? \n");
				server->SendPost(client_Session);
			}


		}
		InterlockedDecrement((LONG*)&client_Session->m_IoCount);
		/*	if (session->m_IoCount == 0)
			{
				printf("??");
			}*/
		server->RemoveSession(client_Session->m_Session_ID);
	}

	printf("����������\n");

	return 0;
}
DWORD WINAPI CLanServer::AcceptThread(LPVOID arg)
{
	CLanServer* server = (CLanServer*)arg;
	while (true)
	{
		//accept

		SOCKET Sock;

		SOCKADDR_IN sockaddr;

		int addrlen = sizeof(sockaddr);

		Sock = accept(server->m_ListenSocket, (SOCKADDR*)&sockaddr, &addrlen);

		if (Sock == INVALID_SOCKET)
		{
			printf("IO SOCKET ����");
			break;
		}
		bool opt = server->m_nagle;
		int retval = setsockopt(Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt));
		if (retval == SOCKET_ERROR)
		{
			printf("�ɼǿ���");
		}
		
		printf("\n [TCP �ּ�] Ŭ���̾�Ʈ ���� : IP �ּ� = %s, ��Ʈ ��ȣ = %d\n", inet_ntoa(sockaddr.sin_addr), ntohs(sockaddr.sin_port));

		//���� ���� 

		st_SESSION* client =NULL;
		for (int i=0; i < server->m_SesseionMax; i++)
		{
			if (server->m_SessionArr[i].m_bSessionOn == false)
			{
				client = &server->m_SessionArr[i];
				break;
			}
		}
		if (client == NULL)
		{
			continue;
		}
		client->m_Session_ID = server->m_SessionID;

		InterlockedIncrement((LONG*)&server->m_SessionID);
		client->m_socket = Sock;
		client->m_bSendio = false;
		client->m_IoCount = 0;
		client->m_recvRingbuff.init(dfBUFF_SIZE);
		client->m_sendRingbuff.init(dfBUFF_SIZE);
		client->m_bSessionOn = true;
		ZeroMemory(&client->m_recvOverlapped.Overlapped, sizeof(client->m_recvOverlapped.Overlapped));
		ZeroMemory(&client->m_sendOverlapped.Overlapped, sizeof(client->m_sendOverlapped.Overlapped));
		client->m_recvOverlapped.Type = 0;
		client->m_sendOverlapped.Type = 1;

		//EnterCriticalSection(&server->cs);
		//server->m_SessionMap.insert({ client->m_Session_ID, client });
		//LeaveCriticalSection(&server->cs);

		//����� �Ϸ� ��Ʈ ���� 
		CreateIoCompletionPort((HANDLE)client->m_socket, server->m_hcp, (ULONG_PTR)client, 0);


		//Recv

		server->RecvPost(client);
	}

	return 0;
}
bool CLanServer::Start(st_ServerOption opt)// ���� IP / ��Ʈ / ��Ŀ������ ��(������, ���׼�) / ���ۿɼ� / �ִ������� ��
{

	InitializeCriticalSection(&cs);

	lstrcpyW( m_IP, opt.IP);
	m_Port= opt.Port;
	m_WorkerMax= opt.WorkerMax;
	m_RunMax= opt.RunMax;
	m_nagle= opt.nagle;
	m_SesseionMax= opt.SesseionMax;
	m_SessionArr = new st_SESSION[m_SesseionMax];//(st_SESSION*)malloc(m_SesseionMax * sizeof(st_SESSION));
	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}

	//����¿Ϸ� ��Ʈ 
	m_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_RunMax);
	if (m_hcp == NULL) return false;

	

	//�۾��� ������ ���� 
	
	for (int i = 0; i < m_WorkerMax; i++)
	{
		hTread[i] = CreateThread(NULL, 0, WorkerThread, this, 0, NULL);
		if (hTread[i] == NULL)
			return false;
		//CloseHandle(hTread);
	}
	//socket

	m_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_ListenSocket == INVALID_SOCKET)
	{
		printf("SOCKET ����");
		return false;
	}

	//bind
	SOCKADDR_IN 	serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(m_Port);

	int retval = ::bind(m_ListenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR)
	{
		printf("Bind ����");
		return false;
	}

	//Listen
	retval = listen(m_ListenSocket, SOMAXCONN);

	if (retval == SOCKET_ERROR)
	{
		printf("Listen ����");
		return false;
	}

	hTread[m_WorkerMax] = CreateThread(NULL, 0, AcceptThread, this, 0, NULL);
	return true;
}
void CLanServer::Stop()
{
	delete[] m_SessionArr;
}
int CLanServer::GetSessionCount()
{
	return 0;
}


void CLanServer::RecvPost(st_SESSION* session)
{
	DWORD recvbytes;
	DWORD flags = 0;
	WSABUF wsabuf;
	wsabuf.buf = session->m_recvRingbuff.GetUseWriteBuffer();
	wsabuf.len = session->m_recvRingbuff.GetDirectWriteSize();

	InterlockedIncrement((LONG*)&session->m_IoCount);

	//printf("���ú� \n");
	int retval = WSARecv(session->m_socket, &wsabuf, 1, &recvbytes, &flags, (OVERLAPPED*)&session->m_recvOverlapped, NULL);

	if (retval == SOCKET_ERROR)
	{
		int ErrorCode = WSAGetLastError();
		if (ErrorCode != ERROR_IO_PENDING)
		{
			printf("���ú�ó������ \n");
			InterlockedDecrement((LONG*)&session->m_IoCount);
			printf("305ErroCode %d\n", ErrorCode);
			CheckCloseSock(session->m_socket);
			RemoveSession(session->m_socket);
		}

	}
}

void CLanServer::SendPost(st_SESSION* session)
{

	bool bSend= InterlockedExchange((LONG*)&session->m_bSendio,(LONG)TRUE);
	if (bSend == false)
	{
		//printf("���� \n");
		DWORD cbTransferred;
		WSABUF wsabuf[1000];
		session->m_SendPacketCount = session->m_sendRingbuff.GetUseSize() / sizeof(CPacket*);
		if (session->m_SendPacketCount > 1000)
			session->m_SendPacketCount = 1000;

		if (session->m_sendRingbuff.GetUseSize() < sizeof(CPacket*) * session->m_SendPacketCount)
		{
			printf("�̰� �߸�ȵǴµ�;; ��ť �̻��ѵ�?\n");
		}

		CPacket* c[1000];

		session->m_sendRingbuff.Peek((char*)&c, sizeof(CPacket*) * session->m_SendPacketCount);
		for (int i = 0; i < session->m_SendPacketCount; i++)
		{

			printf("���̵�%d\n", session->m_Session_ID);
			wsabuf[i].buf = c[i]->GetHeaderBuffer();
			wsabuf[i].len = c[i]->GetTotalUseSize();
			short* s = (short*)c[i]->GetHeaderBuffer();
			double* d = (double*)c[i]->GetBuffer();
			if (*s != 8)
				printf("�����߸���\n");
			else
				printf("%d����%d %d\n", session->m_SendPacketCount, i, session->m_Session_ID);

		}
		InterlockedIncrement((LONG*)&session->m_IoCount);
		int retval = WSASend(session->m_socket, wsabuf, session->m_SendPacketCount, &cbTransferred, 0, (OVERLAPPED*)&session->m_sendOverlapped, NULL);

		if (retval == SOCKET_ERROR)
		{
			int ErrorCode = WSAGetLastError();
			if (ErrorCode != ERROR_IO_PENDING)
			{
				InterlockedDecrement((LONG*)&session->m_IoCount);
				printf(" 346 ErroCode %d\n", ErrorCode);
				if (ErrorCode == 10022)
				{

				}
				CheckCloseSock(session->m_socket);
				RemoveSession(session->m_socket);
			}

		}
	}
}
void CLanServer::RemoveSession(int sessionID)
{

	st_SESSION* session = FindSession(sessionID);
	if(session !=NULL)
	{
		if (session->m_IoCount == 0)
		{
			while (session->m_sendRingbuff.GetUseSize() >= sizeof(CPacket*))
			{
				CPacket* c= NULL;
				session->m_sendRingbuff.Dequeue((char*)&c, sizeof(CPacket*));
				if (c != NULL)
					delete c;
			}
			if(session->m_sendRingbuff.GetUseSize()>0)
				printf("��������\n");
			/*else
				printf("����\n");*/

		
			closesocket(session->m_socket);
			session->m_bSessionOn = false;
		
		}
	}
}


bool CLanServer::CheckCloseSock(SOCKET sock)
{
	/*SOCKET tmp = sock;
	SHORT isdelete = InterlockedExchange(&(sock), INVALID_SOCKET);

	if (isdelete != INVALID_SOCKET)
	{
		closesocket(tmp);
		return true;
	}*/

	return false;
}
bool CLanServer::Disconnect(int SessionID)
{
	return true;
}
bool CLanServer::SendPacket(int SessionID, CPacket* pack)
{
	st_SESSION* _session= FindSession(SessionID);
	if (_session == NULL)
		return false;

	short s = pack->GetUseSize();
	pack->SetHeader((char*)&s);
	if(_session->m_sendRingbuff.GetFreeSize()>= sizeof(CPacket*))
		_session->m_sendRingbuff.Enqueue((char*)&pack, sizeof(CPacket*));
	else
	{
		printf("%d", _session->m_sendRingbuff.GetFreeSize());
	}


	if (_session->m_bSendio == false)
	{
		SendPost(_session);
	}
	return true;
}

st_SESSION* CLanServer::FindSession(int SessionID)
{
	
	st_SESSION* result = NULL;

	for (int i=0; i < m_SesseionMax; i++)
	{
		if (m_SessionArr[i].m_Session_ID == SessionID)
		{
			result = &m_SessionArr[i];
			break;
		}
	}
	return result;
}