
// WinSockServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WinSockServer.h"
#include "WinSockServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinSockServerDlg �Ի���

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

CWinSockServerDlg* CWinSockServerDlg::m_pDlgInstance = NULL;

int CWinSockServerDlg::GetPortNumber()
{
	CString portText;
	GetDlgItemText(IDE_PORT_NUMBER, portText);
	if (portText.GetLength() == 0) {
		MessageBox(L"�˿ںŲ���Ϊ��");
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

CWinSockServerDlg::CWinSockServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WINSOCKSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
END_MESSAGE_MAP()


// CWinSockServerDlg ��Ϣ�������

BOOL CWinSockServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

									// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_pDlgInstance = this;
	SetDlgItemText(IDE_PORT_NUMBER, kDefaultPortNumber);
	CString hostIP = GetHostIpAddr();
	SetDlgItemText(IDC_HOST_IP_ADDR, hostIP);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWinSockServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CWinSockServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
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
	}
}

void CWinSockServerDlg::OnBnClickedSend()
{
	BOOL isConnected = (SendDlgItemMessage(IDC_START_SERVER, BM_GETCHECK) == BST_CHECKED) && m_clientList.GetCount() > 0;
	if (!isConnected) {
		MessageBox(L"δ��������");
		return;
	}

	CString sendData;
	GetDlgItemText(IDE_SEND, sendData);
	if (sendData.GetLength() == 0)
	{
		MessageBox(L"�����ı����ܲ���");
		return;
	}

	std::string ansiData = UnicodeToAnsi(sendData);
	m_socketServerManager.SendToClient(ansiData);
	SetDlgItemText(IDE_SEND, L"");
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

void CWinSockServerDlg::ClientMessage(CString clientIP, CString message)
{
	CString formatedMessage = GetFormatedString(clientIP, message);
	AppendClientMessage(formatedMessage);
}

CString CWinSockServerDlg::GetFormatedString(CString &clientIP, CString &message)
{
	CString currentTime = GetCurrentSystemTime();
	CString formatedMessage;
	formatedMessage.Append(L"��");
	formatedMessage.Append(L" ");
	formatedMessage.Append(clientIP);
	formatedMessage.Append(L"��");

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
		MessageBox(L"������������");
	}
	else {
		MessageBox(L"����������ʧ��");
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
