// Corr.cpp : 实现文件
//

#include "stdafx.h"
#include "STC1.h"
#include "Corr.h"
#include "afxdialogex.h"


// CCorr 对话框

double M_COORD[6];

IMPLEMENT_DYNAMIC(CCorr, CDialogEx)

CCorr::CCorr(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCorr::IDD, pParent)
	, m_radiogroup(0)
{
	ReadConfig();
}

CCorr::~CCorr()
{
}

void CCorr::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO1, m_radiogroup);

	DDX_Text(pDX, IDC_FXEDIT, FLimitInTTS[0]);
	DDX_Text(pDX, IDC_FYEDIT, FLimitInTTS[1]);
	DDX_Text(pDX, IDC_FZEDIT, FLimitInTTS[2]);
	DDX_Text(pDX, IDC_FAXEDIT, FLimitInTTS[3]);
	DDX_Text(pDX, IDC_FAYEDIT, FLimitInTTS[4]);
	DDX_Text(pDX, IDC_FAZEDIT, FLimitInTTS[5]);

	DDX_Text(pDX, IDC_UXEDIT, ULimitInTTS[0]);
	DDX_Text(pDX, IDC_UYEDIT, ULimitInTTS[1]);
	DDX_Text(pDX, IDC_UZEDIT, ULimitInTTS[2]);
	DDX_Text(pDX, IDC_UAXEDIT, ULimitInTTS[3]);
	DDX_Text(pDX, IDC_UAYEDIT, ULimitInTTS[4]);
	DDX_Text(pDX, IDC_UAZEDIT, ULimitInTTS[5]);

	DDX_Text(pDX, IDC_SOXEDIT, StepOn[0]);
	DDX_Text(pDX, IDC_SOYEDIT, StepOn[1]);
	DDX_Text(pDX, IDC_SOZEDIT, StepOn[2]);
	DDX_Text(pDX, IDC_SOAXEDIT, StepOn[3]);
	DDX_Text(pDX, IDC_SOAYEDIT, StepOn[4]);
	DDX_Text(pDX, IDC_SOAZEDIT, StepOn[5]);

	DDX_Text(pDX, IDC_REFXEDIT, RefPoint[0]);
	DDX_Text(pDX, IDC_REFYEDIT, RefPoint[1]);
	DDX_Text(pDX, IDC_REFZEDIT, RefPoint[2]);

	DDX_Text(pDX, IDC_CORRXEDIT, CORR[0]);
	DDX_Text(pDX, IDC_CORRYEDIT, CORR[1]);
	DDX_Text(pDX, IDC_CORRZEDIT, CORR[2]);

	DDX_Text(pDX, IDC_COORDAXEDIT, M_COORD[3]);
	DDX_Text(pDX, IDC_COORDAYEDIT, M_COORD[4]);
	DDX_Text(pDX, IDC_COORDAZEDIT, M_COORD[5]);
}


BEGIN_MESSAGE_MAP(CCorr, CDialogEx)
	ON_BN_CLICKED(IDC_ChangeMotionRangeBUTTON, &CCorr::OnBnClickedChangemotionrangebutton)
END_MESSAGE_MAP()


