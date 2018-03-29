
// STC1Dlg.cpp : 实现文件
//

//声音相关

//#include <windows.h>
//#pragma comment(lib, "WINMM.LIB")

#include "stdafx.h"
#include "STC1.h"
#include "STC1Dlg.h"
#include "afxdialogex.h"
#include "Login.h"
#include "LoginSetPass.h"
#include "afxdialogex.h"
#include "DOFComm.h"
#include "math.h"
#include "mmsystem.h"
#include<ATLBASE.H>
#include<iomanip>
#include <io.h>

#include <hpdf.h>

#include "winsock.h"    
#include "mysql.h"  

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib,"libmysql.lib")//附加依赖项，也可以在工程属性中设置
#pragma comment(lib,"winmm.lib")//预编译静态链接库
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib,"libhpdf.lib")//附加依赖项，也可以在工程属性中设置

#using "ModbusTCP.dll"
using namespace ModbusTCP;
using namespace System;
gcroot<TcpClient^> PLC = gcnew  TcpClient();

#include "SixDof.h"
//#include "MATRICDIV.h"
int  m_nCmd;
DataToDOF dd;

//全局变量
extern float single_move_speed_show[6];
extern double TTS_coordinate_value[7];
extern int IfMoveInPlace[6];//是否运动到位的标志
int ChairMode;//椅子的使用状态
bool REset;//先从负角度转到5.再回零的操作标志位
unsigned long int BufferStopCount = 0;
bool BufferStop = S_OFF;
extern float m_para[18];
extern float ref[6];
extern double ForZAxisCorr[6];
extern Translation translation;
UINT count0;     //主时钟计数器
Mode mode;
Sinmovetime sinmovetime;

extern TreatReportData m_report;
extern COLORREF ShowColor;
extern int m_radiogroup2;
extern BOOL PisOpen;//计划为打开的标志位
//extern int BeamNum;
//extern 	CString BeamName[20];//束流名称
//extern 	CString BeamTime[20];//束流应用时间
//extern 	CString BeamOper[20];//束流操作员
//extern 	double PlanBeamInfo[20][6];//束流位置的信息
//extern 	double ReBeamInfo[20][6];//真实位置的信息

extern CString permission;
extern CString UserName;

extern double M_COORD[6];
extern bool IsTreatMoving;//是否处于治疗状态运动
int DoingTreatMoving;//是否正在运动处于治疗状态运动
extern double correctdatafor360[6];//用于360平台的校正
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CSTC1Dlg 对话框


CSTC1Dlg::CSTC1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSTC1Dlg::IDD, pParent)	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONLOGO);

	//m_pc2pc.Init(GetSafeHwnd());
	m_dof.Init(GetSafeHwnd());
	//this->SetDlgItemTextW(IDC_RAZ360EDIT, L"0");
	//this->SetDlgItemText(IDC_AZ360EDIT, L"0");
	//this->SetDlgItemTextW(IDC_AZ360EDIT, L"0");
	DOFInit();
	//ddd.IsCtrl2Pc = 0;
	count0 = 0;
	ChairMode = 0;
	for (int i = 0; i < 3; i++)//速度赋值
	{
		dd.Vxyz[i] = 0;
		dd.Axyz[i] = 0;
	}

	six_enable = false;
	EMSTOP = false;
	friststart = true;
	nMMTimer = 0;
	for (int i = 0; i<6; i++)
	{
		if (i<3)
		{
			single_move_speed_show[i] = 1;//运动速度赋值
		}
		else
		{
			single_move_speed_show[i] = 10;
		}
		userdata.eRR[i] = 0.f;//初始化userdata结构体
		userdata.m_force[i] = 0.f;
		userdata.m_eDOF[i] = 0.f;
		userdata.m_para[i] = 0;
		userdata.m_para[6 + i] = 0.1f;
		userdata.m_para[12 + i] = 0.f;
		attitude.mid[i] = 0.f;
		attitude.sen[i] = 0.f;
		IfMoveInPlace[i] = 1;
	}

	datachange.eX = 0;//初始化datachange结构体
	datachange.eY = 0;
	datachange.eZ = 0;
	datachange.eH = 0;
	datachange.eP = 0;
	datachange.eR = 0;
	datachange.eAx = 0;
	datachange.eAy = 0;
	datachange.eAz = 0;
	datachange.eVx = 0;
	datachange.eVy = 0;
	datachange.eVz = 0;
	sinmovetime.max = 1;
	sinmovetime.test = 0;
	mode.m_nAct = 0;
	m_nCmd = 255;
	userdata.total_send_do = 1;
	mode.m_nVarIdx = 0;


	lockflag = true;			//锁定标志
	enableflag = false;		//使能标志框
	EthLinkOK = false;
	Ret = 20;
	m_hJueDuiWeiZhi = "0";
	//fangxiang = 0;
	LR = false;
	CanUse = TRUE;
	REset = false;
	//EStop = false;

	maxtime = 0;
	IfMoveGet = false;
	for (size_t i = 0; i < 20; i++)
	{
		m_report.BeamName[i] = "1";
		m_report.BeamOper[i] = "null";
		m_report.BeamTime[i]="-1";
		m_report.PatientID = "123456";
		m_report.BeamAngle[i] = "-1";
	}
	DoingTreatMoving = 0;
}

void CSTC1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);

	DDX_Text(pDX, IDC_XEDIT, attitude.mid[0]);
	DDX_Text(pDX, IDC_YEDIT, attitude.mid[1]);
	DDX_Text(pDX, IDC_ZEDIT, attitude.mid[2]);
	DDX_Text(pDX, IDC_AXEDIT, attitude.mid[3]);
	DDX_Text(pDX, IDC_AYEDIT, attitude.mid[4]);
	DDX_Text(pDX, IDC_AZEDIT, attitude.mid[5]);

	DDX_Control(pDX, IDC_STARTBUTTON, m_Startcontrol);
	DDX_Control(pDX, IDC_EnBUTTON, m_Encontrol);
	DDX_Control(pDX, IDC_LockBUTTON, m_Lockcontrol);
	DDX_Control(pDX, IDC_EStopBUTTON, m_ESTOPcontrol);
}



BEGIN_MESSAGE_MAP(CSTC1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSTC1Dlg::OnTcnSelchangeTab1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CTRLBUTTON, &CSTC1Dlg::OnBnClickedCtrlbutton)
	ON_BN_CLICKED(IDC_STARTBUTTON, &CSTC1Dlg::OnBnClickedStartbutton)
	ON_BN_CLICKED(IDC_RESETBUTTON, &CSTC1Dlg::OnBnClickedResetbutton)
	ON_BN_CLICKED(IDC_LockBUTTON, &CSTC1Dlg::OnBnClickedLockbutton)
	ON_BN_CLICKED(IDC_EStopBUTTON, &CSTC1Dlg::OnBnClickedEstopbutton)
	ON_BN_CLICKED(IDC_BUTTON6, &CSTC1Dlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON10, &CSTC1Dlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON9, &CSTC1Dlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_EnBUTTON, &CSTC1Dlg::OnBnClickedEnbutton)
	ON_COMMAND(ID_32775, &CSTC1Dlg::On32775)
	ON_COMMAND(ID_SaveReport, &CSTC1Dlg::OnSavereport)
	ON_COMMAND(ID_SoftWareInfo, &CSTC1Dlg::OnSoftwareinfo)
	ON_COMMAND(ID_32780, &CSTC1Dlg::OnMenuExit)
	ON_COMMAND(ID_EDIT_RESETPASSWORD, &CSTC1Dlg::OnEditResetpassword)
	ON_BN_CLICKED(IDC_BUTTON13, &CSTC1Dlg::OnBnClickedButton13)
	ON_COMMAND(ID_INPUTPATIENT_ADDPATIENT, &CSTC1Dlg::OnInputpatientAddpatient)
	ON_COMMAND(ID_INPUTPATIENT_EDITPATIENT, &CSTC1Dlg::OnInputpatientEditpatient)
	ON_COMMAND(ID_INPUTPLAN_ADDPLAN, &CSTC1Dlg::OnInputplanAddplan)
	ON_COMMAND(ID_INPUTPLAN_EDITPLAN, &CSTC1Dlg::OnInputplanEditplan)
	ON_COMMAND(ID_USER_ADDUSER, &CSTC1Dlg::OnUserAdduser)
	ON_BN_CLICKED(IDC_movebeginBUTTON, &CSTC1Dlg::OnBnClickedmovebeginbutton)
END_MESSAGE_MAP()


// CSTC1Dlg 消息处理程序

BOOL CSTC1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	CLogin m_login;
	if (m_login.DoModal()==IDOK)
	{

	}
	else
	{
		CDialogEx::OnCancel();
		return FALSE;
	}

	//SetDlgItemText(IDD_STC1_DIALOG,);
	SetWindowText(L"RCTS v0.2   UserName: " + UserName + "  Permission: " + permission);
	menu.LoadMenu(IDR_MENU1);  //IDR_MENU1为菜单栏ID号  
	SetMenu(&menu);
	//menu.Detach();

		m_tab.InsertItem(0, L"Treatment Mode");  //添加参数一选项卡    
		m_tab.InsertItem(1, L"QA Mode");  //添加参数二选项卡   
		m_tab.InsertItem(2, L"Settings");    //添加结果选项卡
		m_mainui.Create(IDD_MAINDIALOG, GetDlgItem(IDD_STC1_DIALOG));
		m_cali.Create(IDD_CALIDIALOG, GetDlgItem(IDD_STC1_DIALOG));
		m_corr.Create(IDD_CORRDIALOG, GetDlgItem(IDD_STC1_DIALOG));

		if (permission == "3")
		{
			m_mainui.ShowWindow(true);
			m_cali.ShowWindow(false);
			m_corr.ShowWindow(false);
			this->GetDlgItem(IDC_TAB1)->EnableWindow(TRUE);
		}
		else if (permission == "2")
		{
			this->GetDlgItem(IDC_TAB1)->EnableWindow(TRUE);
		}
		else if (permission == "1")
		{
			m_mainui.ShowWindow(true);
			m_cali.ShowWindow(false);
			m_corr.ShowWindow(false);
			this->GetDlgItem(IDC_TAB1)->EnableWindow(FALSE);
		}


	CRect rs;    
	m_tab.GetClientRect(&rs);    
//调整子对话框在父窗口中的位置    
	rs.top+=1;     
	rs.bottom-=10;     
	rs.left+=300;    //520 
	rs.right+=500;  //540

//设置子对话框尺寸并移动到指定位置    
	m_mainui.MoveWindow(&rs);    
	m_cali.MoveWindow(&rs);    
	m_corr.MoveWindow(&rs); 
	m_mainui.ShowWindow(true);    
	m_cali.ShowWindow(false);    
	m_corr.ShowWindow(false); 
	m_tab.SetCurSel(0);

	ChangeUI();

	//m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE

	//DOFInit();//初始化六自由度平台解码对象
	
	//m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	//SetWindowPos(&wndTopMost, 0, 0, 0, 0,
		//SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	
}

