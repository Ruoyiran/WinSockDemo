#pragma once
#include <vector>

class WinSockServerListener {
public:
	virtual void ClientJoined(SOCKET, char*) = 0;
	virtual void ClientQuit(SOCKET) = 0;
};

class WinSockServerManager
{
public:
	const static char* SERVER_QUIT_COMMAND;
	const static char* CLIENT_QUIT_COMMAND;

public:
	WinSockServerManager();
	~WinSockServerManager();
	BOOL StartServer(int sockType, int port);  // sockType: SOCK_STREAM or SOCK_DGRAM  �ֱ��ӦTCP��UDP
	void StopServer();
	void SendToClient(std::string msg);
	void SetWinSockServerListener(WinSockServerListener* listener);

protected:
	/* �̺߳��� */
	static DWORD WINAPI AcceptListeningThread(LPVOID lpParameter);
	static DWORD WINAPI SendMessageThread(LPVOID lpParameter);
	static DWORD WINAPI ReceiveMessageThread(LPVOID lpParameter);

	BOOL CreateSocket(int sockType);
	BOOL BindAndListenSocket(int port);
	void LaunchServer();
	void SendToClient();
	void CloseClientConnection(SOCKET clientSocket);
	void StopAllThreads();
	void ClientJoined(SOCKET clientSocket, char* clientIP);
	void ClientQuit(SOCKET clientSocket);

private:
	WinSockServerListener* m_pWinSockServerListener;
	HANDLE m_bufferMutex;		// ��֤�̻߳���ɹ�������ͨ�ŵ��ź������
	HANDLE m_sendThread;
	HANDLE m_acceptThread;
	BOOL m_runAcceptThread;
	BOOL m_runSendThread;
	BOOL m_runReceiveThread;
	BOOL m_sendToClient;
	SOCKET m_sockServer;
	std::vector<SOCKET> m_clientSocketGroup;
	std::string m_sendedMessage;
};
