#pragma once


// CSsdtDlg 对话框

class CSsdtDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSsdtDlg)

public:
	CSsdtDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSsdtDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CSsdtDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	bool OpenDev(HANDLE& hDevice);
	bool CloseDev(HANDLE hDevice);
	bool OnInitList();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CListCtrl m_lstSsdt;
	bool GetSsdt();
};