void CSTC1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == SC_CLOSE)
	{
		if (ChairMode == TreatmentMode)
		{
			AfxMessageBox(_T("当前为治疗模式，若想关闭软件，请先结束治疗"));
			return;
		}
		if (IDYES == MessageBox(_T("Do you want to exit program？Please confirm that the current device is in a safe state"), _T("WARNING!"), MB_YESNO))
		{
			bool canclose=true;
			for (size_t i = 0; i < 6; i++)
			{
				if (abs(re_attitude_for_show[i]) > 1)
				{
					canclose = false;
				}
			}
			if (((m_dof.FromDOFBuf.nDOFStatus!= 55)&& (!canclose)&& EthLinkOK))
			{
				AfxMessageBox(_T("当前治疗椅状态不能关闭！"));
				return;
			}
			CString m_szFileName = L"./SettingsINI.ini";
			CString szKeyValue;
			szKeyValue.Format(L"%.0f", fangxiang);
			::WritePrivateProfileString(L"Open", L"fangxiang", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
			if (LR)
			{
				szKeyValue.Format(L"%d", 1);
				::WritePrivateProfileString(L"Open", L"LR", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
			}
			else
			{
				szKeyValue.Format(L"%d", 0);
				::WritePrivateProfileString(L"Open", L"LR", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
			}		
				EthLinkOK = false;
				enableflag = false;
				::timeKillEvent(nMMTimer);
				KillTimer(0);
				KillTimer(1);
				nMMTimer = 0;
				PLC->DeLink();
			CDialogEx::OnSysCommand(nID, lParam);
		}
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSTC1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	//ChangeUI();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSTC1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSTC1Dlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (ChairMode == TreatmentMode)
	{
		AfxMessageBox(_T("当前状态不允许切换至模式，如需要切换，请先结束当前治疗流程！"));
		return;
	}
	int CurSel = m_tab.GetCurSel();    
 switch(CurSel)    
{    
case 0:    
		ChairMode = NormalMode;
        m_mainui.ShowWindow(true);  
        m_cali.ShowWindow(false);    
		m_corr.ShowWindow(false);    
 break;    
 case 1:
	 if (permission == "1")
	 {
		 AfxMessageBox(_T("permission level 2or 3 should be needed！"));
		 return;
	 }
	 if (ChairMode == NormalMode)
	 {
		 ChairMode = QAMode;
		 m_mainui.ShowWindow(false);
		 m_cali.ShowWindow(true); m_cali.PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON1, BN_CLICKED), NULL);
		 m_corr.ShowWindow(false);
	 }
	 else if (ChairMode == TreatmentMode)
	 {
		 CurSel = 0;
		 AfxMessageBox(_T("Can't to QAMode，please finish treatment workflow frist！"));
	 }
break;    
 case 2:
	 if (permission=="2"|| permission == "1")
	 {
		 AfxMessageBox(_T("permission level 3 should be needed！"));
		 return;
	 }
	 ChairMode = NormalMode;
	 if (ChairMode == NormalMode)
	 {
		 m_mainui.ShowWindow(false);
		 m_cali.ShowWindow(false);
		 m_corr.ShowWindow(true);
	 }
	 else if (ChairMode == TreatmentMode)
	 {
		 CurSel = 0;
		 AfxMessageBox(_T("Can't to Settings，please finish treatment workflow frist！"));
	 }
 break;    
 default:    
         ;    
    }  

	*pResult = 0;
}

BOOL CSTC1Dlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
			//case VK_RETURN: //屏蔽回车键
				//return TRUE;
		case VK_ESCAPE: //屏蔽ESC键
			return TRUE;
		default:
			break;
		}
	}


	if (m_hAccel)
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
		{
			AfxMessageBox(_T("截到了Ctrl+C键"));
			::SendMessage(m_mainui.GetSafeHwnd(), WM_COMMAND, IDC_TREATMODEBUTTON, 0);
			return(TRUE);
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CALLBACK TimeProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)//六自由度平台主计时器
{
	CSTC1Dlg *pDlg = (CSTC1Dlg*)dwUser;
	pDlg->timerout();
}

void CSTC1Dlg::OnOK()
{}
void CSTC1Dlg::OnBnClickedStartbutton()//启动定时器,建立通信
{
	//PlaySound(_T("6531.wav"), NULL, SND_FILENAME | SND_ASYNC);
	//ChangeUI();
	//PLC连接，360°平台通信
	if (!EthLinkOK) {
		String ^LocalIP = gcnew String("192.168.0.131");
		String ^RemoteIP = gcnew String("192.168.0.101");
		Ret = PLC->EntLink(LocalIP, 0, RemoteIP, 502, "LICENCE@LFL20174516-MODBUSTCP-V2[969750209]");
		if (Ret == 0)//成功建立连接
		{
			EthLinkOK = true;
			m_Startcontrol.SetColor(BLACK, GREEN);
		}
		else
		{
			return;
		}
		//六自由度平台通信
		SetTimer(0, 20, NULL);
		nMMTimer = ::timeSetEvent(TIMER, 0, TimeProc, (LPARAM)this, TIME_PERIODIC);
		SetTimer(1, 100, NULL);
		SetDlgItemText(IDC_STARTBUTTON, L"Connected");
		this->GetDlgItem(IDC_EnBUTTON)->EnableWindow(TRUE);

		HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONGREEN);
		((CStatic*)GetDlgItem(IDC_CON_STA_STATIC))->SetIcon(m_hIcon);

		DWORD dwCopied = 0;
		CString szKeyValue;
		CString m_szFileName = L"./SettingsINI.ini";
		dwCopied = ::GetPrivateProfileString(L"Open", L"fangxiang", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
		fangxiang = _wtof(szKeyValue.GetBuffer());
		szKeyValue.ReleaseBuffer();
		dwCopied = ::GetPrivateProfileString(L"Open", L"LR", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
		if (_wtof(szKeyValue.GetBuffer())==1)
		{
			LR = TRUE;
		}
		else
		{
			LR = FALSE;
		}
		szKeyValue.ReleaseBuffer();
	}
	else
	{
		/*if (IDYES == MessageBox(_T("Determine to disconnect the device?please make sure "), _T("提示"), MB_YESNO))
		{
			EthLinkOK = false;
			enableflag = false;
			::timeKillEvent(nMMTimer);
			KillTimer(0);
			KillTimer(1);
			SetDlgItemText(IDC_STARTBUTTON, L"START");
			nMMTimer = 0;
			PLC->DeLink();
			m_Startcontrol.SetColor(BLACK, LLTGRAY);
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONGRAY);
			((CStatic*)GetDlgItem(IDC_CON_STA_STATIC))->SetIcon(m_hIcon);
			((CStatic*)GetDlgItem(IDC_EN_STA_STATIC))->SetIcon(m_hIcon);
			((CStatic*)GetDlgItem(IDC_LOCK_STA_STATIC))->SetIcon(m_hIcon);
			Sleep(2000);
			((CStatic*)GetDlgItem(IDC_OPE_STA_STATIC))->SetIcon(m_hIcon);
		}*/
	}
}


void CSTC1Dlg::timerout()
{
	float Kslow = 1;
	float re_attitude_for_show[6] = { 0 };//显示用姿态数组
	dd.nCheckID = 55;//m_dof.ToDOFBuf.nCheckID;
	dd.nAct = 0;//mode.m_nAct;
	dd.nCmd = m_nCmd;
	dd.nReserved = 1;//userdata.total_send_do;
					 //网络收的下位机状态，重点是电机的码值
	
	//m_nRecvCount = m_dof.nRecvCount;
	if (EMSTOP)
	{
		HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONRED);
		((CStatic*)GetDlgItem(IDC_OPE_STA_STATIC))->SetIcon(m_hIcon);
		SetDlgItemText(IDC_OPE_STATIC, L"Operating Status :Emergency Stop");
		this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);

		m_mainui.GetDlgItem(IDC_UPBUTTON)->EnableWindow(FALSE);
		m_mainui.GetDlgItem(IDC_DOWNBUTTON)->EnableWindow(FALSE);
		m_cali.GetDlgItem(IDC_CTRLBUTTON2)->EnableWindow(FALSE);
	}
	else if (!enableflag)
	{
		this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);

		m_mainui.GetDlgItem(IDC_UPBUTTON)->EnableWindow(FALSE);
		m_mainui.GetDlgItem(IDC_DOWNBUTTON)->EnableWindow(FALSE);
		m_cali.GetDlgItem(IDC_CTRLBUTTON2)->EnableWindow(FALSE);
	}
	else
	{
		if (ChairMode == NormalMode)
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONGRAY);
			((CStatic*)GetDlgItem(IDC_MODE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_MODE_STATIC, L"Mode: Normal Mode");
		}
		if (ChairMode == QAMode)
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONYELLOW);
			((CStatic*)GetDlgItem(IDC_POSLOCK_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_POS_STATIC, L"Postrue Lock Status: QA Unlock");

			m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONYELLOW);
			((CStatic*)GetDlgItem(IDC_MODE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_MODE_STATIC, L"Mode: QA Mode");

			m_cali.GetDlgItem(IDC_CTRLBUTTON2)->EnableWindow(TRUE);
		}
		if (ChairMode==TreatmentMode)
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONGREEN);
			((CStatic*)GetDlgItem(IDC_OPE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_OPE_STATIC, L"Operating Status : Normal Working");
			this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);
			m_mainui.GetDlgItem(IDC_UPBUTTON)->EnableWindow(FALSE);
			m_mainui.GetDlgItem(IDC_DOWNBUTTON)->EnableWindow(FALSE);

			((CStatic*)GetDlgItem(IDC_MODE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_MODE_STATIC, L"Mode: Treatment Mode");
		}		
		else if (m_dof.FromDOFBuf.nDOFStatus == 3)//正常工作态
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONGREEN);
			((CStatic*)GetDlgItem(IDC_OPE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_OPE_STATIC, L"Operating Status : Normal Working");
			this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(TRUE);
			this->GetDlgItem(IDC_BUTTON10)->EnableWindow(TRUE);
			this->GetDlgItem(IDC_BUTTON13)->EnableWindow(TRUE);
			m_mainui.GetDlgItem(IDC_UPBUTTON)->EnableWindow(TRUE);
			m_mainui.GetDlgItem(IDC_DOWNBUTTON)->EnableWindow(TRUE);
		}
		else if (m_dof.FromDOFBuf.nDOFStatus == 1&& m_nCmd == 6)//正在升起
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONYELLOW);
			((CStatic*)GetDlgItem(IDC_OPE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_OPE_STATIC, L"Operating Status : Rising");
			this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
		}
		else if (m_dof.FromDOFBuf.nDOFStatus == 2)//准运行态
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONYELLOW);
			((CStatic*)GetDlgItem(IDC_OPE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_OPE_STATIC, L"Operating Status :Quasi-Operational");
			OnBnClickedButton9();//开始运行
			//this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
			//this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
		}
		else if (m_dof.FromDOFBuf.nDOFStatus == 2 && m_nCmd == 2)//复位中
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONYELLOW);
			((CStatic*)GetDlgItem(IDC_OPE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_OPE_STATIC, L"Operating Status :Resetting");
			OnBnClickedButton9();//开始运行
								 //this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
								 //this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
		}
		else if (m_dof.FromDOFBuf.nDOFStatus == 55&& lockflag)//下降到最低并且锁定
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONYELLOW);
			((CStatic*)GetDlgItem(IDC_OPE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_OPE_STATIC, L"Operating Status : At The Lowest");
			GetDlgItemText(IDC_RAZ360EDIT, str);
			if (abs(_wtof(str.GetBuffer())+90)<2)
			{
				this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(TRUE);
				this->GetDlgItem(IDC_EnBUTTON)->EnableWindow(TRUE);
			}
		}
		else if (m_dof.FromDOFBuf.nDOFStatus == 55 && !lockflag)//下降到最低但并非锁定锁定，就不能升起
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONYELLOW);
			((CStatic*)GetDlgItem(IDC_OPE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_OPE_STATIC, L"Operating Status : At The Lowest");
			m_mainui.GetDlgItemText(IDC_RZEDIT2, str);
			if (abs(_wtof(str.GetBuffer()))<1 || abs(_wtof(str.GetBuffer()) - 360)<1)
			{
				this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(TRUE);
				this->GetDlgItem(IDC_EnBUTTON)->EnableWindow(TRUE);
			}
		}
		else if (m_dof.FromDOFBuf.nDOFStatus == 0 && m_nCmd == 7)//正在下降
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONYELLOW);
			((CStatic*)GetDlgItem(IDC_OPE_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_OPE_STATIC, L"Operating Status : Going Down");
			this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);
			m_mainui.GetDlgItem(IDC_UPBUTTON)->EnableWindow(FALSE);
			m_mainui.GetDlgItem(IDC_DOWNBUTTON)->EnableWindow(FALSE);
		}

		if (ChairMode == 0)
		{
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONGREEN);
			((CStatic*)GetDlgItem(IDC_POSLOCK_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_POS_STATIC, L"Postrue Lock Status: normal");
		}
	}
	
	

	for (size_t i = 0; i < 18; i++)
	{
		userdata.m_para[i] = m_para[i];
	}

	if (m_dof.FromDOFBuf.nDOFStatus == 3)
	{
		this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
	}

	if (m_dof.FromDOFBuf.nDOFStatus == 3 && dd.nCmd == S_CMD_Work)
	{
		this->GetDlgItem(IDC_CTRLBUTTON)->EnableWindow(TRUE);
		//this->GetDlgItem(IDC_BUTTON13)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(TRUE);
		//this->GetDlgItem(IDC_BUTTON10)->EnableWindow(TRUE);
	}

	if (m_dof.FromDOFBuf.nDOFStatus == 2 && dd.nCmd == S_CMD_Work)
	{
		this->GetDlgItem(IDC_CTRLBUTTON)->EnableWindow(TRUE);
	}

	if (m_dof.FromDOFBuf.nDOFStatus == 2 && dd.nCmd == S_CMD_Back2MID)
	{
		this->GetDlgItem(IDC_CTRLBUTTON)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
	}

	if (m_dof.FromDOFBuf.nDOFStatus == 55 && dd.nCmd == S_CMD_Stop)
	{
		this->GetDlgItem(IDC_CTRLBUTTON)->EnableWindow(TRUE);
		if (enableflag)
		{
			this->GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
		}
		
		//this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(TRUE);
	}

	if (m_dof.FromDOFBuf.nDOFStatus == 55 && dd.nCmd == S_CMD_RUN)
	{
		//this->GetDlgItem(IDC_WORK)->EnableWindow(TRUE);
		//this->GetDlgItem(IDC_RUN)->EnableWindow(FALSE);
		//this->GetDlgItem(IDC_TOMID)->EnableWindow(FALSE);
		//this->GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	}

	//有回中位命令时，所有参数进行初始化||(m_nCmd==S_CMD_ChaConf)
	if ((m_nCmd == S_CMD_Back2MID) || (m_nCmd == S_CMD_Work) || (m_nCmd == S_CMD_Stop))
	{
		for (int i = 0; i<6; i++)
		{
			dd.DOFs[i] = 0.f;
			userdata.m_para[i] = 0.f;
			userdata.m_para[i + 6] = 0.f;
			attitude.mid[i] = 0.f;
			attitude.end[i] = 0.f;
			attitude.sen[i] = 0.f;
		}
		datachange.eR = 0;
		datachange.eP = 0;
		datachange.eH = 0;
		datachange.eX = 0;
		datachange.eY = 0;
		datachange.eZ = 0;
		count0 = 0;
		count1 = 0;
		mode.flag_count = F_None;
		onoff.set_button = S_ON;
		onoff.back2mid = S_OFF;
	}

	switch (mode.flag_count) //平台正常运行模式
	{
	case F_SinMove://单步运动
		UINT ACOUNT;
		float abs_tattitude[6];
		ACOUNT = 0;
		count0++;
		for (int i = 0; i < 6; i++)
		{
			abs_tattitude[i] = fabs((attitude.end[i] - attitude.mid[i])*60000.f) / 10000.f;//过渡
			if (i < 3)
			{
				if (ACOUNT <= PI / (single_move_speed[i] / (abs_tattitude[i] / 2.f))*(1000.f / TIMER))
					ACOUNT = PI / (single_move_speed[i] / (abs_tattitude[i] / 2.f))*(1000.f / TIMER);
			}
			else
			{
				if (ACOUNT <= PI / (single_move_speed[i] / (abs_tattitude[i] / 2.f))*(1000.f / TIMER))
					ACOUNT = PI / (single_move_speed[i] / (abs_tattitude[i] / 2.f))*(1000.f / TIMER);
			}
		}

		if (count0 <= ACOUNT)//表示还在运动过程中
		{
			for (int i = 0; i < 6; i++)
			{
				if (i < 3)
				{
					if (count0 <= PI / (single_move_speed[i] / (abs_tattitude[i] / 2.f))*(1000.f / TIMER))
					{
						attitude.sen[i] = ((attitude.end[i] / 2.f) + (attitude.end[i] / 2.f)*cos(single_move_speed[i] / (abs_tattitude[i] / 2.f)*count0 / (1000.f / TIMER)));
						attitude.sen[i] = attitude.sen[i] + ((attitude.mid[i] / 2.f) - (attitude.mid[i] / 2.f)*cos(single_move_speed[i] / (abs_tattitude[i] / 2.f)*count0 / (1000.f / TIMER)));
					}
				}
				else
				{
					if (count0 <= PI / (single_move_speed[i] / (abs_tattitude[i] / 2.f))*(1000.f / TIMER))
					{
						attitude.sen[i] = ((attitude.end[i] / 2.f) + (attitude.end[i] / 2.f)*cos(single_move_speed[i] / (abs_tattitude[i] / 2.f)*count0 / (1000.f / TIMER)));
						attitude.sen[i] = attitude.sen[i] + ((attitude.mid[i] / 2.f) - (attitude.mid[i] / 2.f)*cos(single_move_speed[i] / (abs_tattitude[i] / 2.f)*count0 / (1000.f / TIMER)));
					}
				}
			}
		}//运动结束
		else
		{
			for (int i = 0; i < 6; i++)
			{
				attitude.sen[i] = attitude.mid[i];
			}
			//判断是否运动到位			
		}
		//if (JudgeAction())
		//{
			//ShowColor = RGB(0, 255, 0);//AfxMessageBox(_T("已运动到位！"));
		//}
		break;

	case F_SinMotion://单轴正弦运动
		if (count0<2000)
			Kslow = count0 / 2000.0;
		else if (count0<(sinmovetime.max + 20) * 100)
			Kslow = 1;
		else if (count0 >= (sinmovetime.max + 20) * 100 && count0 <= ((sinmovetime.max + 20) * 100 + 2000))
			Kslow = (sinmovetime.max * 100 + 4000 - count0) / 2000.0;
		else
		{
			//GetDlgItem(IDC_STOPSINMOVE)->EnableWindow(FALSE);
			//this->GetDlgItem(IDC_STARTSINMOVE)->EnableWindow(TRUE);
			Kslow = 0;
		}
		if (BufferStop == S_ON)
		{
			if ((count0 - BufferStopCount)<1000)
				Kslow = Kslow*(1 - (count0 - BufferStopCount) / 1000.0);
			else
			{
				Kslow = 0;
				//this->GetDlgItem(IDC_STARTSINMOVE)->EnableWindow(TRUE);
				//this->GetDlgItem(IDC_STOPSINMOVE)->EnableWindow(FALSE);
			}

		}

		if (Kslow>1) Kslow = 1;
		if (Kslow<0) Kslow = 0;

		datachange.eR = Kslow*userdata.m_para[0] * sin(6.283*(userdata.m_para[6] * count0*TIMER / 1000.f + userdata.m_para[12] / 360.f));
		datachange.eP = Kslow*userdata.m_para[1] * sin(6.283*(userdata.m_para[7] * count0*TIMER / 1000.f + userdata.m_para[13] / 360.f));
		datachange.eH = Kslow*userdata.m_para[2] * sin(6.283*(userdata.m_para[8] * count0*TIMER / 1000.f + userdata.m_para[14] / 360.f));
		datachange.eX = Kslow*userdata.m_para[3] * sin(6.283*(userdata.m_para[9] * count0*TIMER / 1000.f + userdata.m_para[15] / 360.f));
		datachange.eY = Kslow*userdata.m_para[4] * sin(6.283*(userdata.m_para[10] * count0*TIMER / 1000.f + userdata.m_para[16] / 360.f));
		datachange.eZ = Kslow*userdata.m_para[5] * sin(6.283*(userdata.m_para[11] * count0*TIMER / 1000.f + userdata.m_para[17] / 360.f));
		count0++;

		sinmovetime.test -= 0.01;
		if (sinmovetime.test<0) sinmovetime.test = 0;
		break;

	}


	if (mode.flag_count == F_SinMove)
	{
		for (int i = 0; i<6; i++)
		{
			if (i<3)
				dd.DOFs[i] = attitude.sen[i];
			else
				dd.DOFs[i] = attitude.sen[i] / 1000.f;
		}
	}
	else
	{
		dd.DOFs[0] = datachange.eR;
		dd.DOFs[1] = datachange.eP;
		dd.DOFs[2] = datachange.eH;
		dd.DOFs[3] = datachange.eX / 1000.f;
		dd.DOFs[4] = datachange.eY / 1000.f;
		dd.DOFs[5] = datachange.eZ / 1000.f;
	}

	m_dof.SendData(&dd);
}

