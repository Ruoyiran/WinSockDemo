
// WinSockServerDlg.h : ͷ�ļ�
//

#pragma once

#include "WinSockServerManager.h"
#include "afxwin.h"
#include <list>

// CWinSockServerDlg �Ի���
class CWinSockServerDlg : public CDialogEx
{
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
	struct ClientSocketData {
		SOCKET clientSocket;
		CString clientIP;
		ClientSocketData(SOCKET clientSocket, CString clientIP) {
			this->clientSocket = clientSocket;
			this->clientIP = clientIP;
		}
	};
	static CWinSockServerDlg* m_pDlgInstance;
	WinSockServerManager m_socketServerManager;
	CListBox m_clientList;
	std::list<ClientSocketData> m_clientSocketDataBuffer;

	int FindClientPositionInList(SOCKET clientSocket);
	void RemoveClientData(int index);
private:
	static void ClientJoined(SOCKET clientSocket, char* clientIP);
	static void ClientQuit(SOCKET clientSocket);

	void AddClientToList(SOCKET clientSocket, char* clientIP);
	void RemoveClientFromList(SOCKET clientSocket);

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
