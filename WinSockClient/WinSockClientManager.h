#pragma once
#include <string>

class WinSockClientListener {
public:
	virtual void ServerHasDisconnected() = 0;
	virtual void ServerMessage(CString serverIP, CString message) = 0;
};

class WinSockClientManager {
public:
	const static char* SERVER_QUIT_COMMAND;
	const static char* CLIENT_QUIT_COMMAND;

public:
	WinSockClientManager();
	~WinSockClientManager();

	BOOL StartConnect(char* ipAddr, int port, int sockType = SOCK_STREAM);  // sockType: SOCK_STREAM or SOCK_DGRAM  分别对应TCP和UDP
	void StopConnect();
	void ServerDisconnected();
	void SendToServer(std::string msg);
	void SetWinSockClientListener(WinSockClientListener* listener);

protected:
	/* 线程函数 */
	static DWORD WINAPI SendMessageThread(LPVOID lpParameter);
	static DWORD WINAPI ReceiveMessageThread(LPVOID lpParameter);

	BOOL CreateSocket(int sockType);
	BOOL ConnectToServer(char * ipAddr, int port);
	void LaunchSendRecvThread();
	void StopAllThreads();
	void SendToServer();
	void ReceiveMessage(CString& recvData);

private:
	WinSockClientListener *m_pWinSockClientListener;
	SOCKET m_sockClient;

	HANDLE m_bufferMutex;		// 保证线程互斥成功并正常通信的信号量句柄
	HANDLE m_sendThread;
	HANDLE m_receiveThread;

	BOOL m_runSendThread;
	BOOL m_runReceiveThread;

	BOOL m_sendToServer;
	std::string m_sendMessage;
};