void CSTC1Dlg::OnBnClickedCtrlbutton()//控制治疗椅的移动
{
	translation.Offset[3] = M_COORD[3];
	translation.Offset[4] = M_COORD[4];
	translation.Offset[5] = M_COORD[5];

	CString m_hJueDuiSuduSet = L"240";//20°每10秒
									  //360°平台的控制
									  //CString m_hJueDuiWeiZhi;
	GetDlgItemText(IDC_AZ360EDIT, m_hJueDuiWeiZhi);
	//int m_hJueDuiWei = _ttof(m_hJueDuiWeiZhi)*100;
	//CString m_hJueDuiWeiZhiSet;
	//m_hJueDuiWeiZhiSet.Format(L"%d", m_hJueDuiWei);
	//CString m_hJueDuiWeiZhiSet = m_hJueDuiWeiZhi + "00";
	CString m_hJueDuiWeiZhiSet;
	m_hJueDuiWeiZhiSet.Format(L"%.2f", _wtof(m_hJueDuiWeiZhi.GetBuffer()) * 100);
	CString m_hJueDuiWeiZhiH = ToHdata(m_hJueDuiWeiZhiSet);
	CString m_hJueDuiWeiZhiL = ToLdata(m_hJueDuiWeiZhiSet);

	cli::array<Object ^>^ bRD = gcnew cli::array<Object ^>(3);
	String^ JueDuiSuduSet = gcnew String(m_hJueDuiSuduSet.GetBuffer(0));
	String^ JueDuiWeiZhiH = gcnew String(m_hJueDuiWeiZhiH.GetBuffer(0));
	String^ JueDuiWeiZhiL = gcnew String(m_hJueDuiWeiZhiL.GetBuffer(0));

	bRD[0] = JueDuiSuduSet;//定义速度（10°每10s）
	bRD[1] = JueDuiWeiZhiL;
	bRD[2] = JueDuiWeiZhiH;
	if (enableflag)
	{

		// 在此调用Button按钮按下的操作
		//short Ret;
		Ret = PLC->CmdWrite(ModbusTCP::TcpClient::PlcMemory::HR, ModbusTCP::TcpClient::DataType::INT16, 4510, 3, bRD); //写PLC D414 D415 D416的数据
		if (Ret != 0)
		{
			//MessageBox(L"通信异常1！");
			return;
		}
		if (m_hJueDuiWeiZhi.Left(1) == "-")
		{
			Ret = PLC->Bit_Set(ModbusTCP::TcpClient::PlcMemory::DQ, 2052, 0);//置位PLC M4线圈
			if (Ret != 0)
			{
				//MessageBox(L"通信异常2！");
				return;
			}
		}
		else
		{
			Ret = PLC->Bit_Set(ModbusTCP::TcpClient::PlcMemory::DQ, 2051, 0);//置位PLC M3线圈
			if (Ret != 0)
			{
				//MessageBox(L"通信异常3！");
				return;
			}
		}
	}
	else
	{
		//MessageBox(L"系统未使能！");
		return;
	}
	delete bRD;
	delete JueDuiWeiZhiH;
	delete JueDuiWeiZhiL;
	delete JueDuiSuduSet;

	if (enableflag)
	{
		//ddd.IsCtrl2Pc = 2;
		count0 = 0;
		mode.flag_count = F_SinMove;

		for (int i = 0; i < 3; i++)
		{
			attitude.end[i] = dd.DOFs[i];
		}
		for (int i = 3; i < 6; i++)
		{
			attitude.end[i] = dd.DOFs[i] * 1000.f;
		}
		for (size_t i = 0; i < 6; i++)
		{
			speed[i] = attitude.mid[i];
		}
		UpdateData(TRUE);//用控件中的值更新对应的变量值

		if (ChairMode == TreatmentMode)
		{
			//DoingTreatMoving = 0;
			CtrlSpeed();
		}

		GetXYCORRData(attitude.mid);
		
		for (int i = 0; i < 3; i++)
		{
			if (attitude.mid[i] >= 20.f)//软件限位
			{
				attitude.mid[i] = 20.0f;
			}
			else if (attitude.mid[i] <= -20.f)//软件限位
			{
				attitude.mid[i] = -20.f;
			}

			if (ChairMode==NormalMode||ChairMode == TreatmentMode)//QA模式下不限速
			{
				if (single_move_speed_show[i] >= 1.f)//软件限速
				{
					single_move_speed_show[i] = 1.f;
				}
				else if (single_move_speed_show[i] <= 0.1f)//软件限速
				{
					single_move_speed_show[i] = 0.1f;
				}
			}
		}

		for (int i = 3; i < 6; i++)
		{
			if (attitude.mid[i] >= 200.f)
			{
				attitude.mid[i] = 200.f;
			}
			else if (attitude.mid[i] <= -200.f)
			{
				attitude.mid[i] = -200.f;
			}

			if (ChairMode == NormalMode || ChairMode == TreatmentMode)//在QA模式下不限速
			{
				if (single_move_speed_show[i] >= 10.f)
				{
					single_move_speed_show[i] = 10.f;
				}
				else if (single_move_speed_show[i] <= 0.1f)
				{
					single_move_speed_show[i] = 0.1f;
				}
			}
		}

		UpdateData(FALSE);//用变量的值更新控件上显示的数值

		for (int i = 0; i < 6; i++)
		{
			single_move_speed[i] = single_move_speed_show[i] * 3 * PI;
		}
	}
}

void CSTC1Dlg::OnBnClickedButton6()//初始化
{
	if (IDYES == MessageBox(_T("Determine to lift the device?"), _T("Notice"), MB_YESNO))
	{
		//六自由度平台
		m_nCmd = 6;
		Sleep(1000);
		this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
	}
}


void CSTC1Dlg::OnBnClickedButton10()//平台落下
{
	if (IDYES == MessageBox(_T("Determine to stop the current running state and stop the device?"), _T("Notice"), MB_YESNO))
	{
		m_tab.SetCurSel(0);
		m_mainui.ShowWindow(true);
		m_cali.ShowWindow(false);
		m_corr.ShowWindow(false);
		m_tab.EnableWindow(FALSE);
		m_mainui.GetDlgItem(IDC_UPBUTTON)->EnableWindow(FALSE); m_mainui.GetDlgItem(IDC_DOWNBUTTON)->EnableWindow(FALSE); 
		m_mainui.GetDlgItem(IDC_TREATMODEBUTTON)->EnableWindow(FALSE);
		//m_mainui.ShowWindow(TRUE); m_.ShowWindow(TRUE); m_mainui.ShowWindow(TRUE);
		SetDlgItemTextW(IDC_AZ360EDIT, L"-90");
		OnBnClickedCtrlbutton();

		this->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
		//this->GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
		//this->GetDlgItem(IDC_STOPBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_CTRLBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);
	
		m_nCmd = 7;
		ChairMode = NormalMode;

	}
}


