
// WinSockServerDlg.h : 头文件
//

#pragma once

#include "afxwin.h"
#include <list>
#include <map>
#include <set>
#include <functional>
#include "WinSockServerManager.h"
#include "ClientSwitchingUnit.h"

// CWinSockServerDlg 对话框
class CWinSockServerDlg : public CDialogEx, public WinSockServerListener
{
private:
	struct ClientSocketData {
		SOCKET clientSocket;
		CString clientIP;
		ClientSocketData(SOCKET clientSocket, CString clientIP) {
			this->clientSocket = clientSocket;
			this->clientIP = clientIP;
		}
	};

	// 构造
public:
	CWinSockServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINSOCKSERVER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	static CWinSockServerDlg* m_pDlgInstance;

	std::list<ClientSocketData> m_clientSocketDataBuffer;
	std::map<SOCKET, ClientSwitchingUnit> m_clientSwitchingUnitData;
	std::map<std::string, std::function<void (SOCKET)>> m_clientCommands;
	WinSockServerManager m_socketServerManager;
	CListBox m_clientList;
	CEdit m_clientMessageEditText;

protected:
	/* Override WinSockServerListener functions */
	void ClientJoined(SOCKET clientSocket, char* clientIP);
	void ClientQuit(SOCKET clientSocket);
	void ClientMessage(SOCKET clientSocket, CString clientIP, CString message);

	/* Client command callback functions */
	static void ClientLightOnCommand(SOCKET clientSocket);
	static void ClientLightOffCommand(SOCKET clientSocket);

private:
	void StartServer(int port);
	void StopServer();
	void InitUIComponents();
	void InitClientCommands();
	void AddClientToList(SOCKET clientSocket, char* clientIP);
	void RemoveClientFromList(SOCKET clientSocket);
	void RemoveClientData(int index);
	void NotifyClientServerReadyForQuit();
	void AppendClientMessage(const CString & msg);
	void ProcessClientCommand(SOCKET clientSocket, std::string cmd);
	void BulbStateIsOn();
	void BulbStateIsOff();
	void DisableBulbState();
	void SendToAllClients(CString& msg);
	void SendToOneClient(CString sendData);

	int FindClientPositionInList(SOCKET clientSocket);
	int GetPortNumber();
	BOOL IsClientCommandMessage(std::string& message);
	BOOL CheckIsSelectedClient(SOCKET clientSocket);
	SOCKET GetClientSocket(int position);
	CString GetFormatedString(CString &clientIP, CString &message);
	CString GetHostIpAddr();

	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartServer();
	afx_msg void OnBnClickedSend();

	afx_msg void OnClose();
	afx_msg void OnBnClickedLightSwitch();
	afx_msg void OnClientListSelectChanged();
};
