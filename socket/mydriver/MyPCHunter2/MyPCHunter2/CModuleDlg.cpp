// CModuleDlg.cpp: 实现文件
//

#include "pch.h"
#include "MyPCHunter2.h"
#include "CModuleDlg.h"
#include "afxdialogex.h"


// CModuleDlg 对话框

IMPLEMENT_DYNAMIC(CModuleDlg, CDialogEx)

CModuleDlg::CModuleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CModuleDlg, pParent)
{

}

CModuleDlg::~CModuleDlg()
{
}

void CModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LST_MODULE, m_lstModule);
}


BEGIN_MESSAGE_MAP(CModuleDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CModuleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CModuleDlg 消息处理程序


void CModuleDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


BOOL CModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	OnInitList();
    GetMoudules();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


bool CModuleDlg::OnInitList()
{
	m_lstModule.ModifyStyle(LVS_TYPEMASK, //清掉以前的风格 
		LVS_REPORT | LVS_SINGLESEL
		| LVS_EX_GRIDLINES); //设置为报表风格

	m_lstModule.SetExtendedStyle(m_lstModule.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int nItemIdx = 0;

	m_lstModule.InsertColumn(nItemIdx++, "内核模块名称", LVCFMT_LEFT, 170);
	m_lstModule.InsertColumn(nItemIdx++, "内核模块基址", LVCFMT_LEFT, 70);
	m_lstModule.InsertColumn(nItemIdx++, "内核模块大小", LVCFMT_LEFT, 70);
	return true;
}


NTSTATUS CModuleDlg::GetMoudules()
{
    NTSTATUS Status;
    PRTL_PROCESS_MODULES            Modules;
    PRTL_PROCESS_MODULE_INFORMATION ModuleInfo;
    PVOID Buffer;
    ULONG BufferSize = 0x1000;
    ULONG ReturnLength;
    ULONG i;
    PFNNtQuerySystemInformation MyNtQuerySystemInformation;
    int nId = 0;
    int nC = 0;
    HMODULE hMod = GetModuleHandleA("ntdll.dll");
    MyNtQuerySystemInformation = (PFNNtQuerySystemInformation)GetProcAddress(hMod, "NtQuerySystemInformation");

    if (MyNtQuerySystemInformation == 0)
    {
        return STATUS_NO_MEMORY;
    }
    
retry:
    Buffer = new char[BufferSize] {0};
    //
    if (!Buffer) {
        return STATUS_NO_MEMORY;
    }
    Status = MyNtQuerySystemInformation(SystemModuleInformation,
        Buffer,
        BufferSize,
        &ReturnLength
    );

    if (Status == STATUS_INFO_LENGTH_MISMATCH) {
        delete[] Buffer;
        BufferSize = ReturnLength;
        goto retry;
    }

    if (NT_SUCCESS(Status)) {
        Modules = (PRTL_PROCESS_MODULES)Buffer;
        for (i = 0, ModuleInfo = &(Modules->Modules[0]);
            i < Modules->NumberOfModules;
            i++, ModuleInfo++) {
            CString szTmp;
            nC = m_lstModule.InsertItem(nId++, (char*)Modules->Modules[i].FullPathName);

            szTmp.Format("%p", Modules->Modules[i].ImageBase); //序号
            m_lstModule.SetItemText(nC, 1, szTmp);

            szTmp.Format("%p", Modules->Modules[i].ImageSize);
            m_lstModule.SetItemText(nC, 2, szTmp);
            /*printf("[HookSSDT] ImageBase=%p ImageSize=%p FullPathName=%s\n",
                Modules->Modules[i].ImageBase,
                Modules->Modules[i].ImageSize,
                Modules->Modules[i].FullPathName);*/
        }
    }

    delete[] Buffer;
    return Status;
}
