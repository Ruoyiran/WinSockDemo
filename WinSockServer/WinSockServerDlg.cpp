
// WinSockServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WinSockServer.h"
#include "WinSockServerDlg.h"
#include "afxdialogex.h"
#include "Command.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinSockServerDlg 对话框
CWinSockServerDlg* CWinSockServerDlg::m_pDlgInstance = NULL;

const CString kDefaultPortNumber = L"6000";

std::string UnicodeToAnsi(const wchar_t* szStr) {
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	char *ansiText = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, ansiText, nLen, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	std::string resultText(ansiText);
	delete ansiText;

	return resultText;
}

CString GetCurrentSystemTime() {
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString currentTime;
	currentTime.Format(L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
	return currentTime;
}

int CWinSockServerDlg::GetPortNumber()
{
	CString portText;
	GetDlgItemText(IDE_PORT_NUMBER, portText);
	if (portText.GetLength() == 0) {
		MessageBox(L"端口号不能为空");
		return -1;
	}

	std::string portStr = UnicodeToAnsi(portText);
	int port = atoi(portStr.c_str());
	return port;
}

CString CWinSockServerDlg::GetHostIpAddr()
{
	return L"127.0.0.1";
}

void CWinSockServerDlg::BulbStateIsOn()
{
	SetDlgItemText(IDE_CLIENT_LIGHT_BULB_STATE, L"亮");
	SetDlgItemText(IDB_LIGHT_SWITCH, L"关灯");
	GetDlgItem(IDB_LIGHT_SWITCH)->EnableWindow(TRUE);
}

void CWinSockServerDlg::BulbStateIsOff()
{
	SetDlgItemText(IDE_CLIENT_LIGHT_BULB_STATE, L"暗");
	SetDlgItemText(IDB_LIGHT_SWITCH, L"开灯");
	GetDlgItem(IDB_LIGHT_SWITCH)->EnableWindow(TRUE);
}

CWinSockServerDlg::CWinSockServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WINSOCKSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	InitClientCommands();
}

void CWinSockServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDL_CLIENT_LIST, m_clientList);
	DDX_Control(pDX, IDE_CLIENT_MESSAGE, m_clientMessageEditText);
}

BEGIN_MESSAGE_MAP(CWinSockServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_SERVER, &CWinSockServerDlg::OnBnClickedStartServer)
	ON_BN_CLICKED(IDB_SEND, &CWinSockServerDlg::OnBnClickedSend)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDB_LIGHT_SWITCH, &CWinSockServerDlg::OnBnClickedLightSwitch)
	ON_LBN_SELCHANGE(IDL_CLIENT_LIST, &CWinSockServerDlg::OnClientListSelectChanged)
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
	InitUIComponents();
	
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

void CWinSockServerDlg::InitUIComponents()
{
	CString hostIP = GetHostIpAddr();
	SetDlgItemText(IDC_HOST_IP_ADDR, hostIP);
	SetDlgItemText(IDE_PORT_NUMBER, kDefaultPortNumber);
	SendDlgItemMessage(IDC_SEND_TO_ALL_CLIENTS, BM_SETCHECK, TRUE);
	DisableBulbState();
}

void CWinSockServerDlg::InitClientCommands()
{
	m_clientCommands[CLIENT_LIGHT_ON_COMMAND] = CWinSockServerDlg::ClientLightOnCommand;
	m_clientCommands[CLIENT_LIGHT_OFF_COMMAND] = CWinSockServerDlg::ClientLightOffCommand;
}

void CWinSockServerDlg::ClientLightOnCommand(SOCKET clientSocket) {
	m_pDlgInstance->m_clientSwitchingUnitData[clientSocket].SetBulbState(TRUE);

	BOOL isSelectedClient = m_pDlgInstance->CheckIsSelectedClient(clientSocket);
	if (isSelectedClient)
		m_pDlgInstance->BulbStateIsOn();
}

void CWinSockServerDlg::ClientLightOffCommand(SOCKET clientSocket) {
	m_pDlgInstance->m_clientSwitchingUnitData[clientSocket].SetBulbState(FALSE);

	BOOL isSelectedClient = m_pDlgInstance->CheckIsSelectedClient(clientSocket);
	if (isSelectedClient)
		m_pDlgInstance->BulbStateIsOff();
}

void CWinSockServerDlg::OnBnClickedStartServer()
{
	BOOL isChecked = (SendDlgItemMessage(IDC_START_SERVER, BM_GETCHECK) == BST_CHECKED);
	if (isChecked) {
		int port = GetPortNumber();
		if (port < 0) {
			SendDlgItemMessage(IDC_START_SERVER, BM_SETCHECK, BST_UNCHECKED);
			return;
		}
		StartServer(port);
	}
	else {
		StopServer();
		m_clientList.ResetContent();
		OnClientListSelectChanged();
	}
}

void CWinSockServerDlg::OnBnClickedSend()
{
	BOOL isConnected = (SendDlgItemMessage(IDC_START_SERVER, BM_GETCHECK) == BST_CHECKED) && m_clientList.GetCount() > 0;
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

	BOOL sendToAll = SendDlgItemMessage(IDC_SEND_TO_ALL_CLIENTS, BM_GETCHECK) == BST_CHECKED;
	if (sendToAll) {
		SendToAllClients(sendData);
	}
	else {
		SendToOneClient(sendData);
	}
	SetDlgItemText(IDE_SEND, L"");
}

