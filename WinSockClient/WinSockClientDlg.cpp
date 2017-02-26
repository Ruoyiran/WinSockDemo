
// WinSockClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WinSockClient.h"
#include "WinSockClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinSockClientDlg �Ի���

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


// CWinSockClientDlg ��Ϣ�������

BOOL CWinSockClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
		MessageBox(L"���ӳɹ�");
	}
	else {
		MessageBox(L"�޷����ӵ�������");
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

	char* ansiData = UnicodeToAnsi(sendData);
	m_winSockClientManager.SendToServer(ansiData);
	SetDlgItemText(IDE_SEND, L"");
}
