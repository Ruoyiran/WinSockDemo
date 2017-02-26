
// WinSockClientDlg.h : ͷ�ļ�
//

#pragma once

#include "WinSockClientManager.h"

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
	void ConnectToServer();
	void StopConnect();
	void NotifyServerClientReadyForQuit();
	virtual void ServerHasDisconnected();

private:
	WinSockClientManager m_winSockClientManager;

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
