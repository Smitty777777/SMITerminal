
// SMITerminalDlg.h : header file
//

#pragma once
#include <map>
#include <functional>


// CSMITerminalDlg dialog
class CSMITerminalDlg : public CDialogEx
{
// Construction
public:
	CSMITerminalDlg(CWnd* pParent = nullptr);	// standard constructor
	
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SMITERMINAL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CListCtrl m_listCtrl;
	CFont m_listFont;
	CEdit m_edit;
	CBrush m_brushBlack;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMCustomdrawList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnSetfocusEdit1();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor); 
	afx_msg void OnEnChangeEdit1();
private:
	int m_protectedLength = 0;
	std::map<CString, std::function<void()>> m_commands;
	int m_gridCount = 0;
	int FindNextAvailableRow();
	void HandleEcon();
	void HandleUnknown(const CString& cmd);
	void ShowIndex();
	void ShowComm();
	void ShowNews();
	void ShowRate();
	void ShowForex();
	void ShowIPO();
	void ShowEarnings();
	void ShowEqMetric();
};