void CSTC1Dlg::OnBnClickedButton9()//开始运行
{
	//if (IDYES == MessageBox(_T("Determine to start the  running state?"), _T("提示"), MB_YESNO))
	//{
		if ((m_dof.FromDOFBuf.nDOFStatus == 2) || (m_dof.FromDOFBuf.nDOFStatus == 3))
		{
			CanUse = TRUE;
			this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(TRUE);
			this->GetDlgItem(IDC_BUTTON10)->EnableWindow(TRUE);
			this->GetDlgItem(IDC_CTRLBUTTON)->EnableWindow(TRUE);

			m_tab.EnableWindow(TRUE);
			m_mainui.GetDlgItem(IDC_UPBUTTON)->EnableWindow(TRUE); m_mainui.GetDlgItem(IDC_DOWNBUTTON)->EnableWindow(TRUE);
			m_mainui.GetDlgItem(IDC_TREATMODEBUTTON)->EnableWindow(TRUE);
			this->GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_TAB1)->EnableWindow(TRUE);
			this->GetDlgItem(IDC_EStopBUTTON)->EnableWindow(TRUE);
			this->GetDlgItem(IDC_BUTTON13)->EnableWindow(TRUE);
			m_nCmd = 0;
		}
		else
		{
			MessageBox(L"设备尚未准备完成，请等待!");
		}
	//}
}

void CSTC1Dlg::OnBnClickedResetbutton()//复位
{
	if (IDYES == MessageBox(_T("Determine to stop the current running state and reset the device ? The device will go back to the run ready location"), _T("Notice"), MB_YESNO))
	{
		CanUse = false;
		//LR = false;
		m_nCmd = 2;
		m_dof.ToNeutral();
		for (int i = 0; i < 6; i++)
		{
			attitude.mid[i] = 0;
		}
		UpdateData(false);
		this->GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EStopBUTTON)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);

		/*CString m_hShiJiJiaoDu;
		double ang = 0;
		GetDlgItem(IDC_RAZ360EDIT)->GetWindowTextW(m_hShiJiJiaoDu);
		//m_hShiJiJiaoDu.Format(_T("4%f"), ang);
		ang = _wtof(m_hShiJiJiaoDu.GetBuffer());
		if (ang >5)//当位置大于5才能执行归零操作
		{
			if (EthLinkOK)
			{
				fangxiang = 0;
				cli::array<Object ^>^ bRD = gcnew cli::array<Object ^>(1);
				String^ StrReg = gcnew String("");
				if (enableflag)
				{
					//fangxiang = 0;
					//short Ret = 254;
					Ret = PLC->Bit_Set(ModbusTCP::TcpClient::PlcMemory::DQ, 2050, 0); //设置PLC M2线圈置位
					if (Ret != 0)
					{
						MessageBox(L"座椅归零故障！");
						return;
					}
					Ret = PLC->CmdRead(ModbusTCP::TcpClient::PlcMemory::HR, ModbusTCP::TcpClient::DataType::INT16, 4098, 1, bRD); //读PLC D2的数据，3535表示正常
					if (Ret == 0)
					{
						StrReg = bRD[0]->ToString();
						if (StrReg != "3535")
						{
							MessageBox(L"座椅归零读取数据故障！");
							return;
						}
						else
						{
							this->SetDlgItemTextW(IDC_AZ360EDIT, L"0");
						}
					}
					else
					{
						MessageBox(L"座椅归零读取数据异常！");
					}
				}
				else
				{
					MessageBox(L"系统未使能！");
				}
				delete bRD;
				delete StrReg;
			}
			else
			{
				MessageBox(L"PLC未连接");
			}
		}*/
	}
}

void CSTC1Dlg::OnBnClickedEstopbutton()
{
	if (EMSTOP == false)
	{
		if (!enableflag)
		{
			return;
		}
		EMSTOP = true;
		m_ESTOPcontrol.SetColor(BLACK,RED);
		//this->GetDlgItem(IDC_EStopBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EnBUTTON)->EnableWindow(FALSE);

		/*cli::array<Object ^>^ bRD = gcnew cli::array<Object ^>(1);
		String^ StrReg = gcnew String("");
		Ret = PLC->Bit_Set(ModbusTCP::TcpClient::PlcMemory::DQ, 2053, 0); //设置PLC M5线圈开
		if (Ret != 0)
		{
			MessageBox(L"停止故障！");
			return;
		}
		m_Lockcontrol.SetColor(BLACK, LTGRAY);
		lockflag = false;
		enableflag = false;
		m_Encontrol.SetColor(BLACK, LTGRAY);*/

		float re_attitude_for_show[6] = { 0 };

		for (int i = 0; i<6; i++)
		{
			re_attitude_for_show[i] = m_dof.FromDOFBuf.motor_code[i];//回传码值
		}
		//码值转换为六维度的分量
		m_dof.FromDOFBuf.motor_code[0] = re_attitude_for_show[4];//userdata.m_eDOF[4];
		m_dof.FromDOFBuf.motor_code[1] = re_attitude_for_show[5];//userdata.m_eDOF[5];
		m_dof.FromDOFBuf.motor_code[2] = re_attitude_for_show[0];
		m_dof.FromDOFBuf.motor_code[3] = re_attitude_for_show[1];
		m_dof.FromDOFBuf.motor_code[4] = re_attitude_for_show[2];//userdata.m_eDOF[2];
		m_dof.FromDOFBuf.motor_code[5] = re_attitude_for_show[3];//userdata.m_eDOF[3];

																 //解码
		SoluteAttitude(m_dof.FromDOFBuf.motor_code, re_attitude_for_show);
		//后三位需要乘以1000
		for (int i = 3; i < 6; i++)
		{
			re_attitude_for_show[i] = re_attitude_for_show[i] * 1000;
		}
		//将数值显示
		str.Format(L"%.2f", re_attitude_for_show[0]); this->SetDlgItemTextW(IDC_XEDIT, str);
		str.Format(L"%.2f", re_attitude_for_show[1]); this->SetDlgItemTextW(IDC_YEDIT, str);
		str.Format(L"%.2f", re_attitude_for_show[2]); this->SetDlgItemTextW(IDC_ZEDIT, str);
		str.Format(L"%.2f", re_attitude_for_show[3]); this->SetDlgItemTextW(IDC_AXEDIT, str);
		str.Format(L"%.2f", re_attitude_for_show[4]); this->SetDlgItemTextW(IDC_AYEDIT, str);
		str.Format(L"%.2f", re_attitude_for_show[5]); this->SetDlgItemTextW(IDC_AZEDIT, str);

		count0 = 0;
		mode.flag_count = F_SinMove;

		for (int i = 0; i<3; i++)
		{
			attitude.end[i] = dd.DOFs[i];
		}
		for (int i = 3; i<6; i++)
		{
			attitude.end[i] = dd.DOFs[i] * 1000.f;
		}

		UpdateData(TRUE);//用控件中的值更新对应的变量值
		for (int i = 0; i<3; i++)
		{
			if (attitude.mid[i] >= 20.f)//软件限位
			{
				attitude.mid[i] = 20.0f;
			}
			else if (attitude.mid[i] <= -20.f)//软件限位
			{
				attitude.mid[i] = -20.f;
			}

			if (single_move_speed_show[i] >= 2)//软件限速
			{
				single_move_speed_show[i] = 2;
			}
			else if (single_move_speed_show[i] <= 0.1)//软件限速
			{
				single_move_speed_show[i] = 0.1f;
			}
		}

		for (int i = 3; i<6; i++)
		{
			if (attitude.mid[i] >= 200.f)
			{
				attitude.mid[i] = 200.f;
			}
			else if (attitude.mid[i] <= -200.f)
			{
				attitude.mid[i] = -200.f;
			}

			if (single_move_speed_show[i] >= 10.f)
			{
				single_move_speed_show[i] = 10.f;
			}
			else if (single_move_speed_show[i] <= 0.1f)
			{
				single_move_speed_show[i] = 0.1f;
			}
		}
		UpdateData(FALSE);//用变量的值更新控件上显示的数值

		for (int i = 0; i<6; i++)
		{
			single_move_speed[i] = single_move_speed_show[i] * 3 * PI;
		}
		OnBnClickedEnbutton();
		SetDlgItemText(IDC_EStopBUTTON, L"Em-stop\nRecovery");
	}
	else if (EMSTOP==true)
	{
		//EMSTOP = false;
		this->GetDlgItem(IDC_EnBUTTON)->EnableWindow(TRUE);
		m_ESTOPcontrol.SetColor(BLACK,LTGRAY);
		OnBnClickedEnbutton();
		SetDlgItemText(IDC_EStopBUTTON, L"Emergency\nStop");
		
	}

	//m_pc2pc.IsCtrl2Pc=false;
}

void CSTC1Dlg::OnBnClickedEnbutton()//使能开关
{
	if (EthLinkOK)//是否连接
	{
		cli::array<Object ^>^ bRD = gcnew cli::array<Object ^>(1);
		String^ StrReg = gcnew String("");
		if (!enableflag)
		{
			Ret = PLC->Bit_Set(ModbusTCP::TcpClient::PlcMemory::DQ, 2048, 0); //设置PLC M0线圈开
			if (Ret != 0)
			{
				MessageBox(L"使能故障1！");
				return;
			}
			Ret = PLC->CmdRead(ModbusTCP::TcpClient::PlcMemory::HR, ModbusTCP::TcpClient::DataType::INT16, 4096, 1, bRD); //读PLC D0的数据，1515表示正常
			if (Ret == 0)
			{
				StrReg = bRD[0]->ToString();
				if (StrReg != "1515")
				{
					MessageBox(L"使能故障2！");
					return;
				}
				else
				{
					//if (friststart)
					//{
						//friststart = false;
						//this->GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
						GetDlgItemText(IDC_RAZ360EDIT, str);
						this->SetDlgItemTextW(IDC_AZ360EDIT, str);
					m_Encontrol.SetColor(BLACK, GREEN);
					this->GetDlgItem(IDC_EStopBUTTON)->EnableWindow(TRUE);
					//this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(TRUE);
					SetDlgItemText(IDC_EnBUTTON, L"Close En");
					enableflag = true;
					if (EMSTOP)
					{
						this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
						EMSTOP = false;
					}
				}
			}
			else
			{
				MessageBox(L"使能时数据读取异常！");
				return;
			}
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONGREEN);
			((CStatic*)GetDlgItem(IDC_EN_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_EN_STATIC, L"Enable Status:ON");
			EMSTOP = false;
		}
		else if (enableflag)
		{
			Ret = PLC->Bit_Reset(ModbusTCP::TcpClient::PlcMemory::DQ, 2048, 0); //设置PLC M0线圈置位
			if (Ret != 0)
			{
				MessageBox(L"关闭使能故障1！");
				return;
			}
			Ret = PLC->CmdRead(ModbusTCP::TcpClient::PlcMemory::HR, ModbusTCP::TcpClient::DataType::INT16, 4096, 1, bRD); //读PLC D0的数据，1515表示正常
			if (Ret == 0)
			{
				StrReg = bRD[0]->ToString();
				if (StrReg != "1010")
				{
					MessageBox(L"关闭使能故障2！");
					return;
				}
				m_Encontrol.SetColor(BLACK, LLTGRAY);
				this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
				this->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
				this->GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);
				this->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
				this->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
				SetDlgItemText(IDC_EnBUTTON, L"Open En");
				enableflag = false;
			}
			else
			{
				MessageBox(L"关闭使能数据读取异常！");
				return;
			}
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONGRAY);
			((CStatic*)GetDlgItem(IDC_EN_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_EN_STATIC, L"Enable Status : OFF");
		}
		delete bRD;
		delete StrReg;
	}
	else
	{
		MessageBox(L"PLC未连接，请重新连接系统后重试");
		//m_btnEnable.SetColor(BLACK, LLTGRAY);
		enableflag = false;
	}

}


