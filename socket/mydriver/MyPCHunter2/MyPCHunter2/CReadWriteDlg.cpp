// CReadWriteDlg.cpp: 实现文件
//

#include "pch.h"
#include "MyPCHunter2.h"
#include "CReadWriteDlg.h"
#include "afxdialogex.h"
#include "GDT.h"

// CReadWriteDlg 对话框

IMPLEMENT_DYNAMIC(CReadWriteDlg, CDialogEx)

CReadWriteDlg::CReadWriteDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CReadWriteDlg, pParent)
{

}

CReadWriteDlg::~CReadWriteDlg()
{
}

void CReadWriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CReadWriteDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CReadWriteDlg::OnBnClickedOk)
	ON_BN_CLICKED(BTN_READ, &CReadWriteDlg::OnBnClickedRead)
	ON_BN_CLICKED(BTN_WRITE, &CReadWriteDlg::OnBnClickedWrite)
	ON_COMMAND(IDOK, &CReadWriteDlg::OnIdok)
END_MESSAGE_MAP()


// CReadWriteDlg 消息处理程序


BOOL CReadWriteDlg::OpenDev(HANDLE& hDevice)
{
	BOOL bRet = TRUE;
	//打开设备
	hDevice = CreateFileA("\\\\?\\cr36zh",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE) {
		CString Error;
		Error.Format("CreateFile Error:%p\n", GetLastError());
		MessageBoxA(Error, "Error", 0);
		bRet = FALSE;
	}

	return bRet;
}


BOOL CReadWriteDlg::CloseDev(HANDLE& hDevice)
{
	if (hDevice != INVALID_HANDLE_VALUE) {
		CloseHandle(hDevice);
	}
	return true;
}


void CReadWriteDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


BOOL CReadWriteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}





void CReadWriteDlg::OnBnClickedRead()
{
	BOOL bRet = TRUE;
	HANDLE hDevice = NULL;
	if (!OpenDev(hDevice))
	{
		ExitProcess(0);
	}
	DWORD dwBytes = 0;
	char* szBuf = new char[0x1000]{ 0 };

	CString szAddr;
	GetDlgItemTextA(EDT_ADDR, szAddr);
	MYREADWRITE rw = { 0 };
	rw.dwPid = m_dwPid;
	rw.dwAddr = strtol(szAddr, NULL, 16);
	rw.dwSize = 4;
	if (!DeviceIoControl(hDevice, CTL_READPRESS, &rw, sizeof(MYREADWRITE),
		szBuf, 4, &dwBytes, NULL)) {
		CString Error;
		Error.Format("DeviceIoControl Error:%p\n", GetLastError());
		MessageBoxA(Error, "Error", 0);
		ExitProcess(0);
	}
	CString szShow;
	szShow.Format("%p", *(DWORD *)szBuf);
	SetDlgItemTextA(EDT_SHOW, szShow);
	
	delete[] szBuf;
	CloseDev(hDevice);
}


void CReadWriteDlg::OnBnClickedWrite()
{
	BOOL bRet = TRUE;
	HANDLE hDevice = NULL;
	if (!OpenDev(hDevice))
	{
		ExitProcess(0);
	}
	DWORD dwBytes = 0;
	int nId = 0;
	int nC = 0;
	CString szAddr;
	GetDlgItemTextA(EDT_ADDR, szAddr);
	MYREADWRITE rw = { 0 };
	rw.dwPid = m_dwPid;
	rw.dwAddr = strtol(szAddr, NULL, 16);
	rw.dwSize = 4;
	CString szShow;
	GetDlgItemTextA(EDT_SHOW, szShow);
	rw.dwData = strtol(szShow, NULL, 16);
	if (!DeviceIoControl(hDevice, CTL_WRITEPRESS, &rw, sizeof(MYREADWRITE),
		&rw, 4, &dwBytes, NULL)) {
		CString Error;
		Error.Format("DeviceIoControl Error:%p\n", GetLastError());
		MessageBoxA(Error, "Error", 0);
		ExitProcess(0);
	}
	
	CloseDev(hDevice);
}


void CReadWriteDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}
