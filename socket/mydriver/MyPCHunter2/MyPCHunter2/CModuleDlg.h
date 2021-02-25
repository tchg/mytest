#pragma once
#include "GDT.h"

// CModuleDlg 对话框

class CModuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModuleDlg)

public:
	CModuleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CModuleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CModuleDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	bool OnInitList();
	CListCtrl m_lstModule;

	NTSTATUS GetMoudules();
};

