
// WinSockClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WinSockClient.h"
#include "WinSockClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinSockClientDlg 对话框

char* UnicodeToAnsi(const wchar_t* szStr) {
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	char *ansiText = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, ansiText, nLen, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	return ansiText;
}

CWinSockClientDlg::CWinSockClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WINSOCKCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinSockClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWinSockClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT_TO_SERVER, &CWinSockClientDlg::OnBnClickedConnectToServer)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDB_SEND_TO_SERVER, &CWinSockClientDlg::OnBnClickedSendToServer)
END_MESSAGE_MAP()


// CWinSockClientDlg 消息处理程序

BOOL CWinSockClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWinSockClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWinSockClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWinSockClientDlg::OnBnClickedConnectToServer()
{
	BOOL isChecked = (SendDlgItemMessage(IDC_CONNECT_TO_SERVER, BM_GETCHECK) == BST_CHECKED);
	if (isChecked) {
		ConnectToServer();
	}
	else
	{
		NotifyServerClientReadyForQuit();
		StopConnect();
	}
}

void CWinSockClientDlg::ConnectToServer()
{
	m_winSockClientManager.SetWinSockClientListener(this);

	BOOL success = m_winSockClientManager.StartConnect("127.0.0.1", 6000);
	if (success) {
		MessageBox(L"连接成功");
	}
	else {
		MessageBox(L"无法连接到服务器");
		SendDlgItemMessage(IDC_CONNECT_TO_SERVER, BM_SETCHECK, FALSE);
	}
}

void CWinSockClientDlg::ServerHasDisconnected()
{
	MessageBox(L"Server disconnected.");
	StopConnect();
}

void CWinSockClientDlg::StopConnect()
{
	m_winSockClientManager.StopConnect();
	SendDlgItemMessage(IDC_CONNECT_TO_SERVER, BM_SETCHECK, FALSE);
}

void CWinSockClientDlg::NotifyServerClientReadyForQuit()
{
	m_winSockClientManager.SendToServer(WinSockClientManager::CLIENT_QUIT_COMMAND);
	Sleep(300);
}

void CWinSockClientDlg::OnClose()
{
	BOOL isChecked = (SendDlgItemMessage(IDC_CONNECT_TO_SERVER, BM_GETCHECK) == BST_CHECKED);
	if (isChecked) {
		NotifyServerClientReadyForQuit();
		StopConnect();
	}

	__super::OnClose();
}


void CWinSockClientDlg::OnBnClickedSendToServer()
{
	BOOL isConnected = (SendDlgItemMessage(IDC_CONNECT_TO_SERVER, BM_GETCHECK) == BST_CHECKED);
	if (!isConnected) {
		MessageBox(L"未建立连接");
		return;
	}

	CString sendData;
	GetDlgItemText(IDE_SEND, sendData);
	if (sendData.GetLength() == 0)
	{
		MessageBox(L"发送文本不能不空");
		return;
	}

	char* ansiData = UnicodeToAnsi(sendData);
	m_winSockClientManager.SendToServer(ansiData);
	SetDlgItemText(IDE_SEND, L"");
}