void CSTC1Dlg::OnBnClickedLockbutton()//滑轨解锁
{
	Double NUM, NUM2;
	GetDlgItemText(IDC_RAZ360EDIT, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.2f", m_dof.FromDOFBuf.nDOFStatus);
	NUM2 = _wtof(str.GetBuffer());
	if (NUM == -90 && (NUM2 == 255 || NUM2 == 55))
	{

	}
	else if (NUM != -90)
	{
		AfxMessageBox(_T("make sure ISO=0°"));
		return;
	}
	else if (m_dof.FromDOFBuf.nDOFStatus != 55)
	{
		AfxMessageBox(_T("make sure chair At the lowest position!"));
		return;
	}
	if (IDYES == MessageBox(_T("Determine to unlock the Rail?"), _T("提示"), MB_YESNO))
	{
		if (EthLinkOK)
		{
			cli::array<Object ^>^ bRD = gcnew cli::array<Object ^>(1);
			String^ StrReg = gcnew String("");
			if (lockflag)
			{
				Ret = PLC->Bit_Set(ModbusTCP::TcpClient::PlcMemory::DQ, 2049, 0); //设置PLC M1线圈置位
				if (Ret != 0)
				{
					MessageBox(L"解锁故障！");
					return;
				}

				Ret = PLC->CmdRead(ModbusTCP::TcpClient::PlcMemory::HR, ModbusTCP::TcpClient::DataType::INT16, 4097, 1, bRD); //读PLC D1的数据
				if (Ret == 0)
				{
					StrReg = bRD[0]->ToString();
					if (StrReg != "2525")
					{
						Ret = PLC->Bit_Reset(ModbusTCP::TcpClient::PlcMemory::DQ, 2049, 0);
					}
					else
					{
						//m_btnLock.SetColor(BLACK, GREEN);
						lockflag = false;
						m_Lockcontrol.SetColor(BLACK, RED);
					}
				}
				else
				{
					MessageBox(L"解锁数据读取异常！");
					return;
				}
			}
			delete bRD;
			delete StrReg;
		}
		else
		{
			MessageBox(L"PLC未连接");
		}
	}
}

void CSTC1Dlg::CtrlSpeed()
{

	for (int i = 0; i < 6; i++)
	{
		if (i < 3)
		{
			single_move_speed_show[i] = 1;//运动速度赋值
		}
		else
		{
			single_move_speed_show[i] = 10;
		}
	}
	float num[6];
	float time[6];
	maxtime = 0;
	for (size_t i = 0; i < 6; i++)
	{
		num[i] = abs(speed[i] - attitude.mid[i]);
		time[i] = num[i] / single_move_speed_show[i];
	}
	
	for (size_t i = 0; i < 6; i++)
	{
		if (time[i]>maxtime)
		{
			maxtime = time[i];
		}
	}
	for (size_t i = 0; i < 3; i++)
	{
		single_move_speed_show[i] =  num[i] / maxtime;
		if (single_move_speed_show[i]<0.1)
		{
			single_move_speed_show[i] = 0.1;
		}
	}
	for (size_t i = 3; i < 6; i++)
	{
		single_move_speed_show[i] = num[i] / maxtime;
		if (single_move_speed_show[i] <2)
		{
			single_move_speed_show[i] = 2;
		}
	}
}

void CSTC1Dlg::ChangeUI()
{
	m_editFont.CreateFont(
		32,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("Arial"));                 // lpszFacename
	m_mainui.GetDlgItem(IDC_AXEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_AYEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_AZEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_RXEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_RYEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_RZEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_XEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_YEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_ZEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_RAXEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_RAYEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_RAZEDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_360EDIT)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_360EDIT2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_patientIDEDIT)->SetFont(&m_editFont);
	m_editFont.CreateFont(
		20,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("Arial"));
	m_mainui.GetDlgItem(IDC_STATICActual)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATICPlanned)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATIC10)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATIC11)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATIC12)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATICActual2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATICPlanned2)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATIC13)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATIC14)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATIC15)->SetFont(&m_editFont);
	
	
	//m_mainui.GetDlgItem(IDC_workflowSTATIC)->SetFont(&m_editFont);
	m_cali.GetDlgItem(IDC_QASTATIC)->SetFont(&m_editFont);
	m_corr.GetDlgItem(IDC_SETTINGSSTATIC)->SetFont(&m_editFont);


	m_editFont.CreateFont(
		18,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("Arial"));

	GetDlgItem(IDC_STARTBUTTON)->SetFont(&m_editFont);
	GetDlgItem(IDC_EnBUTTON)->SetFont(&m_editFont);
	GetDlgItem(IDC_LockBUTTON)->SetFont(&m_editFont);
	GetDlgItem(IDC_BUTTON6)->SetFont(&m_editFont);
	GetDlgItem(IDC_StepOnBUTTON)->SetFont(&m_editFont);
	GetDlgItem(IDC_RESETBUTTON)->SetFont(&m_editFont);
	GetDlgItem(IDC_BUTTON10)->SetFont(&m_editFont);
	GetDlgItem(IDC_EStopBUTTON)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_UPBUTTON)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_DOWNBUTTON)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_TREATMODEBUTTON)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_ALIGNERBUTTON)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_TREATFINISHBUTTON)->SetFont(&m_editFont);
		m_mainui.GetDlgItem(IDC_PLANCOMBO)->SetFont(&m_editFont);

	m_editFont.CreateFont(
		16,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("Arial"));
	m_mainui.GetDlgItem(IDC_patientname)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_patientid)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_patientsex)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_patientbirth)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_COMMLIST)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATIC16)->SetFont(&m_editFont);
	m_mainui.GetDlgItem(IDC_STATIC17)->SetFont(&m_editFont);

	//m_corr.onclick
	m_cali.PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON9, BN_CLICKED), NULL);

}


