// CSsdtDlg.cpp: 实现文件
//

#include "pch.h"
#include "MyPCHunter2.h"
#include "CSsdtDlg.h"
#include "afxdialogex.h"
#include "GDT.h"

extern PVOID KeServiceDescriptorTable;
// CSsdtDlg 对话框

IMPLEMENT_DYNAMIC(CSsdtDlg, CDialogEx)

CSsdtDlg::CSsdtDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CSsdtDlg, pParent)
{

}

CSsdtDlg::~CSsdtDlg()
{
}

void CSsdtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LST_SSDT, m_lstSsdt);
}


BEGIN_MESSAGE_MAP(CSsdtDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSsdtDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSsdtDlg 消息处理程序


bool CSsdtDlg::OpenDev(HANDLE& hDevice)
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


bool CSsdtDlg::CloseDev(HANDLE hDevice)
{
	if (hDevice != INVALID_HANDLE_VALUE) {
		CloseHandle(hDevice);
	}
	return true;
}


bool CSsdtDlg::OnInitList()
{
	m_lstSsdt.ModifyStyle(LVS_TYPEMASK, //清掉以前的风格 
		LVS_REPORT | LVS_SINGLESEL
		| LVS_EX_GRIDLINES); //设置为报表风格

	m_lstSsdt.SetExtendedStyle(m_lstSsdt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int nItemIdx = 0;

	m_lstSsdt.InsertColumn(nItemIdx++, "序号", LVCFMT_LEFT, 70);
	m_lstSsdt.InsertColumn(nItemIdx++, "当前函数地址", LVCFMT_LEFT, 70);
	return true;
}


BOOL CSsdtDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	OnInitList();
	GetSsdt();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSsdtDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


bool CSsdtDlg::GetSsdt()
{

	DWORD dwBytes;

	HANDLE hDevice = NULL;
	if (!OpenDev(hDevice))
	{
		ExitProcess(0);
	}
	
	int nC = 0;
	int nId = 0;
	MYSSDT ssdt = { 0 };

	if (!DeviceIoControl(hDevice, CTL_SSDT, &ssdt, sizeof(DWORD),
		&ssdt, sizeof(ssdt), &dwBytes, NULL)) {
		CString Error;
		Error.Format("DeviceIoControl Error:%p\n", GetLastError());
		MessageBoxA(Error, "Error", 0);
		ExitProcess(0);
	}
	/*CString Error;
	Error.Format("%p %d %d %p\n", ssdt.ServiceTableBase,ssdt.ServiceCounterTable,ssdt.NumberOfServices,ssdt.ParamTableBase);
	MessageBoxA(Error, "Error", 0);*/
	int* nPssdt = new int[ssdt.NumberOfServices]{ 0 };
	if (!DeviceIoControl(hDevice, CTL_GOT, &ssdt.ServiceTableBase, sizeof(DWORD),
		nPssdt, ssdt.NumberOfServices * 4, &dwBytes, NULL)) {
		CString Error;
		Error.Format("DeviceIoControl Error:%p\n", GetLastError());
		MessageBoxA(Error, "Error", 0);
		ExitProcess(0);
	}

	for (int j = 0; j < ssdt.NumberOfServices; j++)
	{
		CString szTmp;
		szTmp.Format("%d", j); //序号
		nC = m_lstSsdt.InsertItem(nId++, szTmp);

		szTmp.Format("%p", nPssdt[j]); //地址
		m_lstSsdt.SetItemText(nC, 1, szTmp);

	}
	delete[] nPssdt;
	CloseDev(hDevice);
	return true;
}
