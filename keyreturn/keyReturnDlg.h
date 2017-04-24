
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
	~CkeyReturnDlg();	
// Dialog Data
	enum { IDD = IDD_KEYRETURN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	LiveRead *getLiveReadHandle(void) { return &m_lread; }
	bool createRecordFolder(const char* foldername);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnBnClickedButtonRefund();
	afx_msg void OnBnClickedButtonDonate();
	afx_msg void OnBnClickedButtonQrcode();
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
	int DisplayConfirmMessageBox();
	static int32_t videoDetectFunction(void *);
private:
	CFont m_font;
	CFont m_title;
	bool m_qrcode_flag;
	bool m_refund;
	LiveRead m_lread;
	std::string m_url;
	int32_t m_running_status;
	std::string m_record_filename;
	std::string m_record_folder;
};
