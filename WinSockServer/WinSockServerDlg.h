
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

	void StopServer();
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

private:
	void StartServer(int port);

	void ClientJoined(SOCKET clientSocket, char* clientIP);
	void ClientQuit(SOCKET clientSocket);

	void AddClientToList(SOCKET clientSocket, char* clientIP);
	void RemoveClientFromList(SOCKET clientSocket);

	int FindClientPositionInList(SOCKET clientSocket);
	void RemoveClientData(int index);

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

};
