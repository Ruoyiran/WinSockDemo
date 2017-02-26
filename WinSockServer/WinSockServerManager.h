#pragma once
#include <vector>

const int kMaxRecvDataBufferSize = 1024;

class WinSockServerManager
{
public:
	typedef void(*ClientJoinedCallback) (SOCKET, char*);
	typedef void(*ClientQuitCallback) (SOCKET);

public:
	WinSockServerManager();
	~WinSockServerManager();
	BOOL StartServer(int sockType, int port);
	void StopServer();
	void SendToClient(std::string msg);
	void SetClientJoinedCallback(ClientJoinedCallback callback);
	void SetClientQuitCallback(ClientQuitCallback callback);

protected:
	BOOL CreateSocket(int sockType); // type: SOCK_STREAM or SOCK_DGRAM  分别对应TCP和UDP
	BOOL BindAndListenSocket(int port);
	void LaunchServer();
	void SendToClient();

private:
	static DWORD WINAPI AcceptListeningThread(LPVOID lpParameter);
	static DWORD WINAPI ReceiveMessageThread(LPVOID lpParameter);
	static DWORD WINAPI SendMessageThread(LPVOID lpParameter);

	void CloseClientConnection(SOCKET clientSocket);
	void StopAllThreads();
	void ClientJoined(SOCKET clientSocket, char* clientIP);

	void ClientQuit(SOCKET clientSocket);
	void ClearAllCallbacks();

private:
	ClientJoinedCallback m_clientJoinedCallback;
	ClientQuitCallback m_clientQuitCallback;

private:
	HANDLE m_bufferMutex;		// 保证线程互斥成功并正常通信的信号量句柄
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