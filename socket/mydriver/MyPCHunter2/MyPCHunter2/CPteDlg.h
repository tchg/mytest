#pragma once
#include "GDT.h"

// CPteDlg 对话框

class CPteDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPteDlg)

public:
	CPteDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPteDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CPteDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPdeAddr;

	BOOL OpenDev(HANDLE& hDevice);
	BOOL CloseDev(HANDLE& hDevice);
	afx_msg void OnIdok();
	CListCtrl m_lstPde;
	CListCtrl m_lstPte;
	afx_msg void OnNMClickPde(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL OnInitList();
	virtual BOOL OnInitDialog();
	BOOL InsertPde();
	BOOL InsertPte(DWORD dwPde);
};
