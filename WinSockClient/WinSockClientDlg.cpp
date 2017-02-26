
// WinSockClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WinSockClient.h"
#include "WinSockClientDlg.h"
#include "Command.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinSockClientDlg �Ի���
CWinSockClientDlg* CWinSockClientDlg::m_pDlgInstance = NULL;

const CString kDefaultPortNumber = L"6000";
const CString kDefaultIpAddr = L"127.0.0.1";
const BOOL kDefaultLightIsOn = TRUE;

std::string UnicodeToAnsi(const wchar_t* szStr) {
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	char *ansiText = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, ansiText, nLen, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	std::string result(ansiText);
	delete ansiText;

	return result;
}

CString GetCurrentSystemTime() {
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString currentTime;
	currentTime.Format(L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
	return currentTime;
}

CWinSockClientDlg::CWinSockClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WINSOCKCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinSockClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDE_SERVER_MESSAGE, m_serverMessageEditText);
	DDX_Control(pDX, IDB_LIGHT_ON, m_lightBulbImg);
}

BEGIN_MESSAGE_MAP(CWinSockClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT_TO_SERVER, &CWinSockClientDlg::OnBnClickedConnectToServer)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDB_SEND_TO_SERVER, &CWinSockClientDlg::OnBnClickedSendToServer)
	ON_BN_CLICKED(IDB_LIGHT_BULB_SWITCH, &CWinSockClientDlg::OnBnClickedLightBulbSwitch)
END_MESSAGE_MAP()


// CWinSockClientDlg ��Ϣ�������

BOOL CWinSockClientDlg::TryToIpAddrAndPortNumber(std::string& ipAddr, int& port)
{
	ipAddr = "";
	port = -1;

	CString ipText;
	GetDlgItemText(IDC_SERVER_IP_ADDR, ipText);
	if (ipText.GetLength() == 0)
		return FALSE;

	ipAddr = UnicodeToAnsi(ipText);

	CString portText;
	GetDlgItemText(IDE_PORT_NUMBER, portText);
	if (portText.GetLength() == 0) {
		return FALSE;
	}

	std::string portStr = UnicodeToAnsi(portText);
	port = atoi(portStr.c_str());

	return TRUE;
}

BOOL CWinSockClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_pDlgInstance = this;
	InitUIComponents();
	InitServerCommands();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CWinSockClientDlg::InitUIComponents()
{
	SetDlgItemText(IDE_PORT_NUMBER, kDefaultPortNumber);
	SetDlgItemText(IDC_SERVER_IP_ADDR, kDefaultIpAddr);

	m_bitmapLightOn = (HBITMAP)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_LIGHT_ON), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
	m_bitmapLightOff = (HBITMAP)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_LIGHT_OFF), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

	if (kDefaultLightIsOn == TRUE) {
		m_lightBulbImg.SetBitmap(m_bitmapLightOn);
		SetDlgItemText(IDB_LIGHT_BULB_SWITCH, L"�ص�");
	}
}