// CCorr 消息处理程序
void CCorr::ReadConfig()
{
	DWORD dwCopied = 0;
	CString szKeyValue;
	CString m_szFileName = L"./SettingsINI.ini";
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"pitch", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	ULimitInTTS[0] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"roll", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	ULimitInTTS[1] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"iso", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	ULimitInTTS[2] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"lat", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	ULimitInTTS[3] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"long", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	ULimitInTTS[4] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"vert", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	ULimitInTTS[5] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();

	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"mpitch", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	FLimitInTTS[0] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"mroll", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	FLimitInTTS[1] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"miso", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	FLimitInTTS[2] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"mlat", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	FLimitInTTS[3] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"mlong", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	FLimitInTTS[4] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"mvert", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	FLimitInTTS[5] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();

	dwCopied = ::GetPrivateProfileString(L"StepOn", L"pitch", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	StepOn[0] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"roll", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	StepOn[1] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"iso", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	StepOn[2] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"LeftRight", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	StepOn[3] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"FrontBack", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	StepOn[4] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"TopBottom", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	StepOn[5] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();

	dwCopied = ::GetPrivateProfileString(L"ReferencePoint", L"lat", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	RefPoint[0] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"ReferencePoint", L"long", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	RefPoint[1] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"ReferencePoint", L"vert", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	RefPoint[2] = _wtof(szKeyValue.GetBuffer());

	dwCopied = ::GetPrivateProfileString(L"360Correction", L"X", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	CORR[0] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"360Correction", L"Y", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	CORR[1] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"360Correction", L"Z", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	CORR[2] = _wtof(szKeyValue.GetBuffer());


	dwCopied = ::GetPrivateProfileString(L"Translation", L"X", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	M_COORD[3] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"Translation", L"Y", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	M_COORD[4] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	dwCopied = ::GetPrivateProfileString(L"Translation", L"Z", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	M_COORD[5] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();

	dwCopied = ::GetPrivateProfileString(L"EndTreatment", L"M", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	m_radiogroup = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
}


void CCorr::OnBnClickedChangemotionrangebutton()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString m_szFileName = L"./SettingsINI.ini";
	CString szKeyValue;
	 szKeyValue.Format(L"%.1f",ULimitInTTS[0]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"pitch", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.1f", ULimitInTTS[1]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"roll", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.1f", ULimitInTTS[2]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"iso", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", ULimitInTTS[3]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"lat", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", ULimitInTTS[4]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"long", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", ULimitInTTS[5]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"vert", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);

	 szKeyValue.Format(L"%.1f", FLimitInTTS[0]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"mpitch", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.1f", FLimitInTTS[1]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"mroll", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.1f", FLimitInTTS[2]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"miso", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", FLimitInTTS[3]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"mlat", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", FLimitInTTS[4]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"mlong", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", FLimitInTTS[5]);
	 ::WritePrivateProfileString(L"LimitInTTS", L"mvert", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);

	 szKeyValue.Format(L"%.1f", StepOn[0]);
	 ::WritePrivateProfileString(L"StepOn", L"pitch", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.1f", StepOn[1]);
	 ::WritePrivateProfileString(L"StepOn", L"roll", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.1f", StepOn[2]);
	 ::WritePrivateProfileString(L"StepOn", L"iso", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", StepOn[3]);
	 ::WritePrivateProfileString(L"StepOn", L"LeftRight", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", StepOn[4]);
	 ::WritePrivateProfileString(L"StepOn", L"FrontBack", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", StepOn[5]);
	 ::WritePrivateProfileString(L"StepOn", L"TopBottom", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);

	 szKeyValue.Format(L"%.2f", RefPoint[0]);
	 ::WritePrivateProfileString(L"ReferencePoint", L"lat", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", RefPoint[1]);
	 ::WritePrivateProfileString(L"ReferencePoint", L"long", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", RefPoint[2]);
	 ::WritePrivateProfileString(L"ReferencePoint", L"vert", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);

	 szKeyValue.Format(L"%.2f", CORR[0]);
	 ::WritePrivateProfileString(L"360Correction", L"X", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", CORR[1]);
	 ::WritePrivateProfileString(L"360Correction", L"Y", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", CORR[2]);
	 ::WritePrivateProfileString(L"360Correction", L"Z", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);

	 szKeyValue.Format(L"%.2f", M_COORD[3]);
	 ::WritePrivateProfileString(L"Translation", L"X", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", M_COORD[4]);
	 ::WritePrivateProfileString(L"Translation", L"Y", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 szKeyValue.Format(L"%.2f", M_COORD[5]);
	 ::WritePrivateProfileString(L"Translation", L"Z", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);

	 //
	 if (m_radiogroup==0)//standby
	 {
		 szKeyValue.Format(L"%d", 0);
		 ::WritePrivateProfileString(L"EndTreatment", L"M", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 }
	 else if (m_radiogroup == 1)//keep iso,standby
	 {
		 szKeyValue.Format(L"%d", 1);
		 ::WritePrivateProfileString(L"EndTreatment", L"M", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
	 }
	 
	 AfxMessageBox(_T("已保存修改参数！"));
}

void CCorr::OnOK()
{
	OnBnClickedChangemotionrangebutton();
}
