// CGdtDlg.cpp: 实现文件
//

#include "pch.h"
#include "MyPCHunter2.h"
#include "CGdtDlg.h"
#include "afxdialogex.h"


// CGdtDlg 对话框

IMPLEMENT_DYNAMIC(CGdtDlg, CDialog)

CGdtDlg::CGdtDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CGdtDlg, pParent)
{

}

CGdtDlg::~CGdtDlg()
{
}

void CGdtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, lst_gdt, m_lstGdt);
}


BEGIN_MESSAGE_MAP(CGdtDlg, CDialog)
END_MESSAGE_MAP()


// CGdtDlg 消息处理程序


BOOL CGdtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//初始化listctrl
	OnInitList();
	GetGDT();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


bool CGdtDlg::OnInitList()
{
	// TODO: 在此处添加实现代码.
	m_lstGdt.ModifyStyle(LVS_TYPEMASK, //清掉以前的风格 
		LVS_REPORT | LVS_SINGLESEL
		| LVS_EX_GRIDLINES); //设置为报表风格

	m_lstGdt.SetExtendedStyle(m_lstGdt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int nItemIdx = 0;

	m_lstGdt.InsertColumn(nItemIdx++, "CPU序号", LVCFMT_LEFT, 70);
	m_lstGdt.InsertColumn(nItemIdx++, "选择子", LVCFMT_LEFT, 70);
	m_lstGdt.InsertColumn(nItemIdx++, "基址", LVCFMT_LEFT, 100);
	m_lstGdt.InsertColumn(nItemIdx++, "界限", LVCFMT_LEFT, 100);
	m_lstGdt.InsertColumn(nItemIdx++, "段粒度", LVCFMT_LEFT, 100);
	m_lstGdt.InsertColumn(nItemIdx++, "段特权级", LVCFMT_LEFT, 100);
	m_lstGdt.InsertColumn(nItemIdx++, "类型", LVCFMT_LEFT, 100);
	return true;
}


bool CGdtDlg::GetGDT()
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
			nC = m_lstGdt.InsertItem(nId++, szTmp);
			szTmp.Format("%d", j);
			m_lstGdt.SetItemText(nC, 1, szTmp);
			szTmp.Format("%p", GetBaseAddr(sd));
			m_lstGdt.SetItemText(nC, 2, szTmp);
			szTmp.Format("%p", GetLimit(sd));
			m_lstGdt.SetItemText(nC, 3, szTmp);
			sd->G ? szTmp.Format("%s", "Page") : szTmp.Format("%s", "Byte");
			m_lstGdt.SetItemText(nC, 4, szTmp);
			szTmp.Format("%d", sd->DPL);
			m_lstGdt.SetItemText(nC, 5, szTmp);
			szTmp.Format("%d", sd->type);
			m_lstGdt.SetItemText(nC, 6, szTmp);

		}
		shift <<= 1;
	}
	CloseDev(hDevice);
	return TRUE;
}


BOOL CGdtDlg::OpenDev(HANDLE& hDevice)
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


BOOL CGdtDlg::CloseDev(HANDLE& hDevice)
{
	if (hDevice != INVALID_HANDLE_VALUE) {
		CloseHandle(hDevice);
	}
	return true;
}

