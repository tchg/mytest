#pragma once
#include "GDT.h"
#include "CReadWriteDlg.h"
#include "CPteDlg.h"
// CProcessDlg 对话框

class CProcessDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessDlg)

public:
	CProcessDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProcessDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CProcessDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstProcess;
	int m_nIndex;
	BOOL OnInitList();
	virtual BOOL OnInitDialog();
	BOOL OpenDev(HANDLE& hDevice);
	BOOL CloseDev(HANDLE& hDevice);
	BOOL GetProcess();
	afx_msg void OnNMRClickPath(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMnPtd();
	afx_msg void OnMnReadwrite();
	afx_msg void OnLvnItemchangedPath(NMHDR* pNMHDR, LRESULT* pResult);
};
