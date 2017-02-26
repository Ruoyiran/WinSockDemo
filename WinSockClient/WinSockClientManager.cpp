#include "stdafx.h"
#include "WinSockClientManager.h"

const int kMaxRecvDataBufferSize = 1024;
const char* WinSockClientManager::SERVER_QUIT_COMMAND = "server:quit";
const char* WinSockClientManager::CLIENT_QUIT_COMMAND = "client:quit";

WinSockClientManager::WinSockClientManager():
	m_runSendThread(NULL),
	m_receiveThread(NULL),
	m_pWinSockClientListener(NULL)
{
	m_bufferMutex = CreateSemaphore(NULL, 1, 1, NULL);
}

WinSockClientManager::~WinSockClientManager()
{
	StopConnect();
}

BOOL WinSockClientManager::StartConnect(char* ipAddr, int port, int sockType)
{
	BOOL success = CreateSocket(sockType);
	if (!success) {
		return FALSE;
	}

	success = ConnectToServer(ipAddr, port);
	if (!success) {
		return FALSE;
	}

	LaunchSendRecvThread();
	return TRUE;
}

void WinSockClientManager::StopConnect()
{
	StopAllThreads();

	CloseHandle(m_bufferMutex);

	closesocket(m_sockClient);

	WSACleanup();
}

BOOL WinSockClientManager::CreateSocket(int sockType)
{
	WORD wVisionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	
	int err = WSAStartup(wVisionRequested, &wsaData);	// 请求2.2版本socket
	if (err != 0) {
		WSACleanup();
		return FALSE;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return FALSE;
	}

	m_sockClient = socket(AF_INET, sockType, 0);

	if (m_sockClient == INVALID_SOCKET) {
		WSACleanup();
		return FALSE;
	}

	return TRUE;
}

BOOL WinSockClientManager::ConnectToServer(char* ipAddr, int port) {
	SOCKADDR_IN addrSvr;
	addrSvr.sin_addr.S_un.S_addr = inet_addr(ipAddr);	// IP地址转换为点分十进制形式
	addrSvr.sin_family = AF_INET;
	addrSvr.sin_port = htons(port);

	int retVal = connect(m_sockClient, (SOCKADDR*)&addrSvr, sizeof(SOCKADDR));
	if (retVal == -1) {
		return FALSE;
	}

	return TRUE;
}

void WinSockClientManager::LaunchSendRecvThread()
{
	m_sendThread = CreateThread(NULL, 0, SendMessageThread, this, 0, NULL);
	m_receiveThread = CreateThread(NULL, 0, ReceiveMessageThread, this, 0, NULL);
}

void WinSockClientManager::StopAllThreads()
{
	m_runSendThread = FALSE;
	m_runReceiveThread = FALSE;
	if (m_sendThread != NULL) {
		WaitForSingleObject(m_sendThread, INFINITE);
		CloseHandle(m_sendThread);
		m_sendThread = NULL;
	}

	if (m_receiveThread != NULL) {
		CloseHandle(m_receiveThread);
		m_receiveThread = NULL;
	}
}

DWORD WinSockClientManager::SendMessageThread(LPVOID lpParameter)
{
	WinSockClientManager* pThis = (WinSockClientManager*)lpParameter;
	pThis->m_runSendThread = TRUE;
	while (pThis->m_runSendThread) {
		if (pThis->m_sendToServer) {
			WaitForSingleObject(pThis->m_bufferMutex, INFINITE);
			pThis->SendToServer();
			ReleaseSemaphore(pThis->m_bufferMutex, 1, NULL);
		}
	}

	return 0;
}

DWORD WinSockClientManager::ReceiveMessageThread(LPVOID lpParameter)
{
	WinSockClientManager* pThis = (WinSockClientManager*)lpParameter;
	pThis->m_runReceiveThread = TRUE;
	char recvBuf[kMaxRecvDataBufferSize];
	while (pThis->m_runReceiveThread)
	{
		memset(recvBuf, 0, kMaxRecvDataBufferSize);
		recv(pThis->m_sockClient, recvBuf, kMaxRecvDataBufferSize, 0);
		WaitForSingleObject(pThis->m_bufferMutex, INFINITE);

		BOOL serverIsDisconnected = !strcmp(SERVER_QUIT_COMMAND, recvBuf);
		if (serverIsDisconnected) {
			pThis->ServerDisconnected();
			break;
		}
		CString recvData(recvBuf);
		pThis->ReceiveMessage(recvData);

		ReleaseSemaphore(pThis->m_bufferMutex, 1, NULL);
	}

	return 0;
}

void WinSockClientManager::ServerDisconnected()
{
	StopConnect();
	if (m_pWinSockClientListener != NULL) {
		m_pWinSockClientListener->ServerHasDisconnected();
		m_pWinSockClientListener = NULL;
	}
}

void WinSockClientManager::SendToServer(std::string msg)
{
	m_sendMessage = msg;
	m_sendToServer = TRUE;
}

void WinSockClientManager::SetWinSockClientListener(WinSockClientListener * listener)
{
	m_pWinSockClientListener = listener;
}

void WinSockClientManager::SendToServer()
{
	if (m_sendToServer) {
		send(m_sockClient, m_sendMessage.c_str(), m_sendMessage.length(), 0);
	}
	m_sendToServer = FALSE;
}

void WinSockClientManager::ReceiveMessage(CString& recvData)
{
	if (recvData.GetLength() > 0 && m_pWinSockClientListener!= NULL) {
		CString serverIP = L"127.0.0.1";
		m_pWinSockClientListener->ServerMessage(serverIP, recvData);
	}
}
