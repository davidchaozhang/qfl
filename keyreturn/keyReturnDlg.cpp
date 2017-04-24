
// keyReturnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "keyReturn.h"
#include "keyReturnDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/utime.h>

#include <boost/timer/timer.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL _exit_signal = false;

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
	m_running_status = 0;
	m_record_filename.clear();
	m_font.CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Arial"));
	m_title.CreateFont(28, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Times"));
}

CkeyReturnDlg::~CkeyReturnDlg()
{
	_exit_signal = true;
	m_record_filename.clear();
	_sleep(500);
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
	ON_BN_CLICKED(IDOK, &CkeyReturnDlg::OnBnClickedOk)
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

	GetDlgItem(IDOK)->SendMessage(WM_SETFONT, WPARAM(HFONT(m_font)), 0);
	GetDlgItem(IDC_STATIC)->SendMessage(WM_SETFONT, WPARAM(HFONT(m_title)), 0);
	GetDlgItem(IDC_BUTTON_QRCODE)->SendMessage(WM_SETFONT, WPARAM(HFONT(m_font)), 0);
	GetDlgItem(IDC_BUTTON_REFUND)->SendMessage(WM_SETFONT, WPARAM(HFONT(m_font)), 0);
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

bool CkeyReturnDlg::createRecordFolder(const char* foldername)
{
	std::string recorder_folder, record_filename;
	if (foldername == NULL)
		return false;

	if (!boost::filesystem::exists(foldername)) {
		boost::filesystem::create_directory(foldername);
		recorder_folder = foldername + std::string("/2017");
		boost::filesystem::create_directory(recorder_folder);
		record_filename = recorder_folder + std::string("/key_return.txt");
	}
	else
		record_filename = foldername + std::string("/2017") + std::string("/key_return.txt");
	return true;
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
			if (DisplayConfirmMessageBox()) {
				refund_code = "name=" + m_lread.getKeyword() + "&refund=-20";
				m_lread.checkSalesforce(m_url, refund_code);
			}
		}

		m_qrcode_flag = false;
		GetDlgItem(IDC_BUTTON_REFUND)->EnableWindow(FALSE);

		FILE *hf = fopen(m_record_filename.c_str(), "a+");
		if (hf == NULL) {
			fprintf(stderr, "Can't open input !\n");
			return;
		}


	}
}


void CkeyReturnDlg::OnBnClickedButtonDonate()
{
	SYSTEMTIME  system_time;;
	GetLocalTime(&system_time);
	std::string cur_time = std::to_string(system_time.wYear) + "." +
		std::to_string(system_time.wMonth) + "." +
		std::to_string(system_time.wDay) + "." +
		std::to_string(system_time.wHour) + "." +
		std::to_string(system_time.wMinute) + "." +
		std::to_string(system_time.wSecond);

	std::string record_string;
	std::string donate_code;
	m_refund = false;
	if (m_qrcode_flag) {
		std::string code = m_lread.getCode();
		if (m_lread.parse_code(code) != 0) {
			std::cout << code << " is invalid" << std::endl;
			return;
		}
		else {
			LiveRead::KeyWords *kw = this->getLiveReadHandle()->getRecordWords();
			FILE *fh = fopen(m_record_filename.c_str(), "a+");
			if (fh == NULL)
			{
				fprintf(stderr, "Not able to open file %s\n", m_record_filename.c_str());
				return;
			}

			if (DisplayConfirmMessageBox()) {
				donate_code = "name=" + m_lread.getKeyword() + "&refund=1";
				m_lread.checkSalesforce(m_url, donate_code);
				record_string = cur_time + " " + kw->person_id + " " + kw->party_type + " " + kw->church + " " + "refund";
				fprintf(fh, "%s\n", record_string.c_str());
				fclose(fh);
			}
			else {
				record_string = cur_time + " " + kw->person_id + " " + kw->party_type + " " + kw->church + " " + "donate";
				fprintf(fh, "%s\n", record_string.c_str());
				fclose(fh);
			}
		}

		m_qrcode_flag = false;
		GetDlgItem(IDC_BUTTON_REFUND)->EnableWindow(FALSE);
	}
}


void CkeyReturnDlg::OnBnClickedButtonQrcode()
{
	// TODO: Add extra initialization here
	std::string checkin_code;

	m_url = m_lread.getTestURL();

	m_running_status = videoDetectFunction(this);

	if (m_running_status > 0)
	{
		m_qrcode_flag = true;
		GetDlgItem(IDC_BUTTON_REFUND)->EnableWindow(TRUE);
		_sleep(100);
	}

	return;
}


int CkeyReturnDlg::DisplayConfirmMessageBox()
{
	int msgboxID = 0;
	// Display a message box asking user to confirm the action
	msgboxID = AfxMessageBox(_T("Please Confirm to Proceed: "), MB_YESNO | MB_ICONSTOP);
	if (msgboxID == IDYES)
		return 1;
	else if (msgboxID == IDNO)
		return 0;

	return 0;

}

void CkeyReturnDlg::OnBnClickedOk()
{
	_exit_signal = true;
	_sleep(500);
	CDialogEx::OnOK();
}


int32_t CkeyReturnDlg::videoDetectFunction(void* lpParam)
{
	CkeyReturnDlg *thisapp = (CkeyReturnDlg*)lpParam;

	int32_t ret = 0;

	cv::VideoCapture cap(0); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		std::cout << "Cannot open the video cam" << std::endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
	std::cout << "Frame size : " << dWidth << " x " << dHeight << std::endl;
	cv::namedWindow("QR Code Scanning Window", CV_WINDOW_AUTOSIZE); //create a window

	while (1)
	{
		if (_exit_signal)
			break;
		cv::Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video
		if (!bSuccess) //if not success, break loop
		{
			std::cout << "Cannot read a frame from video stream" << std::endl;
			ret = -2;
			break;
		}

		LiveRead *lread = thisapp->getLiveReadHandle();
		ret = lread->zbar_qrcode_detect(frame);
		if (ret > 0)
			break;
	}
	return ret;
}