void CSTC1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	switch (nIDEvent)
	{
	case 0:					//实时读PLC内存数据
							//timerout();
		break;
	case 1:
		RealAgleRealTime();
		break;
	case 2:
		ActionTip();
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CSTC1Dlg::RealAgleRealTime()
{
	cli::array<Object ^>^ bRD = gcnew cli::array<Object ^>(1);
	//cli::array<Object ^>^ bRD2 = gcnew cli::array<Object ^>(1);
	String^ StrRegH = gcnew String("");
	String^ StrRegL = gcnew String("");
	String^ StrReg = gcnew String("");
	CString m_hShiJiJiaoDu;
	//short Ret;
	Ret = PLC->CmdRead(ModbusTCP::TcpClient::PlcMemory::HR, ModbusTCP::TcpClient::DataType::INT16, 5432, 1, bRD);
	if (Ret == 0)
	{
		StrRegL = bRD[0]->ToString();
		CString cstrL(StrRegL);
		CString cstrfloat;
		
		bool Dataflag = false;
		if (cstrL.Left(1) == "-")
		{
			Dataflag = true;
			cstrL.Remove('-');
		}

		int a2 = _ttoi(cstrL);
		int b = a2 % 100;
		a2 = a2 / 100;
		cstrL.Format(_T("%d"), a2);
		cstrfloat.Format(_T("%d"), b);
		if (cstrfloat.Left(1) == "-")
		{
			cstrfloat.Remove('-');
		}
		if (Dataflag)
		{
			m_hShiJiJiaoDu = L"-" + cstrL + "." + cstrfloat;
		}
		else
		{
			m_hShiJiJiaoDu = cstrL + "." + cstrfloat;
		}
		if (LR)
		{
			double NNN = _wtof(m_hShiJiJiaoDu.GetBuffer());
			NNN -= 0.5;
			m_hShiJiJiaoDu.Format(L"%.1f",NNN);
		}

		GetDlgItem(IDC_RAZ360EDIT)->SetWindowText(m_hShiJiJiaoDu);
	}
	
	Ret = PLC->CmdRead(ModbusTCP::TcpClient::PlcMemory::HR, ModbusTCP::TcpClient::DataType::INT16, 4116, 1, bRD); //读PLC D1的数据，1515表示正常
	if (Ret == 0)
	{
		StrRegH = bRD[0]->ToString();
		//float test2 = re_attitude_for_show[5];
		//CString str;
		//CString str2;
		//str2.Format(L"%.6f", test2);
		CString cstrL(StrRegH);
		//SetDlgItemText(IDC_INFOEDIT, cstrL);
		if (cstrL == "1030")
		{
			m_Lockcontrol.SetColor(BLACK, GREEN);
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONRED);
			((CStatic*)GetDlgItem(IDC_LOCK_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_LOCK_STATIC, L"Track Lock Status:Unlock");
			this->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
		}
		else if (cstrL == "-1")
		{
			m_Lockcontrol.SetColor(BLACK, LLTGRAY);
			if (lockflag == false)//确保只向一次
			{
				lockflag = true;
				PlaySound(_T("6531.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}		
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONGREEN);
			((CStatic*)GetDlgItem(IDC_LOCK_STA_STATIC))->SetIcon(m_hIcon);
			SetDlgItemText(IDC_LOCK_STATIC, L"Track Lock Status:Lock");
			if (enableflag&&m_dof.FromDOFBuf.nDOFStatus==55)
			{
				this->GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
			}		
		}

	}
	//}
	delete bRD;
	//delete bRD2;


	for (int i = 0; i<6; i++)
	{
		re_attitude_for_show[i] = m_dof.FromDOFBuf.motor_code[i];//回传码值
	}
	//码值转换为六维度的分量
	m_dof.FromDOFBuf.motor_code[0] = re_attitude_for_show[4];//userdata.m_eDOF[4];
	m_dof.FromDOFBuf.motor_code[1] = re_attitude_for_show[5];//userdata.m_eDOF[5];
	m_dof.FromDOFBuf.motor_code[2] = re_attitude_for_show[0];
	m_dof.FromDOFBuf.motor_code[3] = re_attitude_for_show[1];
	m_dof.FromDOFBuf.motor_code[4] = re_attitude_for_show[2];//userdata.m_eDOF[2];
	m_dof.FromDOFBuf.motor_code[5] = re_attitude_for_show[3];//userdata.m_eDOF[3];

															 //解码
	SoluteAttitude(m_dof.FromDOFBuf.motor_code, re_attitude_for_show);
	//补偿，校正返回值的误差

	//后三位需要乘以1000
	for (int i = 3; i < 6; i++)
	{
		re_attitude_for_show[i] = re_attitude_for_show[i] * 1000;
	}
	GetRealData(re_attitude_for_show);//校正差值
	//将数值显示
	str.Format(L"%.1f", re_attitude_for_show[0]); this->SetDlgItemTextW(IDC_RXEDIT, str); m_cali.SetDlgItemTextW(IDC_RXEDIT3, str);
	str.Format(L"%.1f", re_attitude_for_show[1]); this->SetDlgItemTextW(IDC_RYEDIT, str); m_cali.SetDlgItemTextW(IDC_RYEDIT3, str);
	str.Format(L"%.1f", re_attitude_for_show[2]); this->SetDlgItemTextW(IDC_RZEDIT, str); m_cali.SetDlgItemTextW(IDC_RZEDIT3, str);
	str.Format(L"%.1f", re_attitude_for_show[3]); this->SetDlgItemTextW(IDC_RAXEDIT, str); m_cali.SetDlgItemTextW(IDC_RAXEDIT3, str);
	str.Format(L"%.1f", re_attitude_for_show[4]); this->SetDlgItemTextW(IDC_RAYEDIT, str); m_cali.SetDlgItemTextW(IDC_RAYEDIT3, str);
	str.Format(L"%.1f", re_attitude_for_show[5]); this->SetDlgItemTextW(IDC_RAZEDIT, str); m_cali.SetDlgItemTextW(IDC_RAZEDIT3, str);
	m_cali.SetDlgItemTextW(IDC_RAZ360EDIT2, m_hShiJiJiaoDu);//IDC_RAZ360EDIT3

	//z轴偏移的反解码

	
	double offset[6];
	for (size_t i = 0; i < 6; i++)
	{
		offset[i]=0;
	}

	GetDlgItemText(IDC_AZ360EDIT, str);
	offset[2] = _wtof(str.GetBuffer());
	double reoffset[6]; for (size_t i = 0; i < 6; i++)
	{
		reoffset[i] = 0;
	}
	DWORD dwCopied = 0;
	CString szKeyValue;
	CString m_szFileName = L"./SettingsINI.ini";
	dwCopied = ::GetPrivateProfileString(L"360Correction", L"X", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	offset[3] = _wtof(szKeyValue.GetBuffer());
	dwCopied = ::GetPrivateProfileString(L"360Correction", L"Y", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	offset[4] = _wtof(szKeyValue.GetBuffer());
	szKeyValue.ReleaseBuffer();
	if (ChairMode==TreatmentMode)
	{
		for (size_t i = 0; i < 6; i++)
		{
			reoffset[i] = correctdatafor360[i];
		}

		//translation.CorrectionFor360Zaxis(offset, reoffset);
	}

	double vector2[6];
	float re_attitude_for_show2[6];
	
	for (size_t i = 0; i < 6; i++)
	{
		re_attitude_for_show2[i] = re_attitude_for_show[i]-ref[i]- reoffset[i];
	}

	translation.ReMatrixToSolve(re_attitude_for_show2, vector2);


	if (vector2[0]<0)
	{
		vector2[0] += 360;
	}
	if (vector2[1]<0)
	{
		vector2[1] += 360;
	}

	//CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针
	//CWnd *cwnd = this->GetWindow(GW_CHILD);//获取第一个子窗口指针，即Page1  

	//CWnd *cwnd1 = cwnd->GetWindow(GW_HWNDNEXT);//获取第一个子窗口后的窗口，即Page2  
	//cwnd->GetDlgItemText(IDC_ZEDIT2, str);//获取Page2上Text控件的值  

	str.Format(L"%.1f", vector2[0]); m_mainui.SetDlgItemTextW(IDC_RXEDIT2, str); if (m_radiogroup2==0)m_cali.SetDlgItemTextW(IDC_RXEDIT3, str);
	{

	}
	str.Format(L"%.1f", vector2[1]); m_mainui.SetDlgItemTextW(IDC_RYEDIT2, str); if (m_radiogroup2 == 0)m_cali.SetDlgItemTextW(IDC_RYEDIT3, str);
	vector2[2] += 270;
	CString str2;
	GetDlgItemText(IDC_RAZ360EDIT, str2);
	double n = _wtof(str2.GetBuffer());

	vector2[2] -= n;
	if ((vector2[2]>360))
	{
		vector2[2] -= 360;
		if (abs(vector2[2] - 360)<0.1)
		{
			vector2[2] = 0;
		}
	}

	str.Format(L"%.1f", vector2[2]);	  m_mainui.SetDlgItemTextW(IDC_RZEDIT2, str); if (m_radiogroup2 == 0)m_cali.SetDlgItemTextW(IDC_RZEDIT3, str);
	str.Format(L"%.2f", vector2[3] / 10); m_mainui.SetDlgItemTextW(IDC_RAXEDIT2, str); if (m_radiogroup2 == 0) { str.Format(L"%.2f", vector2[3]); m_cali.SetDlgItemTextW(IDC_RAXEDIT3, str); }
	str.Format(L"%.2f", vector2[4] / 10); m_mainui.SetDlgItemTextW(IDC_RAYEDIT2, str); if (m_radiogroup2 == 0) {str.Format(L"%.2f", vector2[4]); m_cali.SetDlgItemTextW(IDC_RAYEDIT3, str);}
	str.Format(L"%.2f", vector2[5] / 10); m_mainui.SetDlgItemTextW(IDC_RAZEDIT2, str); if (m_radiogroup2 == 0) {str.Format(L"%.2f", vector2[5]); m_cali.SetDlgItemTextW(IDC_RAZEDIT3, str);}
	if (REset)
	{
		//REset = false;
		LR = false;
		CString m_hShiJiJiaoDu;
		double ang = 0;
		GetDlgItem(IDC_RAZ360EDIT)->GetWindowTextW(m_hShiJiJiaoDu);
		//m_hShiJiJiaoDu.Format(_T("4%f"), ang);
		ang = _wtof(m_hShiJiJiaoDu.GetBuffer());
		if (abs(ang - 5.5)<0.6)//当位置ga位于5才能执行归零操作
		{
			if (EthLinkOK)
			{
				cli::array<Object ^>^ bRD = gcnew cli::array<Object ^>(1);
				String^ StrReg = gcnew String("");
				if (enableflag)
				{
					//fangxiang = 0;
					//short Ret = 254;
					Ret = PLC->Bit_Set(ModbusTCP::TcpClient::PlcMemory::DQ, 2050, 0); //设置PLC M2线圈置位
					if (Ret != 0)
					{
						MessageBox(L"座椅归零故障！");
						return;
					}
					else
					{
						REset = false;
					}
					Ret = PLC->CmdRead(ModbusTCP::TcpClient::PlcMemory::HR, ModbusTCP::TcpClient::DataType::INT16, 4098, 1, bRD); //读PLC D2的数据，3535表示正常
					if (Ret == 0)
					{
						StrReg = bRD[0]->ToString();
						if (StrReg != "3535")
						{
							MessageBox(L"座椅归零读取数据故障！");
							return;
						}
						else
						{
							this->SetDlgItemTextW(IDC_AZ360EDIT, L"0");
						}
					}
					else
					{
						MessageBox(L"座椅归零读取数据异常！");
					}
				}
				else
				{
					MessageBox(L"系统未使能！");
				}
				delete bRD;
				delete StrReg;
			}
			else
			{
				MessageBox(L"PLC未连接");
			}
		}
	}
}

CString  CSTC1Dlg::ToHdata(CString Pdata)
{
	INT32 a = _ttoi(Pdata);
	CString str;
	int b = a / 65536;
	if (m_hJueDuiWeiZhi.Left(1) == "-")
	{
		b = b - 1;
	}
	str.Format(_T("%d"), b);
	return str;
}
CString  CSTC1Dlg::ToLdata(CString Pdata)
{
	INT32 a = _ttoi(Pdata);
	if (a > fangxiang)
	{
		fangxiang = a;
		a = a + 50;				//顺转时补偿0.5度
		CString str;
		str.Format(_T("%d"), fangxiang);
		SetDlgItemText(IDC_STATIC3, str);
		if (a != 0)
		{
			LR = true;
		}
	}
	else
	{
		fangxiang = a;
		CString str;
		str.Format(_T("%d"), fangxiang);
		SetDlgItemText(IDC_STATIC3, str);
		LR = false;
	}
	CString str;
	int b = a % 65536;
	str.Format(_T("%d"), b);
	SetDlgItemText(IDC_STATIC2, str);
	return str;

}

void CSTC1Dlg::ActionTip()
{
	JudgeAction();
	if (ChairMode!=TreatmentMode)
	{
		for (size_t i = 0; i < 6; i++)
		{
			IfMoveInPlace[i] = 1;
		}
	}
	if (::GetKeyState(VK_CONTROL) < 0)//按下
	{
		//AfxMessageBox(L"Ctrl 键按下了。");
		if (DoingTreatMoving == 0)
		{
			DoingTreatMoving = 1;
			m_mainui.REOnBnClickedCorrbutton();
		}
	}
	else//未按下
	{
		//AfxMessageBox(L"Ctrl 键没按下。");
		if (DoingTreatMoving==1)
		{
			DoingTreatMoving = 0;
			OnBnClickedEstopbutton();
			Sleep(200);
			OnBnClickedEstopbutton();
		}
	}

	if (IfMoveInPlace[0]==1&& IfMoveInPlace[1] == 1 && IfMoveInPlace[2] == 1 && 
		IfMoveInPlace[3] == 1 && IfMoveInPlace[4] == 1 && IfMoveInPlace[5] == 1)
	{
		HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONGREEN);
		((CStatic*)GetDlgItem(IDC_POSLOCK_STA_STATIC))->SetIcon(m_hIcon);
		SetDlgItemText(IDC_POS_STATIC, L"Postrue Lock Status : Locking");
		KillTimer(2);
		str.Format(L"%.1f", TTS_coordinate_value[0]); m_mainui.SetDlgItemTextW(IDC_XEDIT2, str);
		str.Format(L"%.1f", TTS_coordinate_value[1]); m_mainui.SetDlgItemTextW(IDC_YEDIT2, str);
		double A360Z;
		GetDlgItemText(IDC_AZ360EDIT, str);  A360Z = _wtof(str.GetBuffer());
		double NNN = TTS_coordinate_value[2] - A360Z;
		str.Format(L"%.1f", TTS_coordinate_value[6]); m_mainui.SetDlgItemTextW(IDC_ZEDIT2, str);
		str.Format(L"%.2f", TTS_coordinate_value[3] / 10); m_mainui.SetDlgItemTextW(IDC_AXEDIT2, str);
		str.Format(L"%.2f", TTS_coordinate_value[4] / 10); m_mainui.SetDlgItemTextW(IDC_AYEDIT2, str);
		str.Format(L"%.2f", TTS_coordinate_value[5] / 10); m_mainui.SetDlgItemTextW(IDC_AZEDIT2, str);
		//IsTreatMoving = false;
		DoingTreatMoving = 0;
	}
	else
	{
		//IsTreatMoving = true;
		HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONRED);
		((CStatic*)GetDlgItem(IDC_POSLOCK_STA_STATIC))->SetIcon(m_hIcon);
		SetDlgItemText(IDC_POS_STATIC, L"Postrue Lock Status : Moving");
	}
	
}
void CSTC1Dlg::JudgeAction()
{
	double NBB[7];
	m_mainui.GetDlgItemText(IDC_RXEDIT2, str); NBB[0] = _wtof(str.GetBuffer());
	m_mainui.GetDlgItemText(IDC_RYEDIT2, str); NBB[1] = _wtof(str.GetBuffer());
	m_mainui.GetDlgItemText(IDC_RZEDIT2, str); NBB[2] = _wtof(str.GetBuffer());
	//GetDlgItemText(IDC_RAZ360EDIT, str); NBB[6] = _wtof(str.GetBuffer());
	m_mainui.GetDlgItemText(IDC_RAXEDIT2, str); NBB[3] = _wtof(str.GetBuffer())*10;
	m_mainui.GetDlgItemText(IDC_RAYEDIT2, str); NBB[4] = _wtof(str.GetBuffer())*10;
	m_mainui.GetDlgItemText(IDC_RAZEDIT2, str); NBB[5] = _wtof(str.GetBuffer())*10;
	for (size_t i = 0; i < 6; i++)
	{
		if (i==2)
		{
			if ((abs(NBB[i] - TTS_coordinate_value[6]) < 0.3) || (abs(NBB[i] - 360 - TTS_coordinate_value[6]) < 0.3))
			{
				IfMoveInPlace[i] = 1;
			}
			else { IfMoveInPlace[i] = 0; }
			/*double A360Z;
			GetDlgItemText(IDC_AZ360EDIT, str);  A360Z = _wtof(str.GetBuffer());
			double NNN = TTS_coordinate_value[2] - A360Z;
			if (NNN>360){NNN -= 360;}
			if (NNN<0) { NNN += 360; }
			if(abs(NNN-NBB[2])<0.6)
			{
				IfMoveInPlace[i] = 1;
			}
			else { IfMoveInPlace[i] = 0; }*/
		}
		else {
			if ((abs( NBB[i]- TTS_coordinate_value[i]) < 0.3)|| (abs(NBB[i]-360 - TTS_coordinate_value[i]) < 0.3))
			{
				IfMoveInPlace[i] = 1;
			}
			else { IfMoveInPlace[i] = 0; }
		}
	}
	//return 1;
}


void CSTC1Dlg::OnBnClickedLogoutbutton()
{
	m_tab.SetCurSel(0);
	m_mainui.ShowWindow(true);
	m_cali.ShowWindow(false);
	m_corr.ShowWindow(false);

	CLogin m_login; m_login.CanClose = false;
	//m_login.GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	if (m_login.DoModal() == IDOK)
	{
		if (permission=="3")
		{
			m_mainui.ShowWindow(true);
			m_cali.ShowWindow(false);
			m_corr.ShowWindow(false);
			this->GetDlgItem(IDC_TAB1)->EnableWindow(TRUE);
		}
		else if (permission == "2")
		{
			this->GetDlgItem(IDC_TAB1)->EnableWindow(TRUE);
		}
		else if (permission == "1")
		{
			m_mainui.ShowWindow(true);
			m_cali.ShowWindow(false);
			m_corr.ShowWindow(false);
			this->GetDlgItem(IDC_TAB1)->EnableWindow(FALSE);
		}
		SetWindowText(L"RCTS v0.2   UserName: " + UserName + "  Permission: " + permission);
	}
	else
	{
		//CDialogEx::OnCancel();
	}
	//m_login.GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void show_description(HPDF_Page          page,
	HPDF_REAL          x,
	HPDF_REAL          y,
	const char   *text)
{
	float fsize = HPDF_Page_GetCurrentFontSize(page);
	HPDF_Font font = HPDF_Page_GetCurrentFont(page);
	HPDF_RGBColor c = HPDF_Page_GetRGBFill(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetRGBFill(page, 0, 0, 0);
	HPDF_Page_SetTextRenderingMode(page, HPDF_FILL);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_TextOut(page, x, y - 12, text);
	HPDF_Page_EndText(page);

	HPDF_Page_SetFontAndSize(page, font, fsize);
	HPDF_Page_SetRGBFill(page, c.r, c.g, c.b);
}

void CSTC1Dlg::OnBnClickedPdfbutton()
{
	if (PisOpen)//若当前有打开的计划，则加载数据库内的数据
	{
		OnStnClickedEnStatic();
	}
	CString defaultDir = L"A:\\FileTest";   //默认打开的文件路径  
	CString fileName = L"test.pdf";         //默认打开的文件名  
	CString filter = L"文件 (*.pdf)|*.pdf||";   //文件过虑的类型  
	CFileDialog openFileDlg(TRUE, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
	//openFileDlg.GetOFN().lpstrInitialDir = L"E:\\FileTest\\test.doc";
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = defaultDir + "\\" + fileName;
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();

		if (PathFileExists(filePath))
		{
			if (IDYES == MessageBox(_T("The file of the same name already exists,Whether or not covered?"), _T("提示"), MB_YESNO))
			{
			}
			else
			{
				return;
			}
		}
		else
		{
			//return FALSE;
		}
	
	}
	else {
		return;
	}

	const wchar_t* wstr = (LPCTSTR)filePath;         //一定得是unicode，否则这句话会错的
	char c[256] = { 0 }; char c1[256] = { 0 };
	wcstombs(c, wstr, wcslen(wstr));
	const char* fname = c;

	const char *page_title = "Chair System Treatment Report";

	HPDF_Doc  pdf;
	HPDF_Font font, font2;
	HPDF_Page page;
	//char fname[256];
	float height = 0; float width = 0;
	int hpos=0;


	//strcpy(fname, "D:\\新建文件夹\\");
	//strcat(fname, "pdftest");
	//strcat(fname, ".pdf");

	pdf = HPDF_New(nullptr, nullptr);
	if (!pdf)
	{
		MessageBox(_T("error: cannot create PdfDoc object\n"));
		return;
	}

	/* set compression mode */
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
	HPDF_STATUS status;
	HPDF_UseCNSFonts(pdf);
	HPDF_UseCNSEncodings(pdf);
	/* create default-font */
	font = HPDF_GetFont(pdf, "SimHei","GBK-EUC-H");
	font2 = HPDF_GetFont(pdf, "Helvetica", NULL);

	/* add a new page object. */
	page = HPDF_AddPage(pdf);

	status = HPDF_Page_SetFontAndSize(page, font, 30);
	float tw = HPDF_Page_TextWidth(page, page_title);
	status = HPDF_Page_BeginText(page);
	//show_description(page, 60, ypos - 50,
	//"RenderingMode=PDF_STROKE");
	width = HPDF_Page_GetWidth(page); height = HPDF_Page_GetHeight(page);
	HPDF_Page_SetTextRenderingMode(page, HPDF_FILL_CLIPPING);
	hpos = 40;
	status = HPDF_Page_TextOut(page, (width - tw) / 2,height - hpos, page_title);

	status = HPDF_Page_SetFontAndSize(page, font, 20);
	tw = HPDF_Page_TextWidth(page, "SHANGHAI");
	hpos += 30; status = HPDF_Page_TextOut(page, width - tw - 50, height - hpos, "SHANGHAI");

	status = HPDF_Page_SetFontAndSize(page, font, 12);
	tw = HPDF_Page_TextWidth(page, "Shanghai Proton and Heavy Ion Center");
	status = HPDF_Page_TextOut(page, width - tw - 50,height - 85, "Shanghai Proton and Heavy Ion Center");
	tw = HPDF_Page_TextWidth(page, "4365 Kangxin Road, Pudong");
	status = HPDF_Page_TextOut(page, width - tw - 50,height - 100, "4365 Kangxin Road, Pudong");
	tw = HPDF_Page_TextWidth(page, "20171227 Shanghai");
	status = HPDF_Page_TextOut(page, width - tw - 50,height - 115, "20171227 Shanghai");
	tw = HPDF_Page_TextWidth(page, "China");
	status = HPDF_Page_TextOut(page, width - tw - 50,height - 130, "China");

	status = HPDF_Page_TextOut(page, 50,height - 70, "Name           :"); wstr = (LPCTSTR)m_report.PatientName; for (int i = 0; i < 256; i++) { c1[i] = 0; }; wcstombs(c1, wstr, wcslen(wstr)); status = HPDF_Page_TextOut(page, 150, height - 70, c1);
	status = HPDF_Page_TextOut(page, 50,height - 90, "Gender         :"); wstr = (LPCTSTR)m_report.PatientGender; for (int i = 0; i < 256; i++) { c1[i] = 0; }; wcstombs(c1, wstr, wcslen(wstr)); status = HPDF_Page_TextOut(page, 150, height - 90, c1);
	status = HPDF_Page_TextOut(page, 50,height - 110, "Date of birth  :"); wstr = (LPCTSTR)m_report.PatientBirth; for (int i = 0; i < 256; i++) { c1[i] = 0; }; wcstombs(c1, wstr, wcslen(wstr)); status = HPDF_Page_TextOut(page, 150, height - 110, c1);
	status = HPDF_Page_TextOut(page, 50,height - 130, "ID             :"); wstr = (LPCTSTR)m_report.PatientID; for (int i = 0; i < 256; i++) { c1[i] = 0; }; wcstombs(c1, wstr, wcslen(wstr)); status = HPDF_Page_TextOut(page, 150, height - 130, c1);

	status = HPDF_Page_TextOut(page, 50,height - 160, "Fraction sequence  :");
	status = HPDF_Page_TextOut(page, 50,height - 180, "Treatment plan     :"); wstr = (LPCTSTR)m_report.PlanName; for (int i = 0; i < 256; i++) { c1[i] = 0; }; wcstombs(c1, wstr, wcslen(wstr)); status = HPDF_Page_TextOut(page, 200, height - 180, c1);
	status = HPDF_Page_SetFontAndSize(page, font, 10);
	status = HPDF_Page_TextOut(page, 50,height - 190, "----------------------------------------------------------------------------------------------------");
	status = HPDF_Page_TextOut(page, 50, height - 192, "----------------------------------------------------------------------------------------------------");
	
	//m_report.BeamNum=18;
	int P_BeamNum = 0;
	if (m_report.BeamNum>7)
	{
		for (size_t i = 0; i < 8; i++)
		{
			wstr = (LPCTSTR)m_report.BeamName[i]; for (int i = 0; i < 256; i++){c1[i] = 0;}; wcstombs(c1, wstr, wcslen(wstr));
			status = HPDF_Page_TextOut(page, 50, height - 210 - i*70, c1);
			wstr = (LPCTSTR)m_report.BeamTime[i]; for (int i = 0; i < 256; i++){c1[i] = 0;}; wcstombs(c1, wstr, wcslen(wstr));
			status = HPDF_Page_TextOut(page, width / 2, height - 210 - i*70, c1);
			status = HPDF_Page_TextOut(page, 100, height - 225 - i*70, "Lat/Long/Vert[cm]:");
			sprintf(c1, "%.2f(%.2f) / %.2f(%.2f) / %.2f(%.2f)", m_report.PlanBeamInfo[i][3], m_report.ReBeamInfo[i][3], m_report.PlanBeamInfo[i][4], m_report.ReBeamInfo[i][4], m_report.PlanBeamInfo[i][5], m_report.ReBeamInfo[i][5]); 
			status = HPDF_Page_TextOut(page, 250, height - 225 - i*70, c1);
			status = HPDF_Page_TextOut(page, 100, height - 240 - i*70, "Iso/Pitch/Roll[°]:");
			sprintf(c1, "%.2f(%.2f) / %.2f(%.2f) / %.2f(%.2f)", m_report.PlanBeamInfo[i][0], m_report.ReBeamInfo[i][0], m_report.PlanBeamInfo[i][1], m_report.ReBeamInfo[i][1], m_report.PlanBeamInfo[i][2], m_report.ReBeamInfo[i][2]);
			status = HPDF_Page_TextOut(page, 250, height - 240 - i*70, c1);
			status = HPDF_Page_TextOut(page, 100, height - 255 - i*70, "Perm.to treat"); 
			tw = HPDF_Page_TextWidth(page, "Perm.to treat");
			wstr = (LPCTSTR)m_report.BeamOper[i]; for (int i = 0; i < 256; i++){c1[i] = 0;}; wcstombs(c1, wstr, wcslen(wstr));
			status = HPDF_Page_TextOut(page, tw + 110, height - 255 - i*70, c1);
			status = HPDF_Page_TextOut(page, 50, height - 270 - i*70, "----------------------------------------------------------------------------------------------------");
		}		
		P_BeamNum = 7;
		int li= (m_report.BeamNum+2)/ 10;//需要添加的页数
		for (int pi = 0; pi < li; pi++)
		{	
			status = HPDF_Page_EndText(page);
			page = HPDF_AddPage(pdf);
			status = HPDF_Page_BeginText(page);
			status = HPDF_Page_SetFontAndSize(page, font, 12);
			status = HPDF_Page_TextOut(page, 50, height - 50, "----------------------------------------------------------------------------------------------------");
			int lim;
			if (pi == (li-1))
			{
				lim = m_report.BeamNum;
			}
			else 
			{
				lim = P_BeamNum + 10;
			}
			for (size_t i = P_BeamNum; i < lim; i++)
			{
				wstr = (LPCTSTR)m_report.BeamName[i]; for (int i = 0; i < 256; i++){c1[i] = 0;}; wcstombs(c1, wstr, wcslen(wstr));
				status = HPDF_Page_TextOut(page, 50, height - 80 - (i - P_BeamNum) *70, c1);
				wstr = (LPCTSTR)m_report.BeamTime[i]; for (int i = 0; i < 256; i++){c1[i] = 0;}; wcstombs(c1, wstr, wcslen(wstr));
				status = HPDF_Page_TextOut(page, width / 2, height - 80 - (i - P_BeamNum) *70, c1);
				status = HPDF_Page_TextOut(page, 100, height - 95 - (i - P_BeamNum) *70, "Lat/Long/Vert[cm]:");
				sprintf(c1, "%.2f(%.2f) / %.2f(%.2f) / %.2f(%.2f)", m_report.PlanBeamInfo[i][3], m_report.ReBeamInfo[i][3], m_report.PlanBeamInfo[i][4], m_report.ReBeamInfo[i][4], m_report.PlanBeamInfo[i][5], m_report.ReBeamInfo[i][5]);
				status = HPDF_Page_TextOut(page, 250, height - 95 - (i - P_BeamNum) *70, c1);
				status = HPDF_Page_TextOut(page, 100, height - 110 - (i - P_BeamNum) *70, "Iso/Pitch/Roll[°]:");
				sprintf(c1, "%.2f(%.2f) / %.2f(%.2f) / %.2f(%.2f)", m_report.PlanBeamInfo[i][0], m_report.ReBeamInfo[i][0], m_report.PlanBeamInfo[i][1], m_report.ReBeamInfo[i][1], m_report.PlanBeamInfo[i][2], m_report.ReBeamInfo[i][2]);
				status = HPDF_Page_TextOut(page, 250, height - 110 - (i - P_BeamNum) *70, c1);
				status = HPDF_Page_TextOut(page, 100, height - 125 - (i - P_BeamNum) *70, "Perm.to treat");
				tw = HPDF_Page_TextWidth(page, "Perm.to treat");
				wstr = (LPCTSTR)m_report.BeamOper[i]; for (int i = 0; i < 256; i++){c1[i] = 0;}; wcstombs(c1, wstr, wcslen(wstr));
				status = HPDF_Page_TextOut(page, tw + 110, height - 125 - (i - P_BeamNum) *70, c1);
				status = HPDF_Page_TextOut(page, 50, height - 140 - (i - P_BeamNum) *70, "----------------------------------------------------------------------------------------------------");
			
				hpos = height - 150 - (i - P_BeamNum) *70;
			}
			P_BeamNum += 10;		
		}
	}
	else
	{
		for (int i = 0; i <m_report.BeamNum+1; i++)
		{		
			//m_report.BeamName[0] = L"Beam001"; //BeamTime[0] = "20171227"; BeamOper[0] = "1000";
			wstr = (LPCTSTR)m_report.BeamName[i];for (int i = 0; i < 256; i++){c1[i] = 0;};wcstombs(c1, wstr, wcslen(wstr));
			status = HPDF_Page_TextOut(page, 100,height - 210 - i * 70, c1);
			wstr = (LPCTSTR)m_report.BeamTime[i]; for (int i = 0; i < 256; i++){c1[i] = 0;}; wcstombs(c1, wstr, wcslen(wstr));
			status = HPDF_Page_TextOut(page, width / 2,height - 210 - i*70, c1);
			status = HPDF_Page_TextOut(page, 150,height - 225 - i*70, "Lat/Long/Vert[cm]:");
			sprintf(c1, "%.2f(%.2f) / %.2f(%.2f) / %.2f(%.2f)", m_report.PlanBeamInfo[i][3], m_report.ReBeamInfo[i][3], m_report.PlanBeamInfo[i][4], m_report.ReBeamInfo[i][4], m_report.PlanBeamInfo[i][5], m_report.ReBeamInfo[i][5]); 
			status = HPDF_Page_TextOut(page, 300, height - 225 - i*70, c1);
			status = HPDF_Page_TextOut(page, 150,height - 240 - i*70, "Iso/Pitch/Roll[°]:");
			sprintf(c1, "%.2f(%.2f) / %.2f(%.2f) / %.2f(%.2f)", m_report.PlanBeamInfo[i][0], m_report.ReBeamInfo[i][0], m_report.PlanBeamInfo[i][1], m_report.ReBeamInfo[i][1], m_report.PlanBeamInfo[i][2], m_report.ReBeamInfo[i][2]);
			status = HPDF_Page_TextOut(page, 300, height - 240 - i*70, c1);
			status = HPDF_Page_TextOut(page, 100,height - 255 - i*70, "Perm.to treat");
			tw = HPDF_Page_TextWidth(page, "Perm.to treat");
			wstr = (LPCTSTR)m_report.BeamOper[i]; for (int i = 0; i < 256; i++){c1[i] = 0;}; wcstombs(c1, wstr, wcslen(wstr));
			status = HPDF_Page_TextOut(page, tw + 160,height - 255 - i*70, c1);
			status = HPDF_Page_TextOut(page, 50,height - 270 - i*70, "----------------------------------------------------------------------------------------------------");
		}
		hpos = height - 270 - m_report.BeamNum * 70;
		/*if (m_report.BeamNum==2)
		{
			status = HPDF_Page_EndText(page);
			//HPDF_Page page2;
			page = HPDF_AddPage(pdf);
			status = HPDF_Page_BeginText(page);
			status = HPDF_Page_SetFontAndSize(page, font, 12);
			hpos = height -150;
		}*/
		
	}

	status = HPDF_Page_TextOut(page, 50, hpos-2, "----------------------------------------------------------------------------------------------------");
	status = HPDF_Page_TextOut(page, 50, hpos - 10, "Fraction :");
	status = HPDF_Page_TextOut(page, width / 3, hpos - 10, "Date :");
	CTime tm; tm = CTime::GetCurrentTime(); 
	int year = tm.GetYear(); int month = tm.GetMonth(); int day = tm.GetDay(); int hour = tm.GetHour(); int minute = tm.GetMinute(); ///分钟
	char time[200];sprintf(time, "%d/%d/%d ", year,month,day);
	status = HPDF_Page_SetFontAndSize(page, font2, 12); HPDF_Page_SetRGBFill(page, 0.2, 0.2, 0.2);
	status = HPDF_Page_TextOut(page, width / 3+35, hpos - 10,time );
	status = HPDF_Page_SetFontAndSize(page, font, 12); HPDF_Page_SetRGBFill(page, 0, 0, 0);
	status = HPDF_Page_TextOut(page, width *2/ 3, hpos - 10, "Room :  Room4");
	status = HPDF_Page_TextOut(page, 50, hpos - 30, "Status :");
	status = HPDF_Page_TextOut(page, width/2, hpos - 30, "Therapist :"); 
	wstr = (LPCTSTR)UserName; for (int i = 0; i < 256; i++) { c1[i] = 0; }; wcstombs(c1, wstr, wcslen(wstr)); status = HPDF_Page_TextOut(page, width / 2+80, hpos - 30, c1);
	//status = HPDF_Page_TextOut(page, 50, hpos - 35, "----------------------------------------------------------------------------------------------------");

	status = HPDF_Page_EndText(page);
	
	/* save the document to a file */
	HPDF_SaveToFile(pdf, fname);
	/* clean up */
	HPDF_Free(pdf);
	MessageBox(_T("Save successful！\n"));
}

void CSTC1Dlg::On32775()
{
	OnBnClickedLogoutbutton();
	//OnBnClickedPdfbutton;
}

void CSTC1Dlg::OnSavereport()
{
	OnBnClickedPdfbutton();
}

void CSTC1Dlg::OnSoftwareinfo()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CSTC1Dlg::OnMenuExit()
{
	if (ChairMode == TreatmentMode)
	{
		AfxMessageBox(_T("In treatment mode, if you want to shut down the software, please finish the treatment first."));
		return;
	}
	if (IDYES == MessageBox(_T("Do you want to exit program？Please confirm that the current device is in a safe state"), _T("WARNING!"), MB_YESNO))
	{
		bool canclose = true;
		for (size_t i = 0; i < 6; i++)
		{
			if (abs(re_attitude_for_show[i]) > 1)
			{
				canclose = false;
			}
		}
		if (((m_dof.FromDOFBuf.nDOFStatus != 55) && (!canclose) && EthLinkOK))
		{
			AfxMessageBox(_T("当前治疗椅状态不能关闭！"));
			return;
		}
		CString m_szFileName = L"./SettingsINI.ini";
		CString szKeyValue;
		szKeyValue.Format(L"%.0f", fangxiang);
		::WritePrivateProfileString(L"Open", L"fangxiang", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
		if (LR)
		{
			szKeyValue.Format(L"%d", 1);
			::WritePrivateProfileString(L"Open", L"LR", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
		}
		else
		{
			szKeyValue.Format(L"%d", 0);
			::WritePrivateProfileString(L"Open", L"LR", szKeyValue.GetBuffer(MAX_PATH), m_szFileName);
		}
		EthLinkOK = false;
		enableflag = false;
		::timeKillEvent(nMMTimer);
		KillTimer(0);
		KillTimer(1);
		nMMTimer = 0;
		PLC->DeLink();
		CDialogEx::OnCancel();
	}
}


void CSTC1Dlg::OnStnClickedEnStatic()//加载数据库内的治疗数据的函数,在其他函数内保存
{
	mysql_init(&m_sqlCon);//初始化数据库对象
	if (!mysql_real_connect(&m_sqlCon, "localhost", "root", "123456", "chair001", 3306, NULL, 0))//localhost:服务器地址，可以直接填入IP;root:账号;123:密码;test:数据库名;3306:网络端口  
	{
		AfxMessageBox(_T("Database connection failed!"));
		return;
	}
	else//连接成功则继续访问数据库，之后的相关操作代码基本是放在这里面的
	{
		//AfxMessageBox(_T("数据库连接成功!"));
	}
	//查询数据
	/*(mysql_query(&m_sqlCon, "SELECT id FROM patient"));//从users这个表格查询id
	m_res = mysql_store_result(&m_sqlCon);
	while (m_row = mysql_fetch_row(m_res))
	{
		str = m_row[0];
		if (_wtof(m_report.PatientID.GetBuffer()) == _wtof(str.GetBuffer()))
		{
		}
	}*/
	str.Format(L"SELECT TreatTimes FROM treatmentreport where PlanName = \"%s\"", m_report.PlanName);
	const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
	char c1[500] = { 0 };
	wcstombs(c1, wstr, wcslen(wstr));
	const char* cc1 = c1;
	mysql_query(&m_sqlCon, cc1);//从users这个表格查询密码"SELECT password FROM users where id = 1000"								//获取结果集
	m_res = mysql_store_result(&m_sqlCon);
	m_row = mysql_fetch_row(m_res);
	str = m_row[0];
	m_report.BeamNum = _wtof(str.GetBuffer())-1;//获取数据库内已治疗过的Beam数据数量
	mysql_close(&m_sqlCon);//关闭Mysql连接
	OnBnClickedButton2();
	//mysql_init(&m_sqlCon);//初始化数据库对象
	/*for (int i = 0; i < m_report.BeamNum; i++)
	{
		str.Format(L"SELECT B%d FROM treatmentreport where PlanName = \"%s\"",i+1, m_report.PlanName);
		wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
		c1[500] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		cc1 = c1;
		mysql_query(&m_sqlCon, cc1);
		m_res = mysql_store_result(&m_sqlCon);
		m_row = mysql_fetch_row(m_res);
		str = m_row[0]; wstr = (LPCTSTR)str;
		c1[500] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		CString beamdata[14];
		for (int n = 0; n < 14; n++)
		{
			if (c1[n] == 0x2C)//逗号
			{	
				beamdata[n] = result;
				result = L"";
			}
			else
			{
				result += c1[n];
			}
		}
		m_report.ReBeamInfo[i][0] = _wtof(beamdata[0].GetBuffer()); m_report.PlanBeamInfo[i][0] = _wtof(beamdata[1].GetBuffer());
		m_report.ReBeamInfo[i][1] = _wtof(beamdata[2].GetBuffer()); m_report.PlanBeamInfo[i][1] = _wtof(beamdata[3].GetBuffer());
		m_report.ReBeamInfo[i][2] = _wtof(beamdata[4].GetBuffer()); m_report.PlanBeamInfo[i][2] = _wtof(beamdata[5].GetBuffer());
		m_report.ReBeamInfo[i][3] = _wtof(beamdata[6].GetBuffer()); m_report.PlanBeamInfo[i][3] = _wtof(beamdata[7].GetBuffer());
		m_report.ReBeamInfo[i][4] = _wtof(beamdata[8].GetBuffer()); m_report.PlanBeamInfo[i][4] = _wtof(beamdata[9].GetBuffer());
		m_report.ReBeamInfo[i][5] = _wtof(beamdata[10].GetBuffer()); m_report.PlanBeamInfo[i][5] = _wtof(beamdata[11].GetBuffer());
		m_report.BeamOper[i] = beamdata[12]; m_report.BeamTime[i] = beamdata[13];
	}
	mysql_close(&m_sqlCon);//关闭Mysql连接
	*/
}


void CSTC1Dlg::OnEditResetpassword()
{
	CLogin m_login;
	if (m_login.DoModal() == IDOK)
	{

	}
	else
	{
		return;
	}
	LoginSetPass m_login2;
	//m_login2.ChangePass = true;
	if (m_login2.DoModal() == IDOK)
	{
		AfxMessageBox(_T("Reset Password Successful!"));
	}
}


void CSTC1Dlg::OnBnClickedButton13()
{
	m_mainui.PostMessage(WM_COMMAND, MAKEWPARAM(IDC_UPBUTTON, BN_CLICKED), NULL);
}

void CSTC1Dlg::OnBnClickedButton2()
{
	CString Result = L"";
	mysql_init(&m_sqlCon);//初始化数据库对象
	if (!mysql_real_connect(&m_sqlCon, "localhost", "root", "123456", "chair001", 3306, NULL, 0))//localhost:服务器地址，可以直接填入IP;root:账号;123:密码;test:数据库名;3306:网络端口  
	{
		AfxMessageBox(_T("Database connection failed!"));
		return;
	}
	for (int i = 0; i < m_report.BeamNum+1; i++)
	{
		str.Format(L"SELECT B%d FROM treatmentreport where PlanName = \"%s\"",i+1, m_report.PlanName);
		const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
		char c1[500] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		const char* cc1 = c1;
		mysql_query(&m_sqlCon, cc1);
		m_res = mysql_store_result(&m_sqlCon);
		m_row = mysql_fetch_row(m_res);
		str = m_row[0]; 
		wstr = (LPCTSTR)str;
		c1[500] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		CString beamdata[15];
		int infonum=0;
		for (int n = 0; n < 500; n++)
		{
			if (c1[n] == 0x2C)//逗号
			{
				beamdata[infonum] = Result;
				Result = L"";
				infonum++;
			}
			else
			{
				Result += c1[n];
			}
			if (infonum==15)
			{
				break;
			}
		}
			m_report.PlanBeamInfo[i][0] = _wtof(beamdata[0].GetBuffer()); m_report.ReBeamInfo[i][0] = _wtof(beamdata[1].GetBuffer());
			m_report.PlanBeamInfo[i][1] = _wtof(beamdata[2].GetBuffer()); m_report.ReBeamInfo[i][1] = _wtof(beamdata[3].GetBuffer());
			m_report.PlanBeamInfo[i][2] = _wtof(beamdata[4].GetBuffer()); m_report.ReBeamInfo[i][2] = _wtof(beamdata[5].GetBuffer());
			m_report.PlanBeamInfo[i][3] = _wtof(beamdata[6].GetBuffer()); m_report.ReBeamInfo[i][3] = _wtof(beamdata[7].GetBuffer());
			m_report.PlanBeamInfo[i][4] = _wtof(beamdata[8].GetBuffer()); m_report.ReBeamInfo[i][4] = _wtof(beamdata[9].GetBuffer());
			m_report.PlanBeamInfo[i][5] = _wtof(beamdata[10].GetBuffer()); m_report.ReBeamInfo[i][5] = _wtof(beamdata[11].GetBuffer());
			m_report.BeamName[i] = beamdata[12]; m_report.BeamOper[i] = beamdata[13]; m_report.BeamTime[i] = beamdata[14];
	}
	mysql_close(&m_sqlCon);//关闭Mysql连接
}

void CSTC1Dlg::OnInputpatientAddpatient()//新建病人并添加
{
	AddPatient ADDP;
	if (ADDP.DoModal() == IDOK)
	{

	}
	else
	{}
}


void CSTC1Dlg::OnInputpatientEditpatient()
{
	AddPatient ADDP;
	ADDP.edit = TRUE;
	if (ADDP.DoModal() == IDOK)
	{
	}
	else
	{}
}

void CSTC1Dlg::OnInputplanAddplan()
{
	ADDPLAN ADDP;
	//ADDP.edit = TRUE;
	if (ADDP.DoModal() == IDOK)
	{
	}
	else
	{
	}
}


void CSTC1Dlg::OnInputplanEditplan()
{
	ADDPLAN ADDP;
	ADDP.isedit = TRUE;
	if (ADDP.DoModal() == IDOK)
	{
	}
	else
	{
	}
}


void CSTC1Dlg::OnUserAdduser()
{
	// TODO: 在此添加命令处理程序代码
	ADDUSER ADDP;
	//ADDP.isedit = TRUE;
	if (ADDP.DoModal() == IDOK)
	{
	}
	else
	{
	}
}


void CSTC1Dlg::OnBnClickedmovebeginbutton()
{
	if (ChairMode == TreatmentMode)
	{
		//CtrlSpeed();
		SetTimer(2, 100, NULL);
	}
}
