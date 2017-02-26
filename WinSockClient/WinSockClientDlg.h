
// WinSockClientDlg.h : 头文件
//

#pragma once

#include "WinSockClientManager.h"

// CWinSockClientDlg 对话框
class CWinSockClientDlg : public CDialogEx, public WinSockClientListener
{
// 构造
public:
	CWinSockClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINSOCKCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	void ConnectToServer();
	void StopConnect();
	void NotifyServerClientReadyForQuit();
	virtual void ServerHasDisconnected();

private:
	WinSockClientManager m_winSockClientManager;

protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConnectToServer();
	afx_msg void OnClose();
};