void CWinSockClientDlg::InitServerCommands()
{
	m_serverCommands[CLIENT_LIGHT_ON_COMMAND] = CWinSockClientDlg::ServerLightOnCommand;
	m_serverCommands[CLIENT_LIGHT_OFF_COMMAND] = CWinSockClientDlg::ServerLightOffCommand;
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWinSockClientDlg::OnPaint()
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
HCURSOR CWinSockClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWinSockClientDlg::OnBnClickedConnectToServer()
{
	BOOL isChecked = (SendDlgItemMessage(IDC_CONNECT_TO_SERVER, BM_GETCHECK) == BST_CHECKED);
	if (isChecked) {
		std::string ipAddr;
		int port;
		BOOL isValid = TryToIpAddrAndPortNumber(ipAddr, port);
		if (isValid) {
			ConnectToServer(const_cast<char*>(ipAddr.c_str()), port);
		}
		else {
			MessageBox(L"��������Ч��IP��ַ�Ͷ˿ں�");
		}
	}
	else
	{
		NotifyServerClientReadyForQuit();
		StopConnect();
	}
}

void CWinSockClientDlg::ConnectToServer(char* ipAddr, int port)
{
	m_winSockClientManager.SetWinSockClientListener(this);

	BOOL success = m_winSockClientManager.StartConnect(ipAddr, port);
	if (success) {
		ConnectSuccess();
		MessageBox(L"���ӳɹ�");
	}
	else {
		MessageBox(L"�޷����ӵ�������");
		SendDlgItemMessage(IDC_CONNECT_TO_SERVER, BM_SETCHECK, FALSE);
	}
}

void CWinSockClientDlg::ConnectSuccess()
{
	SendLightBulbStateToServer();
}

void CWinSockClientDlg::SendLightBulbStateToServer()
{
	BOOL lightIsOn = IsLightOn();
	if (lightIsOn)
		m_winSockClientManager.SendToServer(CLIENT_LIGHT_ON_COMMAND);
	else
		m_winSockClientManager.SendToServer(CLIENT_LIGHT_OFF_COMMAND);
}

BOOL CWinSockClientDlg::IsLightOn()
{
	CString lightState;
	GetDlgItemText(IDB_LIGHT_BULB_SWITCH, lightState);

	return lightState == L"�ص�";
}

void CWinSockClientDlg::ServerHasDisconnected()
{
	StopConnect();
	MessageBox(L"�������ѶϿ�����");
}

void CWinSockClientDlg::ServerMessage(CString serverIP, CString message)
{
	std::string ansiStr = UnicodeToAnsi(message);
	BOOL isCommandMsg = IsServerCommandMessage(ansiStr);
	if (!isCommandMsg) {
		CString formatedMessage = GetFormatedString(serverIP, message);
		AppendClientMessage(formatedMessage);
	}
	else {
		ProcessServerCommand(ansiStr);
	}
}

void CWinSockClientDlg::ServerLightOnCommand()
{
	m_pDlgInstance->LightOn();
}

void CWinSockClientDlg::ServerLightOffCommand()
{
	m_pDlgInstance->LightOff();
}

BOOL CWinSockClientDlg::IsServerCommandMessage(std::string& message)
{
	return m_serverCommands.find(message) != m_serverCommands.end();
}

CString CWinSockClientDlg::GetFormatedString(CString &clientIP, CString &message)
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

void CWinSockClientDlg::AppendClientMessage(const CString &msg) {
	CString curText;
	GetDlgItemText(IDE_SERVER_MESSAGE, curText);
	if (curText.GetLength() > 0)
	{
		curText.Append(L"\r\n\r\n");
	}

	curText.Append(msg);

	m_serverMessageEditText.SetWindowTextW(curText);
	m_serverMessageEditText.LineScroll(m_serverMessageEditText.GetLineCount() - 1, 0);
}

void CWinSockClientDlg::ProcessServerCommand(std::string& cmd)
{
	m_serverCommands[cmd]();
}

void CWinSockClientDlg::StopConnect()
{
	m_winSockClientManager.StopConnect();
	SendDlgItemMessage(IDC_CONNECT_TO_SERVER, BM_SETCHECK, FALSE);
}

void CWinSockClientDlg::NotifyServerClientReadyForQuit()
{
	m_winSockClientManager.SendToServer(CLIENT_QUIT_COMMAND);
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
	m_winSockClientManager.SendToServer(ansiData);
	SetDlgItemText(IDE_SEND, L"");
}

void CWinSockClientDlg::OnBnClickedLightBulbSwitch()
{
	BOOL lightIsOn = IsLightOn();
	if (lightIsOn) {
		LightOff();
	}
	else {
		LightOn();
	}
	Sleep(100);
}

void CWinSockClientDlg::LightOn() {
	m_lightBulbImg.SetBitmap(m_bitmapLightOn);
	SetDlgItemText(IDB_LIGHT_BULB_SWITCH, L"�ص�");

	BOOL isConnected = (SendDlgItemMessage(IDC_CONNECT_TO_SERVER, BM_GETCHECK) == BST_CHECKED);
	if (isConnected) {
		SendLightBulbStateToServer();
		Sleep(300);
	}
}

void CWinSockClientDlg::LightOff() {
	m_lightBulbImg.SetBitmap(m_bitmapLightOff);
	SetDlgItemText(IDB_LIGHT_BULB_SWITCH, L"����");

	BOOL isConnected = (SendDlgItemMessage(IDC_CONNECT_TO_SERVER, BM_GETCHECK) == BST_CHECKED);
	if (isConnected) {
		SendLightBulbStateToServer();
		Sleep(300);
	}
}
