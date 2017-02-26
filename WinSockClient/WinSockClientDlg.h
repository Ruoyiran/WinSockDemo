
// WinSockClientDlg.h : ͷ�ļ�
//

#pragma once

#include "WinSockClientManager.h"
#include "afxwin.h"

// CWinSockClientDlg �Ի���
class CWinSockClientDlg : public CDialogEx, public WinSockClientListener
{
// ����
public:
	CWinSockClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINSOCKCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	virtual void ServerHasDisconnected();
	virtual void ServerMessage(CString serverIP, CString message);

	void ConnectToServer(char* ipAddr, int port);
	void StopConnect();
	void NotifyServerClientReadyForQuit();
	void AppendClientMessage(const CString & msg);
	CString GetFormatedString(CString &clientIP, CString &message);
	BOOL TryToIpAddrAndPortNumber(std::string& ipAddr, int& port);

private:
	WinSockClientManager m_winSockClientManager;
	CEdit m_serverMessageEditText;

protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConnectToServer();
	afx_msg void OnClose();
	afx_msg void OnBnClickedSendToServer();
};
