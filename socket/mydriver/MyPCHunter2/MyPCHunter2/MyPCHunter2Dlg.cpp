
// MyPCHunter2Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MyPCHunter2.h"
#include "MyPCHunter2Dlg.h"
#include "afxdialogex.h"
#include "GDT.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyPCHunter2Dlg 对话框



CMyPCHunter2Dlg::CMyPCHunter2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MYPCHUNTER2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyPCHunter2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, TAB_CTRL, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CMyPCHunter2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, TAB_CTRL, &CMyPCHunter2Dlg::OnTcnSelchangeCtrl)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMyPCHunter2Dlg 消息处理程序

BOOL CMyPCHunter2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	char szPath[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(MAX_PATH, szPath);
	m_SysName = "hello.sys";
	m_DevName = "Hello";
	//写驱动文件

	m_path.Format("%s\\%s", szPath, m_SysName);
	if (!WriteSys())
	{
		AfxMessageBox("写驱动失败");
	}
	LoadSys::InstallSvsA(m_DevName, m_path);
	LoadSys::StartSvsA(m_DevName);
	//初始化tabctrl
	intiTabCtrl();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMyPCHunter2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyPCHunter2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyPCHunter2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



bool CMyPCHunter2Dlg::intiTabCtrl()
{
	// TODO: 在此处添加实现代码.
	int nIdx = 0;
	m_TabCtrl.InsertItem(nIdx++, "GDT");
	m_TabCtrl.InsertItem(nIdx++, "进程遍历");
	m_TabCtrl.InsertItem(nIdx++, "IDT");
	m_TabCtrl.InsertItem(nIdx++, "SSDT");
	m_TabCtrl.InsertItem(nIdx++, "模块列表");
	m_TabCtrl.InsertItem(nIdx++, "系统回调");

	m_dlgGdt.Create(IDD_CGdtDlg, &m_TabCtrl);
	//移动到合适位置
	CRect rc;
	m_TabCtrl.GetClientRect(&rc);

	rc.top += 30;
	rc.left += 10;
	rc.right -= 10;
	rc.bottom -= 10;
	m_dlgGdt.MoveWindow(&rc);
	m_dlgGdt.ShowWindow(SW_SHOW);
	return TRUE;
}


void CMyPCHunter2Dlg::OnTcnSelchangeCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int nSelIdx = m_TabCtrl.GetCurSel();

	DelDlg();
	//移动到合适位置
	CRect rc;
	m_TabCtrl.GetClientRect(&rc);

	rc.top += 30;
	rc.left += 10;
	rc.right -= 10;
	rc.bottom -= 10;


	switch (nSelIdx)
	{
	case 0:
		m_dlgGdt.Create(IDD_CGdtDlg, &m_TabCtrl);
		m_dlgGdt.MoveWindow(&rc);
		m_dlgGdt.ShowWindow(SW_SHOW);
		break;
	case 1:
		m_dlgProcess.Create(IDD_CProcessDlg, &m_TabCtrl);
		m_dlgProcess.MoveWindow(&rc);
		m_dlgProcess.ShowWindow(SW_SHOW);
		break;
	case 2:
		m_dlgIdt.Create(IDD_CIdtDlg, &m_TabCtrl);
		m_dlgIdt.MoveWindow(&rc);
		m_dlgIdt.ShowWindow(SW_SHOW);
		break;
	case 3:
		m_dlgSsdt.Create(IDD_CSsdtDlg, &m_TabCtrl);
		m_dlgSsdt.MoveWindow(&rc);
		m_dlgSsdt.ShowWindow(SW_SHOW);
		break;
	case 4:
		m_dlgModule.Create(IDD_CModuleDlg, &m_TabCtrl);
		m_dlgModule.MoveWindow(&rc);
		m_dlgModule.ShowWindow(SW_SHOW);
		break;
	case 5:
		m_dlgCallBack.Create(IDD_CCallBack, &m_TabCtrl);
		m_dlgCallBack.MoveWindow(&rc);
		m_dlgCallBack.ShowWindow(SW_SHOW);
		break;
	}
}


void CMyPCHunter2Dlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	LoadSys::StopSvsA(m_DevName);
	LoadSys::DeleteSvsA(m_DevName);
	DeleteFile(m_path);
	CDialogEx::OnClose();
}


BOOL CMyPCHunter2Dlg::WriteSys()
{
	HRSRC hResource = FindResource(GetModuleHandle(NULL),
		MAKEINTRESOURCE(SYS_MYDEV), TEXT("SYS"));
	if (hResource)
	{
		HGLOBAL   hg = LoadResource(GetModuleHandle(NULL), hResource);
		if (hg)
		{
			LPVOID pData = LockResource(hg);
			if (pData)
			{
				DWORD dwSize = SizeofResource(GetModuleHandle(NULL), hResource);
				CFile calcFile;

				if (calcFile.Open(m_path, CFile::modeCreate | CFile::modeReadWrite))
				{
					calcFile.Write(pData, dwSize);
					calcFile.Close();
					return true;
				}
			}
		}
	}
	// TODO: 在此处添加实现代码.
	return false;
}


bool CMyPCHunter2Dlg::DelDlg()
{
	m_dlgGdt.DestroyWindow();
	m_dlgProcess.DestroyWindow();
	m_dlgIdt.DestroyWindow();
	m_dlgSsdt.DestroyWindow();
	m_dlgModule.DestroyWindow();
	m_dlgCallBack.DestroyWindow();
	return true;
}
