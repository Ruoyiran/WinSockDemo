
// WinSockServerDlg.h : ͷ�ļ�
//

#pragma once

#include "WinSockServerManager.h"
#include "afxwin.h"
#include <list>


// CWinSockServerDlg �Ի���
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

	// ����
public:
	CWinSockServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINSOCKSERVER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	static CWinSockServerDlg* m_pDlgInstance;

	std::list<ClientSocketData> m_clientSocketDataBuffer;
	WinSockServerManager m_socketServerManager;
	CListBox m_clientList;
	CEdit m_clientMessageEditText;

protected:
	/* Override WinSockServerListener functions */
	void ClientJoined(SOCKET clientSocket, char* clientIP);
	void ClientQuit(SOCKET clientSocket);
	void ClientMessage(CString clientIP, CString message);

private:
	void StartServer(int port);
	void StopServer();
	void AddClientToList(SOCKET clientSocket, char* clientIP);
	void RemoveClientFromList(SOCKET clientSocket);
	void RemoveClientData(int index);
	int FindClientPositionInList(SOCKET clientSocket);
	void NotifyClientServerReadyForQuit();
	void AppendClientMessage(const CString & msg);
	CString GetFormatedString(CString &clientIP, CString &message);
	CString GetHostIpAddr();
	int GetPortNumber();

	// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartServer();
	afx_msg void OnBnClickedSend();

	afx_msg void OnClose();
};
