// CIdtDlg.cpp: 实现文件
//

#include "pch.h"
#include "MyPCHunter2.h"
#include "CIdtDlg.h"
#include "afxdialogex.h"
#include "GDT.h"

// CIdtDlg 对话框

IMPLEMENT_DYNAMIC(CIdtDlg, CDialogEx)

CIdtDlg::CIdtDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CIdtDlg, pParent)
{

}

CIdtDlg::~CIdtDlg()
{
}

void CIdtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LST_IDT, m_lstIdt);
}


BEGIN_MESSAGE_MAP(CIdtDlg, CDialogEx)
END_MESSAGE_MAP()


// CIdtDlg 消息处理程序


bool CIdtDlg::GetIdt()
{
	char idt[6];
	DWORD dwBytes;

	//解析gdt
	PMYIDT pIdt = NULL;
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	HANDLE hDevice = NULL;
	if (!OpenDev(hDevice))
	{
		ExitProcess(0);
	}
	//printf("dwNumberOfProcess= %d\n", sysinfo.dwNumberOfProcessors);
	int shift = 1;
	int nC = 0;
	int nId = 0;
	for (DWORD i = 0; i < sysinfo.dwNumberOfProcessors; i++)
	{
		SetProcessAffinityMask(GetCurrentProcess(), shift);
		__asm {
			sidt idt
		}
		char OutBuf[0x7ff] = { 0 };

		if (!DeviceIoControl(hDevice, CTL_GOT, (idt+2), sizeof(DWORD),
			OutBuf, sizeof(OutBuf), &dwBytes, NULL)) {
			CString Error;
			Error.Format("DeviceIoControl Error:%p\n", GetLastError());
			MessageBoxA(Error, "Error", 0);
			ExitProcess(0);
		}
		for (int j = 0; j < 0xff; j++)
		{
			pIdt = (PMYIDT)(OutBuf + j * 8);
			CString szTmp;
			szTmp.Format("%d", i); //cpu序号
			nC = m_lstIdt.InsertItem(nId++, szTmp);

			szTmp.Format("%x", j); //序号
			m_lstIdt.SetItemText(nC, 1, szTmp);
			
			szTmp.Format("%x", pIdt->type);
			m_lstIdt.SetItemText(nC, 2, szTmp);
			
			szTmp.Format("%p", GetIdtAddr(pIdt));
			m_lstIdt.SetItemText(nC, 3, szTmp);

			

		}
		shift <<= 1;
	}
	CloseDev(hDevice);
	return true;
}


bool CIdtDlg::OpenDev(HANDLE& hDevice)
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


bool CIdtDlg::CloseDev(HANDLE hDevice)
{
	if (hDevice != INVALID_HANDLE_VALUE) {
		CloseHandle(hDevice);
	}
	return true;
}


bool CIdtDlg::OnInitList()
{
	// TODO: 在此处添加实现代码.
	m_lstIdt.ModifyStyle(LVS_TYPEMASK, //清掉以前的风格 
		LVS_REPORT | LVS_SINGLESEL
		| LVS_EX_GRIDLINES); //设置为报表风格

	m_lstIdt.SetExtendedStyle(m_lstIdt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int nItemIdx = 0;

	m_lstIdt.InsertColumn(nItemIdx++, "CPU序号", LVCFMT_LEFT, 70);
	m_lstIdt.InsertColumn(nItemIdx++, "序号", LVCFMT_LEFT, 70);
	m_lstIdt.InsertColumn(nItemIdx++, "段选择子", LVCFMT_LEFT, 100);
	m_lstIdt.InsertColumn(nItemIdx++, "函数地址", LVCFMT_LEFT, 100);
	return true;
}




BOOL CIdtDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	OnInitList();
	GetIdt();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


bool CIdtDlg::GetCallBack()
{
	GDT gdt;
	DWORD dwBytes;

	//解析gdt
	PSEGMENT_DESCRIPTOR sd = NULL;
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	HANDLE hDevice = NULL;
	if (!OpenDev(hDevice))
	{
		ExitProcess(0);
	}
	//printf("dwNumberOfProcess= %d\n", sysinfo.dwNumberOfProcessors);
	int shift = 1;
	int nC = 0;
	int nId = 0;
	for (DWORD i = 0; i < sysinfo.dwNumberOfProcessors; i++)
	{
		SetProcessAffinityMask(GetCurrentProcess(), shift);
		__asm {
			sgdt gdt
		}
		char OutBuf[0x3ff] = { 0 };

		if (!DeviceIoControl(hDevice, CTL_GOT, &gdt.base, sizeof(DWORD),
			OutBuf, sizeof(OutBuf), &dwBytes, NULL)) {
			CString Error;
			Error.Format("DeviceIoControl Error:%p\n", GetLastError());
			MessageBoxA(Error, "Error", 0);
			ExitProcess(0);
		}
		for (int j = 0; j < 24; j++)
		{
			sd = (PSEGMENT_DESCRIPTOR)(OutBuf + j * 8);
			CString szTmp;
			szTmp.Format("%d", i);
			nC = m_lstIdt.InsertItem(nId++, szTmp);
			szTmp.Format("%d", j);
			m_lstIdt.SetItemText(nC, 1, szTmp);
			szTmp.Format("%p", GetBaseAddr(sd));
			m_lstIdt.SetItemText(nC, 2, szTmp);
			szTmp.Format("%p", GetLimit(sd));
			m_lstIdt.SetItemText(nC, 3, szTmp);
			sd->G ? szTmp.Format("%s", "Page") : szTmp.Format("%s", "Byte");
			m_lstIdt.SetItemText(nC, 4, szTmp);
			szTmp.Format("%d", sd->DPL);
			m_lstIdt.SetItemText(nC, 5, szTmp);
			szTmp.Format("%d", sd->type);
			m_lstIdt.SetItemText(nC, 6, szTmp);

		}
		shift <<= 1;
	}
	CloseDev(hDevice);
	return TRUE;
}
