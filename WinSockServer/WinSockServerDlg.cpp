
// WinSockServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WinSockServer.h"
#include "WinSockServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinSockServerDlg 对话框

CWinSockServerDlg* CWinSockServerDlg::m_pDlgInstance = NULL;

CWinSockServerDlg::CWinSockServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WINSOCKSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinSockServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDL_CLIENT_LIST, m_clientList);
}

BEGIN_MESSAGE_MAP(CWinSockServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_SERVER, &CWinSockServerDlg::OnBnClickedStartServer)
	ON_BN_CLICKED(IDB_SEND, &CWinSockServerDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// CWinSockServerDlg 消息处理程序

BOOL CWinSockServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_pDlgInstance = this;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWinSockServerDlg::OnPaint()
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
HCURSOR CWinSockServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWinSockServerDlg::OnBnClickedStartServer()
{
	BOOL isChecked = (SendDlgItemMessage(IDC_START_SERVER, BM_GETCHECK) == BST_CHECKED);
	if (isChecked) {
		StartServer(6000);
	}
	else {
		NotifyClientServerReadyForQuit();
		StopServer();
		m_clientList.ResetContent();
	}
}

void CWinSockServerDlg::OnBnClickedSend()
{
	m_socketServerManager.SendToClient("Hello, everyone!");
}

void CWinSockServerDlg::NotifyClientServerReadyForQuit()
{
	m_socketServerManager.SendToClient(WinSockServerManager::SERVER_QUIT_COMMAND);
	Sleep(300);
}

void CWinSockServerDlg::ClientJoined(SOCKET clientSocket, char* clientIP)
{
	if (m_pDlgInstance != NULL)
		m_pDlgInstance->AddClientToList(clientSocket, clientIP);
}

void CWinSockServerDlg::ClientQuit(SOCKET clientSocket)
{
	if (m_pDlgInstance != NULL) 
		m_pDlgInstance->RemoveClientFromList(clientSocket);
}

void CWinSockServerDlg::AddClientToList(SOCKET clientSocket, char* clientIP)
{
	CString ipAddr = CString(clientIP);
	m_clientList.AddString(ipAddr);
	m_clientSocketDataBuffer.push_back(ClientSocketData(clientSocket, ipAddr));
}

void CWinSockServerDlg::RemoveClientFromList(SOCKET clientSocket)
{
	int index = FindClientPositionInList(clientSocket);
	if (index >= 0) {
		m_clientList.DeleteString(index);
		RemoveClientData(index);
	}
}

int CWinSockServerDlg::FindClientPositionInList(SOCKET clientSocket)
{
	std::list<ClientSocketData>::const_iterator iter = m_clientSocketDataBuffer.begin();
	for (int index = 0; iter != m_clientSocketDataBuffer.end(); ++iter, ++index)
	{
		if (iter->clientSocket == clientSocket) {
			return index;
		}
	}

	return -1; // not found in the list
}

void CWinSockServerDlg::RemoveClientData(int targetPos)
{
	auto iter = m_clientSocketDataBuffer.begin();
	int index = 0;
	while (index != targetPos)
	{
		++iter;
		++index;
	}
	m_clientSocketDataBuffer.erase(iter);
}

void CWinSockServerDlg::StartServer(int port)
{
	BOOL success = m_socketServerManager.StartServer(SOCK_STREAM, port);
	if (success) {
		m_socketServerManager.SetWinSockServerListener(this);
		MessageBox(L"服务器已启动");
	}
	else {
		MessageBox(L"服务器启动失败");
		SendDlgItemMessage(IDC_START_SERVER, BM_SETCHECK, FALSE);
	}
}

void CWinSockServerDlg::StopServer()
{
	m_socketServerManager.StopServer();
	SendDlgItemMessage(IDC_START_SERVER, BM_SETCHECK, FALSE);
}
