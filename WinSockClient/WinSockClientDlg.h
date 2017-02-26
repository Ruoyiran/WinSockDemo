
// WinSockClientDlg.h : 头文件
//

#pragma once

#include "WinSockClientManager.h"
#include "afxwin.h"
#include <map>
#include <functional>

// CWinSockClientDlg 对话框
class CWinSockClientDlg : public CDialogEx, public WinSockClientListener
{
private:
	static CWinSockClientDlg* m_pDlgInstance;

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
	/* Override WinSockClientListener functions */
	virtual void ServerHasDisconnected();
	virtual void ServerMessage(CString serverIP, CString message);

	/* Server command callback functions */
	static void ServerLightOnCommand();
	static void ServerLightOffCommand();

	void InitUIComponents();
	void InitServerCommands();
	void ConnectToServer(char* ipAddr, int port);
	void ConnectSuccess();
	void StopConnect();
	void NotifyServerClientReadyForQuit();
	void AppendClientMessage(const CString & msg);
	CString GetFormatedString(CString &clientIP, CString &message);
	BOOL TryToIpAddrAndPortNumber(std::string& ipAddr, int& port);
	BOOL IsLightOn();
	BOOL IsServerCommandMessage(std::string& message);
	void LightOn();
	void LightOff();
	void SendLightBulbStateToServer();
	void ProcessServerCommand(std::string& cmd);

private:
	WinSockClientManager m_winSockClientManager;
	CEdit m_serverMessageEditText;
	CStatic m_lightBulbImg;
	HBITMAP m_bitmapLightOn;
	HBITMAP m_bitmapLightOff;
	std::map<std::string, std::function<void()>> m_serverCommands;

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
	afx_msg void OnBnClickedSendToServer();
	afx_msg void OnBnClickedLightBulbSwitch();
};
