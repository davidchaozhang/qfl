
// keyReturnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "keyReturn.h"
#include "keyReturnDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CkeyReturnDlg dialog



CkeyReturnDlg::CkeyReturnDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CkeyReturnDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_refund = true;
	m_qrcode_flag = false;
}

void CkeyReturnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CkeyReturnDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_REFUND, &CkeyReturnDlg::OnBnClickedButtonRefund)
	ON_BN_CLICKED(IDC_BUTTON_DONATE, &CkeyReturnDlg::OnBnClickedButtonDonate)
	ON_BN_CLICKED(IDC_BUTTON_QRCODE, &CkeyReturnDlg::OnBnClickedButtonQrcode)
END_MESSAGE_MAP()


// CkeyReturnDlg message handlers

BOOL CkeyReturnDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	GetDlgItem(IDC_BUTTON_DONATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_REFUND)->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CkeyReturnDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CkeyReturnDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CkeyReturnDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CkeyReturnDlg::OnBnClickedButtonRefund()
{
	std::string refund_code;
	m_refund = true;
	if (m_qrcode_flag) {
		std::string code = m_lread.getCode();
		if (m_lread.parse_code(code) != 0) {
			std::cout << code << " is invalid" << std::endl;
			return;
		}
		else {
			refund_code = "name=" + m_lread.getKeyword() + ",key=refund";
			m_lread.checkSalesforce(m_url, refund_code);
		}

		m_qrcode_flag = false;
		GetDlgItem(IDC_BUTTON_DONATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_REFUND)->EnableWindow(FALSE);
	}
}


void CkeyReturnDlg::OnBnClickedButtonDonate()
{
	std::string donate_code;
	m_refund = false;
	if (m_qrcode_flag) {
		std::string code = m_lread.getCode();
		if (m_lread.parse_code(code) != 0) {
			std::cout << code << " is invalid" << std::endl;
			return;
		}
		else {
			donate_code = "name=" + m_lread.getKeyword() + ",key=donate";
			m_lread.checkSalesforce(m_url, donate_code);
		}

		m_qrcode_flag = false;
		GetDlgItem(IDC_BUTTON_DONATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_REFUND)->EnableWindow(FALSE);
	}
}


void CkeyReturnDlg::OnBnClickedButtonQrcode()
{
	// TODO: Add extra initialization here
	std::string checkin_code;

	m_url = m_lread.getTestURL();

	if (m_lread.zbar_video_detect() == 0)
	{
		m_qrcode_flag = true;
		GetDlgItem(IDC_BUTTON_DONATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_REFUND)->EnableWindow(TRUE);
		_sleep(100);
	}

	return;
}
