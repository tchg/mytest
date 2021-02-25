// CProcessDlg.cpp: 实现文件
//

#include "pch.h"
#include "MyPCHunter2.h"
#include "CProcessDlg.h"
#include "afxdialogex.h"


// CProcessDlg 对话框

IMPLEMENT_DYNAMIC(CProcessDlg, CDialog)

CProcessDlg::CProcessDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CProcessDlg, pParent)
{

}

CProcessDlg::~CProcessDlg()
{
}

void CProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LST_PATH, m_lstProcess);
}


BEGIN_MESSAGE_MAP(CProcessDlg, CDialog)
	ON_NOTIFY(NM_RCLICK, LST_PATH, &CProcessDlg::OnNMRClickPath)
	ON_COMMAND(MN_PTE, &CProcessDlg::OnMnPtd)
	ON_COMMAND(MN_READWRITE, &CProcessDlg::OnMnReadwrite)
	ON_NOTIFY(LVN_ITEMCHANGED, LST_PATH, &CProcessDlg::OnLvnItemchangedPath)
END_MESSAGE_MAP()


// CProcessDlg 消息处理程序


BOOL CProcessDlg::OnInitList()
{
	m_lstProcess.ModifyStyle(LVS_TYPEMASK, //清掉以前的风格 
		LVS_REPORT | LVS_SINGLESEL
		| LVS_EX_GRIDLINES); //设置为报表风格

	m_lstProcess.SetExtendedStyle(m_lstProcess.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int nItemIdx = 0;

	m_lstProcess.InsertColumn(nItemIdx++, "名称", LVCFMT_LEFT, 100);
	m_lstProcess.InsertColumn(nItemIdx++, "进程id", LVCFMT_LEFT, 100);
	m_lstProcess.InsertColumn(nItemIdx++, "eprocess", LVCFMT_LEFT, 100);
	m_lstProcess.InsertColumn(nItemIdx++, "dirbase", LVCFMT_LEFT, 100);
	return true;;
}


BOOL CProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	OnInitList();
	GetProcess();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CProcessDlg::OpenDev(HANDLE& hDevice)
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


BOOL CProcessDlg::CloseDev(HANDLE& hDevice)
{
	if (hDevice != INVALID_HANDLE_VALUE) {
		CloseHandle(hDevice);
	}
	return true;
}


BOOL CProcessDlg::GetProcess()
{
	HANDLE hDevice = NULL;
	if (!OpenDev(hDevice))
	{
		ExitProcess(0);
	}
	DWORD dwBytes = 0;
	int nId = 0;
	int nC = 0;
	char* szBuf = new char[0x1000] { 0 };
	int z = 0;
	if (!DeviceIoControl(hDevice, CTL_ENUMPROCESS, &z, 4,
		szBuf, 0x1000, &dwBytes, NULL)) {
		CString Error;
		Error.Format("DeviceIoControl Error:%p\n", GetLastError());
		MessageBoxA(Error, "Error", 0);
		ExitProcess(0);
	}
	PMYPROCESSLIST list = (PMYPROCESSLIST)szBuf;
	for (int j = 0; j < list->nNum; j++)
	{
		CString szTmp;
		nC = m_lstProcess.InsertItem(nId++, list->m_process[j].image_name);
		szTmp.Format("%d", list->m_process[j].pid);
		m_lstProcess.SetItemText(nC, 1, szTmp);
		szTmp.Format("%p", list->m_process[j].eprocess);
		m_lstProcess.SetItemText(nC, 2, szTmp);
		szTmp.Format("%p", list->m_process[j].dir_base);
		m_lstProcess.SetItemText(nC, 3, szTmp);
	}
	delete[] szBuf;
	CloseDev(hDevice);
	return TRUE;
}


void CProcessDlg::OnNMRClickPath(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));
		CMenu menu;
		//添加线程操作
		menu.LoadMenu(MENU_PREOCESS);
		CMenu* popup = menu.GetSubMenu(0);
		ASSERT(popup != NULL);
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

		//下面的两行代码主要是为了后面的操作为准备的
		//获取列表视图控件中第一个被选择项的位置  
		POSITION pstion = m_lstProcess.GetFirstSelectedItemPosition();
		//该函数获取由pos指定的列表项的索引，然后将pos设置为下一个位置的POSITION值
		 m_nIndex = m_lstProcess.GetNextSelectedItem(pstion);
	}

}


void CProcessDlg::OnMnPtd()
{
	CPteDlg dlg;
	CString szTmp = m_lstProcess.GetItemText(m_nIndex, 3);
	MessageBoxA(szTmp, NULL, 0);
	int ptd = strtol(szTmp, NULL, 16);
	dlg.m_dwPdeAddr = ptd;
	dlg.DoModal();
	//dlg.m_dwPdeAddr = 
}


void CProcessDlg::OnMnReadwrite()
{
	// TODO: 在此添加命令处理程序代码
	CReadWriteDlg dlg;
	CString szTmp = m_lstProcess.GetItemText(m_nIndex, 1);
	MessageBoxA(szTmp, NULL, 0);
	int ptd = strtol(szTmp, NULL, 10);
	dlg.m_dwPid = ptd;
	dlg.DoModal();
}


void CProcessDlg::OnLvnItemchangedPath(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
