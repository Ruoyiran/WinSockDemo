
// WinSockServerDlg.h : 头文件
//

#pragma once

#include "WinSockServerManager.h"
#include "afxwin.h"
#include <list>


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

	void StopServer();
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
	WinSockServerManager m_socketServerManager;
	CListBox m_clientList;

private:
	void StartServer(int port);

	void ClientJoined(SOCKET clientSocket, char* clientIP);
	void ClientQuit(SOCKET clientSocket);

	void AddClientToList(SOCKET clientSocket, char* clientIP);
	void RemoveClientFromList(SOCKET clientSocket);

	int FindClientPositionInList(SOCKET clientSocket);
	void RemoveClientData(int index);

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

};
