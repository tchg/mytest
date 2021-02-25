// CPteDlg.cpp: 实现文件
//

#include "pch.h"
#include "MyPCHunter2.h"
#include "CPteDlg.h"
#include "afxdialogex.h"


// CPteDlg 对话框

IMPLEMENT_DYNAMIC(CPteDlg, CDialogEx)

CPteDlg::CPteDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CPteDlg, pParent)
{
	
}

CPteDlg::~CPteDlg()
{
}

void CPteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LST_PDE, m_lstPde);
	DDX_Control(pDX, LST_PTE, m_lstPte);
}


BEGIN_MESSAGE_MAP(CPteDlg, CDialogEx)
	ON_COMMAND(IDOK, &CPteDlg::OnIdok)
	ON_NOTIFY(NM_CLICK, LST_PDE, &CPteDlg::OnNMClickPde)
END_MESSAGE_MAP()


// CPteDlg 消息处理程序


BOOL CPteDlg::OpenDev(HANDLE& hDevice)
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


BOOL CPteDlg::CloseDev(HANDLE& hDevice)
{
	if (hDevice != INVALID_HANDLE_VALUE) {
		CloseHandle(hDevice);
	}
	return true;
}


void CPteDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}


void CPteDlg::OnNMClickPde(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1)
	{
		POSITION pstion = m_lstPde.GetFirstSelectedItemPosition();
		//该函数获取由pos指定的列表项的索引，然后将pos设置为下一个位置的POSITION值
		int nId  = m_lstPde.GetNextSelectedItem(pstion);
		CString szTmp = m_lstPde.GetItemText(nId, 0);
		int ptd = strtol(szTmp, NULL, 16);
		m_lstPte.DeleteAllItems();
		ptd = ptd & 0xfffff000;
		if (ptd != 0)
		{
			InsertPte(ptd);
		}
		
	}
}


BOOL CPteDlg::OnInitList()
{
	m_lstPde.ModifyStyle(LVS_TYPEMASK, //清掉以前的风格 
		LVS_REPORT | LVS_SINGLESEL
		| LVS_EX_GRIDLINES); //设置为报表风格

	m_lstPde.SetExtendedStyle(m_lstPde.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int nItemIdx = 0;

	m_lstPde.InsertColumn(nItemIdx++, "页表", LVCFMT_LEFT, 100);

	m_lstPte.ModifyStyle(LVS_TYPEMASK, //清掉以前的风格 
		LVS_REPORT | LVS_SINGLESEL
		| LVS_EX_GRIDLINES); //设置为报表风格

	m_lstPte.SetExtendedStyle(m_lstPte.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	nItemIdx = 0;

	m_lstPte.InsertColumn(nItemIdx++, "页目录表", LVCFMT_LEFT, 100);

	return TRUE;
}


BOOL CPteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	OnInitList();
	InsertPde();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CPteDlg::InsertPde()
{
	// TODO: 在此处添加实现代码.
	BOOL bRet = TRUE;
	HANDLE hDevice = NULL;
	if (!OpenDev(hDevice))
	{
		ExitProcess(0);
	}
	DWORD dwBytes = 0;
	int nId = 0;
	int nC = 0;
	char* szBuf = new char[0x1000]{ 0 };
	if (!DeviceIoControl(hDevice, CTL_READMEMORY, &m_dwPdeAddr, 4,
		szBuf, 0x1000, &dwBytes, NULL)) {
		CString Error;
		Error.Format("DeviceIoControl Error:%p\n", GetLastError());
		MessageBoxA(Error, "Error", 0);
		ExitProcess(0);
	}
	PMYPDELIST list = (PMYPDELIST)szBuf;
	for (int j = 0; j < 256; j++)
	{
		CString szTmp;
		szTmp.Format("%p", list[j].m_pde->LowPart);
		m_lstPde.InsertItem(nId++, szTmp);
	}
	delete[] szBuf;
	CloseDev(hDevice);
	return bRet;
}


BOOL CPteDlg::InsertPte(DWORD dwPde)
{
	// TODO: 在此处添加实现代码.
	BOOL bRet = TRUE;
	HANDLE hDevice = NULL;
	if (!OpenDev(hDevice))
	{
		ExitProcess(0);
	}
	DWORD dwBytes = 0;
	int nId = 0;
	char* szBuf = new char[0x1000]{ 0 };
	if (!DeviceIoControl(hDevice, CTL_READMEMORY, &dwPde, 4,
		szBuf, 0x1000, &dwBytes, NULL)) {
		CString Error;
		Error.Format("DeviceIoControl Error:%p\n", GetLastError());
		MessageBoxA(Error, "Error", 0);
		ExitProcess(0);
	}
	PMYPDELIST list = (PMYPDELIST)szBuf;
	for (int j = 0; j < 256; j++)
	{
		CString szTmp;
		szTmp.Format("%p", list[j].m_pde->LowPart);
		m_lstPte.InsertItem(nId++, szTmp);
	}
	delete[] szBuf;
	CloseDev(hDevice);
	return bRet;
}
