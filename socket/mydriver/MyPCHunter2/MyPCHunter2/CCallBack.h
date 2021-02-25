#pragma once


// CCallBack 对话框

class CCallBack : public CDialogEx
{
	DECLARE_DYNAMIC(CCallBack)

public:
	CCallBack(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCallBack();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CCallBack };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	bool OnInitList();

	bool OpenDev(HANDLE& hDevice);
	bool CloseDev(HANDLE hDevice);
	CListCtrl m_lstCallback;
	bool GetCallBack();
};
