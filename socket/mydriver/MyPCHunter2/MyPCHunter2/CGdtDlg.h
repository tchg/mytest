#pragma once
#include "GDT.h"

// CGdtDlg 对话框

class CGdtDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGdtDlg)

public:
	CGdtDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGdtDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CGdtDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstGdt;

	virtual BOOL OnInitDialog();
	bool OnInitList();
	bool GetGDT();
	BOOL OpenDev(HANDLE& hDevice);
	BOOL CloseDev(HANDLE& hDevice);
};