void CWinSockServerDlg::NotifyClientServerReadyForQuit()
{
	m_socketServerManager.SendToClients(SERVER_QUIT_COMMAND);
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

void CWinSockServerDlg::ClientMessage(SOCKET clientSocket, CString clientIP, CString message)
{
	std::string ansiStr = UnicodeToAnsi(message);
	BOOL isCommandMsg = IsClientCommandMessage(ansiStr);
	if (!isCommandMsg) {
		CString formatedMessage = GetFormatedString(clientIP, message);
		AppendClientMessage(formatedMessage);
	}
	else {
		ProcessClientCommand(clientSocket, ansiStr.c_str());
	}
}

BOOL CWinSockServerDlg::IsClientCommandMessage(std::string& message)
{
	return m_clientCommands.find(message) != m_clientCommands.end();
}

void CWinSockServerDlg::ProcessClientCommand(SOCKET clientSocket, std::string cmd)
{
	if (m_clientSwitchingUnitData.find(clientSocket) == m_clientSwitchingUnitData.end())
		m_clientSwitchingUnitData[clientSocket] = ClientSwitchingUnit();
	m_clientCommands[cmd](clientSocket);
}

CString CWinSockServerDlg::GetFormatedString(CString &clientIP, CString &message)
{
	CString currentTime = GetCurrentSystemTime();
	CString formatedMessage;
	formatedMessage.Append(L"【");
	formatedMessage.Append(L" ");
	formatedMessage.Append(clientIP);
	formatedMessage.Append(L"】");

	formatedMessage.Append(L"\r\n");

	formatedMessage.Append(L"[");
	formatedMessage.Append(currentTime);
	formatedMessage.Append(L"] ");
	formatedMessage.Append(message);
	return formatedMessage;
}

void CWinSockServerDlg::AppendClientMessage(const CString &msg) {
	CString curText;
	GetDlgItemText(IDE_CLIENT_MESSAGE, curText);
	if (curText.GetLength() > 0)
	{
		curText.Append(L"\r\n\r\n");
	}

	curText.Append(msg);

	m_clientMessageEditText.SetWindowTextW(curText);
	m_clientMessageEditText.LineScroll(m_clientMessageEditText.GetLineCount() - 1, 0);
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
		OnClientListSelectChanged();
	}

	auto iter = m_clientSwitchingUnitData.find(clientSocket);
	if (iter != m_clientSwitchingUnitData.end())
		m_clientSwitchingUnitData.erase(iter);
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

SOCKET CWinSockServerDlg::GetClientSocket(int position) {
	if (position < 0) return INVALID_SOCKET;

	std::list<ClientSocketData>::const_iterator iter = m_clientSocketDataBuffer.begin();
	int i = 0;
	while (i < position)
	{
		++i;
		++iter;
	}
	return iter->clientSocket;
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
	if (m_clientList.GetCount() > 0) {
		NotifyClientServerReadyForQuit();
	}
	m_socketServerManager.StopServer();
}

void CWinSockServerDlg::OnClose()
{
	StopServer();

	__super::OnClose();
}

void CWinSockServerDlg::OnBnClickedLightSwitch()
{
	CString bulbState;
	GetDlgItemText(IDE_CLIENT_LIGHT_BULB_STATE, bulbState);
	if (bulbState == L"亮")
		SendToOneClient(CString(CLIENT_LIGHT_OFF_COMMAND));
	else if (bulbState == L"暗")
		SendToOneClient(CString(CLIENT_LIGHT_ON_COMMAND));
	Sleep(100);
}

void CWinSockServerDlg::OnClientListSelectChanged()
{
	int index = m_clientList.GetCurSel();
	if (index == LB_ERR) {
		DisableBulbState();
		return;
	}

	SOCKET clientSocket = GetClientSocket(index);
	BOOL bulbIsOn = m_clientSwitchingUnitData[clientSocket].GetBulbState();
	if (bulbIsOn)
		BulbStateIsOn();
	else
		BulbStateIsOff();
}

void CWinSockServerDlg::DisableBulbState()
{
	SetDlgItemText(IDE_CLIENT_LIGHT_BULB_STATE, L"未知");
	GetDlgItem(IDB_LIGHT_SWITCH)->EnableWindow(FALSE);
}

void CWinSockServerDlg::SendToAllClients(CString & msg)
{
	std::string ansiData = UnicodeToAnsi(msg);
	m_socketServerManager.SendToClients(ansiData);
}

void CWinSockServerDlg::SendToOneClient(CString sendData)
{
	int index = m_clientList.GetCurSel();
	if (index == LB_ERR) {
		MessageBox(L"未选中客户端");
		return;
	}

	SOCKET clientSocket = GetClientSocket(index);
	std::string ansiData = UnicodeToAnsi(sendData);
	m_socketServerManager.SendToClient(clientSocket, ansiData);
}

BOOL CWinSockServerDlg::CheckIsSelectedClient(SOCKET clientSocket)
{
	int selIndex = m_clientList.GetCurSel();
	SOCKET selectedClientSocket = GetClientSocket(selIndex);
	return clientSocket == selectedClientSocket;
}
