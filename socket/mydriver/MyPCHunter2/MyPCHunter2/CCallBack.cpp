// CCallBack.cpp: 实现文件
//

#include "pch.h"
#include "MyPCHunter2.h"
#include "CCallBack.h"
#include "afxdialogex.h"
#include "GDT.h"

// CCallBack 对话框

IMPLEMENT_DYNAMIC(CCallBack, CDialogEx)

CCallBack::CCallBack(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CCallBack, pParent)
{

}

CCallBack::~CCallBack()
{
}

void CCallBack::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LST_CALLBACK, m_lstCallback);
}


BEGIN_MESSAGE_MAP(CCallBack, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCallBack::OnBnClickedOk)
END_MESSAGE_MAP()


// CCallBack 消息处理程序


void CCallBack::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


BOOL CCallBack::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	OnInitList();
	GetCallBack();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


bool CCallBack::OnInitList()
{
	m_lstCallback.ModifyStyle(LVS_TYPEMASK, //清掉以前的风格 
		LVS_REPORT | LVS_SINGLESEL
		| LVS_EX_GRIDLINES); //设置为报表风格

	m_lstCallback.SetExtendedStyle(m_lstCallback.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int nItemIdx = 0;

	m_lstCallback.InsertColumn(nItemIdx++, "回调入口", LVCFMT_LEFT, 100);
	m_lstCallback.InsertColumn(nItemIdx++, "类型", LVCFMT_LEFT, 100);
	m_lstCallback.InsertColumn(nItemIdx++, "备注", LVCFMT_LEFT, 100);
	return true;
}



bool CCallBack::OpenDev(HANDLE& hDevice)
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


bool CCallBack::CloseDev(HANDLE hDevice)
{
	if (hDevice != INVALID_HANDLE_VALUE) {
		CloseHandle(hDevice);
	}
	return true;
}


bool CCallBack::GetCallBack()
{

	HANDLE hDevice = NULL;
	if (!OpenDev(hDevice))
	{
		ExitProcess(0);
	}
	DWORD dwBytes = 0;
	int nId = 0;
	int nC = 0;
	char* szBuf = new char[0x1000]{ 0 };
	int z = 0;
	
	if (!DeviceIoControl(hDevice, CTL_CALLBACK, &z, 4,
		szBuf, 0x1000, &dwBytes, NULL)) {
		CString Error;
		Error.Format("DeviceIoControl Error:%p\n", GetLastError());
		MessageBoxA(Error, "Error", 0);
		ExitProcess(0);
	}
	PMYCALLBACKLIST list = (PMYCALLBACKLIST)szBuf;
	for (int j = 0; j < list->dwNum; j++)
	{
		CString szTmp;
		szTmp.Format("%p", list->callback[j].dwCallBack);
		nC = m_lstCallback.InsertItem(nId++, szTmp);
		switch (list->callback[j].dwStyle)
		{
		case 1:
			szTmp = "BugCheckCallback";
			break;
		case 2:
			szTmp = "shutdown";
			break;
		default:
			szTmp = "";
		}
		m_lstCallback.SetItemText(nC, 1, szTmp);
		szTmp.Format("%p", list->callback[j].dwAddr);
		m_lstCallback.SetItemText(nC, 2, szTmp);
	}
	delete[] szBuf;
	CloseDev(hDevice);
	return TRUE;
}
