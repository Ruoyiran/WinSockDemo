
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
		MessageBox(L"������������");
	}
	else {
		MessageBox(L"����������ʧ��");
		SendDlgItemMessage(IDC_START_SERVER, BM_SETCHECK, FALSE);
	}
}

void CWinSockServerDlg::StopServer()
{
	m_socketServerManager.StopServer();
	SendDlgItemMessage(IDC_START_SERVER, BM_SETCHECK, FALSE);
}
