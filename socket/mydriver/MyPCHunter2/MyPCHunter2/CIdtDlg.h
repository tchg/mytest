#pragma once


// CIdtDlg 对话框

class CIdtDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CIdtDlg)

public:
	CIdtDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CIdtDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CIdtDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	bool GetIdt();
	bool OpenDev(HANDLE& hDevice);
	bool CloseDev(HANDLE hDevice);
	bool OnInitList();
	CListCtrl m_lstIdt;
	virtual BOOL OnInitDialog();
	bool GetCallBack();
};
