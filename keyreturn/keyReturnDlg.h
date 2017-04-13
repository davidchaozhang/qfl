
// keyReturnDlg.h : header file
//

#pragma once

#include <liveRead.h>


// CkeyReturnDlg dialog
class CkeyReturnDlg : public CDialogEx
{
// Construction
public:
	CkeyReturnDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_KEYRETURN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRefund();
	afx_msg void OnBnClickedButtonDonate();
	afx_msg void OnBnClickedButtonQrcode();
private:
	bool m_qrcode_flag;
	bool m_refund;
	LiveRead m_lread;
	std::string m_url;
};
