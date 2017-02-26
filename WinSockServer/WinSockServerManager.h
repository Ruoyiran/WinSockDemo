#pragma once
#include <vector>
#include <map>

class WinSockServerListener {
public:
	virtual void ClientJoined(SOCKET, char*) = 0;
	virtual void ClientQuit(SOCKET) = 0;
	virtual void ClientMessage(SOCKET, CString, CString) = 0;
};

class WinSockServerManager
{
public:
	WinSockServerManager();
	~WinSockServerManager();
	BOOL StartServer(int sockType, int port);  // sockType: SOCK_STREAM or SOCK_DGRAM  分别对应TCP和UDP
	void StopServer();
	void SendToClients(std::string msg);
	void SendToClient(SOCKET clientClient, std::string msg);
	void SetWinSockServerListener(WinSockServerListener* listener);

protected:
	/* 线程函数 */
	static DWORD WINAPI AcceptListeningThread(LPVOID lpParameter);
	static DWORD WINAPI SendMessageThread(LPVOID lpParameter);
	static DWORD WINAPI ReceiveMessageThread(LPVOID lpParameter);

	BOOL CreateSocket(int sockType);
	BOOL BindAndListenSocket(int port);
	void LaunchServer();
	void SendToAllClients();
	void SendToClient();
	void CloseClientConnection(SOCKET clientSocket);
	void StopAllThreads();
	void ClientJoined(SOCKET clientSocket, char* clientIP);
	void ClientQuit(SOCKET clientSocket);
	void ReceiveMessage(SOCKET clientSocket, CString &recvData);
	void RemoveClientSocketBuffer(SOCKET clientSocket);
	void AddClientSocketBuffer(SOCKET clientSocket, char* clientIP);

private:
	WinSockServerListener* m_pWinSockServerListener;
	HANDLE m_bufferMutex;		// 保证线程互斥成功并正常通信的信号量句柄
	HANDLE m_sendThread;
	HANDLE m_acceptThread;
	BOOL m_runAcceptThread;
	BOOL m_runSendThread;
	BOOL m_runReceiveThread;
	BOOL m_sendToAllClients;
	BOOL m_sendToClient;
	SOCKET m_targetClientSocket;
	SOCKET m_sockServer;
	std::vector<SOCKET> m_clientSocketGroup;
	std::map<SOCKET, CString> m_clientIpTable;
	std::string m_sendedMessage;
};
