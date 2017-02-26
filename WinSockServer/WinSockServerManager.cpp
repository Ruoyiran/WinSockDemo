#include "stdafx.h"
#include "WinSockServerManager.h"

const int kMaxRecvDataBufferSize = 1024;
const char* WinSockServerManager::SERVER_QUIT_COMMAND = "server:quit";
const char* WinSockServerManager::CLIENT_QUIT_COMMAND = "client:quit";

struct WinSockPackage {
	SOCKET clientSocket;
	WinSockServerManager* pSockServerInstance;
};

WinSockServerManager::WinSockServerManager() :
	m_runAcceptThread(FALSE),
	m_runSendThread(FALSE),
	m_runReceiveThread(FALSE),
	m_acceptThread(NULL),
	m_sendThread(NULL),
	m_pWinSockServerListener(NULL)
{
	m_bufferMutex = CreateSemaphore(NULL, 1, 1, NULL);
}

WinSockServerManager::~WinSockServerManager()
{
	StopServer();
}

BOOL WinSockServerManager::StartServer(int sockType, int port)
{
	BOOL success = CreateSocket(sockType);
	if (!success) {
		return FALSE;
	}

	success = BindAndListenSocket(port);
	if (!success) {
		return FALSE;
	}

	LaunchServer();
	return TRUE;
}

void WinSockServerManager::StopServer()
{
	StopAllThreads();

	CloseHandle(m_bufferMutex);

	WSACleanup();	// 终止对套接字库的使用

	m_pWinSockServerListener = NULL;
}

BOOL WinSockServerManager::CreateSocket(int type) {
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

	m_sockServer = socket(AF_INET, type, 0);

	if (m_sockServer == INVALID_SOCKET) {
		closesocket(m_sockServer);
		WSACleanup();
		return FALSE;
	}

	return TRUE;
}

BOOL WinSockServerManager::BindAndListenSocket(int port) {
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);	// 转换为网络字节序
	addrSrv.sin_port = htons(port);	// 转换为网络字节序

	int err = bind(m_sockServer, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	if (err == -1) {
		closesocket(m_sockServer);
		WSACleanup();
		return FALSE;
	}

	err = listen(m_sockServer, 20);
	if (err == -1) {
		closesocket(m_sockServer);
		WSACleanup();
		return FALSE;
	}

	return TRUE;
}

void WinSockServerManager::LaunchServer()
{
	m_acceptThread = CreateThread(NULL, 0, AcceptListeningThread, this, 0, NULL);
	m_sendThread = CreateThread(NULL, 0, SendMessageThread, this, 0, NULL);
}

void WinSockServerManager::StopAllThreads()
{
	m_runAcceptThread = FALSE;
	m_runSendThread = FALSE;
	m_runReceiveThread = FALSE;
	if (m_sendThread != NULL) {
		WaitForSingleObject(m_sendThread, INFINITE);
		CloseHandle(m_sendThread);
		m_sendThread = NULL;
	}

	if (m_acceptThread != NULL) {
		CloseHandle(m_acceptThread);
		m_acceptThread = NULL;
	}
}

DWORD WinSockServerManager::AcceptListeningThread(LPVOID lpParameter)
{
	WinSockServerManager *pThis = (WinSockServerManager*)lpParameter;
	ASSERT(pThis != NULL);

	pThis->m_runAcceptThread = TRUE;

	SOCKADDR_IN addrClient;

	while (pThis->m_runAcceptThread)
	{
		SOCKET socketClient = accept(pThis->m_sockServer, (SOCKADDR*)&addrClient, NULL);
		if (socketClient == SOCKET_ERROR) {
			continue;
		}
		char* clientIP = inet_ntoa(addrClient.sin_addr);
		pThis->ClientJoined(socketClient, clientIP);

		WinSockPackage package;
		package.clientSocket = socketClient;
		package.pSockServerInstance = pThis;

		HANDLE receiveThread = CreateThread(NULL, 0, ReceiveMessageThread, (LPVOID)&package, 0, NULL);

		WaitForSingleObject(pThis->m_bufferMutex, INFINITE);
		ReleaseSemaphore(pThis->m_bufferMutex, 1, NULL);

		CloseHandle(receiveThread);
	}
	return 0;
}

DWORD WinSockServerManager::ReceiveMessageThread(LPVOID lpParameter)
{
	WinSockPackage* package = (WinSockPackage*)lpParameter;
	WinSockServerManager* pThis = package->pSockServerInstance;
	ASSERT(pThis != NULL);

	SOCKET clientSocket = package->clientSocket;

	char recvBuf[kMaxRecvDataBufferSize];

	pThis->m_runReceiveThread = TRUE;
	while (pThis->m_runReceiveThread) {
		memset(recvBuf, 0, kMaxRecvDataBufferSize);
		recv(clientSocket, recvBuf, kMaxRecvDataBufferSize, 0);

		WaitForSingleObject(pThis->m_bufferMutex, INFINITE);

		BOOL isQuit = !strcmp(CLIENT_QUIT_COMMAND, recvBuf);
		if (isQuit) {
			pThis->ClientQuit(clientSocket);
			break;
		}

		CString recvData(recvBuf);
		if (recvData.GetLength()) {
			AfxMessageBox(L"Client: " + recvData);
		}
		ReleaseSemaphore(pThis->m_bufferMutex, 1, NULL);
	}
	return 0;
}

DWORD WINAPI WinSockServerManager::SendMessageThread(LPVOID lpParameter)
{
	WinSockServerManager *pThis = (WinSockServerManager*)lpParameter;
	ASSERT(pThis != NULL);

	pThis->m_runSendThread = TRUE;
	while (pThis->m_runSendThread) {
		if (pThis->m_sendToClient) {
			WaitForSingleObject(pThis->m_bufferMutex, INFINITE);
			pThis->SendToClient();
			ReleaseSemaphore(pThis->m_bufferMutex, 1, NULL);
		}
	}
	return 0;
}

void WinSockServerManager::SendToClient()
{
	for (int i = 0; i < m_clientSocketGroup.size(); ++i) {
		send(m_clientSocketGroup[i], m_sendedMessage.c_str(), m_sendedMessage.length(), 0);
	}
	m_sendToClient = FALSE;
}

void WinSockServerManager::SendToClient(std::string msg)
{
	m_sendToClient = TRUE;
	m_sendedMessage = msg;
}

void WinSockServerManager::SetWinSockServerListener(WinSockServerListener * listener)
{
	m_pWinSockServerListener = listener;
}

void WinSockServerManager::ClientJoined(SOCKET clientSocket, char* clientIP)
{
	m_clientSocketGroup.push_back(clientSocket);
	if (m_pWinSockServerListener != NULL) {
		m_pWinSockServerListener->ClientJoined(clientSocket, clientIP);
	}
}

void WinSockServerManager::ClientQuit(SOCKET clientSocket)
{
	CloseClientConnection(clientSocket);
	if (m_pWinSockServerListener != NULL) {
		m_pWinSockServerListener->ClientQuit(clientSocket);
	}
}

void WinSockServerManager::CloseClientConnection(SOCKET clientSocket)
{
	std::vector<SOCKET>::iterator result = find(m_clientSocketGroup.begin(), m_clientSocketGroup.end(), clientSocket);
	m_clientSocketGroup.erase(result);
	closesocket(clientSocket);
	ReleaseSemaphore(m_bufferMutex, 1, NULL);
}
