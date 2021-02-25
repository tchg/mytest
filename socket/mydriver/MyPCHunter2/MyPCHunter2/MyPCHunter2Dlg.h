
// MyPCHunter2Dlg.h: 头文件
//

#pragma once
#include "CGdtDlg.h"
#include "CProcessDlg.h"
#include "LoadSys.h"
#include "CIdtDlg.h"
#include "CSsdtDlg.h"
#include "CModuleDlg.h"
#include "CCallBack.h"
// CMyPCHunter2Dlg 对话框
class CMyPCHunter2Dlg : public CDialogEx
{
// 构造
public:
	CMyPCHunter2Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYPCHUNTER2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl    m_TabCtrl;
	CGdtDlg     m_dlgGdt;
	CIdtDlg     m_dlgIdt;
	CSsdtDlg    m_dlgSsdt;
	CProcessDlg m_dlgProcess;
	CModuleDlg  m_dlgModule;
	CCallBack   m_dlgCallBack;
	CString     m_SysName;
	CString 	m_DevName;
	CString     m_path;

	bool intiTabCtrl();
	afx_msg void OnTcnSelchangeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	BOOL WriteSys();
	bool DelDlg();
};
