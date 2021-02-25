#pragma once


// CReadWriteDlg 对话框

class CReadWriteDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReadWriteDlg)

public:
	CReadWriteDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CReadWriteDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CReadWriteDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPid;
	BOOL OpenDev(HANDLE& hDevice);
	BOOL CloseDev(HANDLE& hDevice);
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnIdok();
};
