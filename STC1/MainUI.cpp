// MainUI.cpp : 实现文件
//

#include "stdafx.h"
#include "STC1.h"
#include "MainUI.h"
#include "afxdialogex.h"
#include "DOFComm.h"
#include "math.h"
#include "mmsystem.h"
#include<ATLBASE.H>
#include<iomanip>
#include <io.h>

#include "winsock.h"    
#include "mysql.h"  

#include <hpdf.h>
#pragma comment(lib,"libhpdf.lib")//附加依赖项，也可以在工程属性中设置

#include <websocket.h>
#include <stdio.h>

#pragma comment(lib,"libmysql.lib")//附加依赖项，也可以在工程属性中设置

//using namespace MSSOAPLib;
//using namespace MSSOAPLib


MatrxData m_m;
float ref[6];
double ForZAxisCorr[6];
double TTS_coordinate_value[7];
int IfMoveInPlace[6];//是否运动到位的标志

CString  PID;
BOOL PisOpen;//计划打开的标志位
extern int ChairMode;
extern bool REset;//先从负角度转到5.再回零的操作标志位
extern CString UserName;
extern CString permission;
extern float single_move_speed_show[6];

extern double M_COORD[6];

bool IsTreatMoving ;

double correctdatafor360[6];//用于360平台的校正

//int BeamNum;
//CString BeamName[20];//束流名称
//CString BeamTime[20];//束流应用时间
//CString BeamOper[20];//束流操作员
//double PlanBeamInfo[20][6];//束流位置的信息
//double ReBeamInfo[20][6];//真实位置的信息

Translation translation;
TreatReportData m_report;
CorrectData m_pv;
COLORREF ShowColor;

// CMainUI 对话框

IMPLEMENT_DYNAMIC(CMainUI, CDialogEx)

CMainUI::CMainUI(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainUI::IDD, pParent)
{
	trans.init();

	//m_Svr.SetDlg(this);
	////pNewConn.SetDlg(this);
	//OnBtnStart();
	//SENDTOADcount = 0;
	//m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	//hAccTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	//::RegisterHotKey(GetSafeHwnd(), 1000, MOD_ALT, 'D');//注册热键alt+D（D必须大写） 
	EStop = false;
	lockflag2 = false;
	PID=L"Patient ID";

	for (size_t i = 0; i < 6; i++)
	{
		TTS_coordinate_value[i] = 0;
		ref[i] = 0;	
	}
	for (size_t i = 0; i < 6; i++)
	{
		ForZAxisCorr[i] = 0;
	}
	TTS_coordinate_value[2] = 270;

	//OnBnClickedButton11();
	ReadConfig();
	ShowColor = RGB(0,255,0);
	isfirstshowbeam = true;
}

CMainUI::~CMainUI()
{
}

void CMainUI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_XEDIT2, TTS_coordinate_value[0]);
	DDX_Text(pDX, IDC_YEDIT2, TTS_coordinate_value[1]);
	DDX_Text(pDX, IDC_ZEDIT2, TTS_coordinate_value[2]);
	DDX_Text(pDX, IDC_AXEDIT2, TTS_coordinate_value[3]);
	DDX_Text(pDX, IDC_AYEDIT2, TTS_coordinate_value[4]);
	DDX_Text(pDX, IDC_AZEDIT2, TTS_coordinate_value[5]);

	DDX_Text(pDX, IDC_patientIDEDIT, PID);


	//DDX_Control(pDX, IDC_TREATMODEBUTTON, m_treatmentmode);

	DDX_Control(pDX, IDC_COMMLIST, m_beamlist);
	DDX_Control(pDX, IDC_PLANCOMBO, m_plancombobox);
	DDX_Control(pDX, IDC_patientIDEDIT, m_patientidedit);
	DDX_Control(pDX, IDC_workflowSTATIC, m_tr_wf_box);
}


BEGIN_MESSAGE_MAP(CMainUI, CDialogEx)
	
	
	ON_BN_CLICKED(IDC_ALIGNERBUTTON, &CMainUI::OnBnClickedAlignerbutton)

ON_WM_TIMER()
//ON_MESSAGE(WM_HOTKEY, OnHotKey)
//ON_WM_DESTROY()
ON_BN_CLICKED(IDC_TREATMODEBUTTON, &CMainUI::OnBnClickedTreatmodebutton)

ON_BN_CLICKED(IDC_CORRBUTTON, &CMainUI::OnBnClickedCorrbutton)
ON_BN_CLICKED(IDC_TREATFINISHBUTTON, &CMainUI::OnBnClickedTreatfinishbutton)
ON_BN_CLICKED(IDC_UPBUTTON, &CMainUI::OnBnClickedUpbutton)
ON_BN_CLICKED(IDC_DOWNBUTTON, &CMainUI::OnBnClickedDownbutton)

ON_EN_SETFOCUS(IDC_patientIDEDIT, &CMainUI::OnEnSetfocuspatientidedit)
ON_EN_KILLFOCUS(IDC_patientIDEDIT, &CMainUI::OnEnKillfocuspatientidedit)
//ON_COMMAND(ID_SaveReport, &CMainUI::OnSavereport)
ON_WM_CTLCOLOR()
ON_STN_CLICKED(IDC_PLANChoice, &CMainUI::OnStnClickedPlanchoice)
ON_NOTIFY(NM_DBLCLK, IDC_COMMLIST, &CMainUI::OnNMDblclkCommlist)
ON_CBN_SELCHANGE(IDC_PLANCOMBO, &CMainUI::OnCbnSelchangePlancombo)
ON_BN_CLICKED(IDC_BUTTON1, &CMainUI::OnBnClickedButton1)
ON_STN_CLICKED(IDC_STATICPExit, &CMainUI::OnStnClickedStaticpexit)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_COMMLIST, &CMainUI::OnNMCustomdrawCommlist)
ON_BN_CLICKED(IDC_BUTTON3, &CMainUI::OnBnClickedButton3)
END_MESSAGE_MAP()


// CMainUI 消息处理程序


/////////////////回调函数/////////////////


//void CMainUI::OnRcv(CClientSock* pConn)
//{
//	if (NULL != pConn)
//	{
//		//char rcvBuf[513] = { 0 };
//		int nRcved = 0;
//		nRcved = pConn->Receive(&fddd, 512);
//		switch (fddd.AD1)
//		{
//		case 1:
//			OnBnClickedEnbutton();
//			break;
//		case 2:
//			OnBnClickedLockbutton();
//			break;
//		case 3:
//			OnBnClickedButton6();
//			break;
//		case 4:
//			OnBnClickedButton9();
//			break;
//		case 5:
//			OnBnClickedResetbutton();
//			break;
//		case 6:
//			OnBnClickedButton10();
//			break;
//		case 7:
//			//pConn->Send(&dddd, 32);
//			//SetDlgItemText(IDC_INFOEDIT, L"7");
//			for (size_t i = 0; i < 6; i++)
//			{
//				attitude.mid[i] = fddd.ADstate[i];
//				UpdateData(FALSE);
//				OnBnClickedCtrlbutton();
//			}
//			break;
//		case 0:
//			pConn->Send(&dddd, 32);
//			SetDlgItemText(IDC_INFOEDIT, L"0");
//			break;
//		}
//		//fddd = rcvBuf;
//		//if (SOCKET_ERROR != nRcved)
//		//{
//			//pConn->Send(&fddd, 32);
//		//}
//	}
//}
//
////有客户端断开
//void CMainUI::OnClientClose(int nErrCode, CClientSock* pConn)
//{
//	IsPhoneLink = FALSE;
//	m_infoedit.ReplaceSel(_T("\r\n手机断开"));
//	//SetDlgItemText(IDC_INFOEDIT, L"手机断开");
//	//pNewConn= nullptr;
//}
//
////有新连接
//void CMainUI::OnAccept(int nErrCode)
//{
//	if (0 == nErrCode)
//	{
//		//CClientSock* pNewConn = new CClientSock;
//		pNewConn->SetDlg(this);
//		m_Svr.Accept(*pNewConn);
//		IsPhoneLink = TRUE;
//		m_infoedit.ReplaceSel(_T("\r\n手机联入"));
//		//SetDlgItemText(IDC_INFOEDIT, L"手机联入");
//	}
//	else
//	{
//	}
//}
//
//void CMainUI::OnBtnStart()
//{
//
//	if (!m_Svr.Create(8080U, SOCK_STREAM, FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE))
//	{
//		MessageBox(L"Create Socket Error!");
//		return;
//	}
//	if (!m_Svr.Listen())
//	{
//		int nErrorCode = m_Svr.GetLastError();
//		if (nErrorCode != WSAEWOULDBLOCK)
//		{
//			MessageBox(L"Listen Error!");
//			m_Svr.Close();
//			PostQuitMessage(0);
//			return;
//		}
//	}
//}
//
//void CMainUI::OnBtnStop()
//{
//	m_Svr.Close();
//}

//治疗流程开始
void CMainUI::OnBnClickedTreatmodebutton()
{
	ChairMode=1;
	m_report.BeamNum = 0;
	for (size_t i = 0; i < 20; i++)
	{
		m_report.BeamOper[i] = "null";
		m_report.BeamTime[i] = "-1";
		m_report.BeamAngle[i] = "-1";
	}
	m_report.BeamNum = 0;
	ReadConfig();
		//m_treatmentmode.SetColor(BLACK, GREEN);

		//this->GetDlgItem(IDC_CTRLBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_ALIGNERBUTTON)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_TREATFINISHBUTTON)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_UPBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_DOWNBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_TREATMODEBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_PLANCOMBO)->EnableWindow(FALSE);
		//this->GetDlgItem(IDC_patientIDEDIT)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_STATICPExit)->ShowWindow(FALSE);
		

		CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针
		tab->GetDlgItem(IDC_TAB1)->EnableWindow(FALSE);

		tab->GetDlgItem(IDC_STARTBUTTON)->EnableWindow(FALSE);
		tab->GetDlgItem(IDC_EnBUTTON)->EnableWindow(FALSE);
		tab->GetDlgItem(IDC_LockBUTTON)->EnableWindow(FALSE);
		tab->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
		tab->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
		tab->GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);

		//translation.Offset[3] = M_COORD[3];
		//translation.Offset[4] = M_COORD[4];
		//translation.Offset[5] = M_COORD[5];
}


void CMainUI::OnBnClickedAlignerbutton()//初始化坐标系
{
	CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针

	for (size_t i = 0; i < 6; i++)
	{
		TTS_coordinate_value[i] = 0;
	}
	TTS_coordinate_value[2] = 270;

	UpdateData(FALSE);
	this->GetDlgItem(IDC_ALIGNERBUTTON)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_CORRBUTTON)->EnableWindow(TRUE);
		SetDlgItemTextW(IDC_360EDIT, L"270");

	DWORD dwCopied = 0;
	double stepnum = 0;
	CString szKeyValue;
	CString m_szFileName = L"./SettingsINI.ini";
	dwCopied = ::GetPrivateProfileString(L"ReferencePoint", L"lat", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer()); ref[3] = stepnum;
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AXEDIT, str);
	dwCopied = ::GetPrivateProfileString(L"ReferencePoint", L"long", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer()); ref[4] = stepnum;
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AYEDIT, str);
	dwCopied = ::GetPrivateProfileString(L"ReferencePoint", L"vert", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer()); ref[5] = stepnum;
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AZEDIT, str);

	dwCopied = ::GetPrivateProfileString(L"360Correction", L"X", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer()); ForZAxisCorr[3] = stepnum;

	dwCopied = ::GetPrivateProfileString(L"360Correction", L"Y", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer()); ForZAxisCorr[4] = stepnum;

	dwCopied = ::GetPrivateProfileString(L"360Correction", L"Z", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer()); ForZAxisCorr[5] = stepnum;

	//this->GetDlgItem(IDC_ESTOPBUTTON2)->EnableWindow(TRUE);
	//this->GetDlgItem(IDC_LockCPositionButton)->EnableWindow(TRUE);

	//str.Format(L"%.2f", DICOM_coordinate_value[0]); this->SetDlgItemTextW(IDC_XEDIT, str);
	//str.Format(L"%.2f", DICOM_coordinate_value[1]); this->SetDlgItemTextW(IDC_YEDIT, str);
	//str.Format(L"%.2f", DICOM_coordinate_value[2]); this->SetDlgItemTextW(IDC_ZEDIT, str);
	//str.Format(L"%.2f", DICOM_coordinate_value[3]); this->SetDlgItemTextW(IDC_AXEDIT, str);
	//str.Format(L"%.2f", DICOM_coordinate_value[4]); this->SetDlgItemTextW(IDC_AYEDIT, str);
	//str.Format(L"%.2f", DICOM_coordinate_value[5]); this->SetDlgItemTextW(IDC_AZEDIT, str);

}


void CMainUI::OnBnClickedCorrbutton()//做六维校正的操作
{
	GetDlgItemText(IDC_XEDIT2, str); if (str == "") { AfxMessageBox(_T("Can't input null data, please check your input")); return; }
	GetDlgItemText(IDC_YEDIT2, str); if (str == "") { AfxMessageBox(_T("Can't input null data, please check your input")); return; }
	GetDlgItemText(IDC_ZEDIT2, str); if (str == "") { AfxMessageBox(_T("Can't input null data, please check your input")); return; }
	GetDlgItemText(IDC_AXEDIT2, str); if (str == "") { AfxMessageBox(_T("Can't input null data, please check your input")); return; }
	GetDlgItemText(IDC_AYEDIT2, str); if (str == "") { AfxMessageBox(_T("Can't input null data, please check your input")); return; }
	GetDlgItemText(IDC_AZEDIT2, str); if (str == "") { AfxMessageBox(_T("Can't input null data, please check your input")); return; }
	
	translation.Offset[3] = M_COORD[3];
	translation.Offset[4] = M_COORD[4];
	translation.Offset[5] = M_COORD[5];

	single_move_speed_show[0] = 1; single_move_speed_show[1] = 1; single_move_speed_show[2] = 1;
	single_move_speed_show[3] = 10; single_move_speed_show[4] = 10; single_move_speed_show[5] = 10;
	ShowColor = RGB(255, 255, 0);
	CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针 

										   //this->GetDlgItem(IDC_CORRBUTTON)->EnableWindow(FALSE);
	double NUM;
	double ifRefresh=false;
	GetDlgItemText(IDC_XEDIT2, str);
	NUM = _wtof(str.GetBuffer());
	TTS_coordinate_value[0] = NUM; 
	GetDlgItemText(IDC_YEDIT2, str);
	NUM = _wtof(str.GetBuffer());
	TTS_coordinate_value[1] = NUM;

	GetDlgItemText(IDC_ZEDIT2, str);
	NUM = _wtof(str.GetBuffer());
	TTS_coordinate_value[2] = NUM; TTS_coordinate_value[6] = TTS_coordinate_value[2];
	GetDlgItemText(IDC_AXEDIT2, str);
	NUM = _wtof(str.GetBuffer());
	TTS_coordinate_value[3] = NUM * 10;//从厘米转为毫米
	GetDlgItemText(IDC_AYEDIT2, str);
	NUM = _wtof(str.GetBuffer());
	TTS_coordinate_value[4] = NUM * 10;
	GetDlgItemText(IDC_AZEDIT2, str);
	NUM = _wtof(str.GetBuffer());
	TTS_coordinate_value[5] = NUM * 10;

	//在TTS中限位
	if ((TTS_coordinate_value[0]>360) || (TTS_coordinate_value[1]>360) || (TTS_coordinate_value[2]>360))
	{
		AfxMessageBox(_T("输入数值存在错误，请检查！"));
		return;
	}
	if ((TTS_coordinate_value[0]<0) || (TTS_coordinate_value[1]<0) || (TTS_coordinate_value[2]<0))
	{
		AfxMessageBox(_T("输入数值存在错误，请检查！"));
		return;
	}
	if ((TTS_coordinate_value[0]-360)>-60)
	{
		TTS_coordinate_value[0] = TTS_coordinate_value[0] - 360;
	}
	if ((TTS_coordinate_value[1] - 360)>-60)
	{
		TTS_coordinate_value[1] = TTS_coordinate_value[1] - 360;
	}

	//if ((TTS_coordinate_value[0] - FLimitInTTS[0])<360)
	//{
	//	TTS_coordinate_value[0] = FLimitInTTS[0] + 360; AfxMessageBox(_T("The input value exceeds the security range and has been refreshed to a qualified value.")); ifRefresh = true;
	//}
	//if ((TTS_coordinate_value[1] - FLimitInTTS[1])<360)
	//{
	//	TTS_coordinate_value[1] = FLimitInTTS[1] + 360; AfxMessageBox(_T("The input value exceeds the security range and has been refreshed to a qualified value.")); ifRefresh = true;
	//}

	//if ((TTS_coordinate_value[0] - FLimitInTTS[0])<360)
	//{
	//	TTS_coordinate_value[0] = FLimitInTTS[0]+360; AfxMessageBox(_T("The input value exceeds the security range and has been refreshed to a qualified value.")); ifRefresh = true;
	//}
	//if ((TTS_coordinate_value[1] - FLimitInTTS[1])<360)
	//{
	//	TTS_coordinate_value[1] = FLimitInTTS[1] + 360; AfxMessageBox(_T("The input value exceeds the security range and has been refreshed to a qualified value.")); ifRefresh = true;
	//}

	if (TTS_coordinate_value[0] > ULimitInTTS[0])
	{
		TTS_coordinate_value[0] = ULimitInTTS[0]; AfxMessageBox(_T("The input value exceeds the security range and has been refreshed to a qualified value.")); ifRefresh = true;
	}
	else if (TTS_coordinate_value[0] < FLimitInTTS[0])
	{
		TTS_coordinate_value[0] = 360+FLimitInTTS[0]; AfxMessageBox(_T("The input value exceeds the security range and has been refreshed to a qualified value.")); ifRefresh = true;
	}
	else if (TTS_coordinate_value[0] > FLimitInTTS[0]&& TTS_coordinate_value[0]<0)
	{
		TTS_coordinate_value[0] += 360;
	}

	if (TTS_coordinate_value[1] > ULimitInTTS[1] )
	{
		TTS_coordinate_value[1] = ULimitInTTS[1]; AfxMessageBox(_T("The input value exceeds the security range and has been refreshed to a qualified value.")); ifRefresh = true;
	}
	else if (TTS_coordinate_value[1] < FLimitInTTS[1])
	{
		TTS_coordinate_value[1] = 360+FLimitInTTS[1]; AfxMessageBox(_T("The input value exceeds the security range and has been refreshed to a qualified value.")); ifRefresh = true;
	}
	else if (TTS_coordinate_value[1] > FLimitInTTS[1] && TTS_coordinate_value[1]<0)
	{
		TTS_coordinate_value[1] += 360;
	}

	if (TTS_coordinate_value[2] >360 || TTS_coordinate_value[2] <0)
	{
		TTS_coordinate_value[2] == 0;
	}
	for (size_t i = 3; i < 6; i++)
	{
		if (TTS_coordinate_value[i] > ULimitInTTS[i])
		{
			TTS_coordinate_value[i] = ULimitInTTS[i]; AfxMessageBox(_T("The input value exceeds the security range and has been refreshed to a qualified value.")); ifRefresh = true;
		}
		if (TTS_coordinate_value[i] < FLimitInTTS[i])
		{
			TTS_coordinate_value[i] = FLimitInTTS[i]; AfxMessageBox(_T("The input value exceeds the security range and has been refreshed to a qualified value.")); ifRefresh = true;
		}
	}
	if (ifRefresh == true)
	{
		str.Format(L"%.1f", TTS_coordinate_value[0]); this->SetDlgItemTextW(IDC_XEDIT2, str);
		str.Format(L"%.1f", TTS_coordinate_value[1]); this->SetDlgItemTextW(IDC_YEDIT2, str);
		str.Format(L"%.1f", TTS_coordinate_value[2]); this->SetDlgItemTextW(IDC_ZEDIT2, str);
		str.Format(L"%.2f", TTS_coordinate_value[3] / 10); this->SetDlgItemTextW(IDC_AXEDIT2, str);
		str.Format(L"%.2f", TTS_coordinate_value[4] / 10); this->SetDlgItemTextW(IDC_AYEDIT2, str);
		str.Format(L"%.2f", TTS_coordinate_value[5] / 10); this->SetDlgItemTextW(IDC_AZEDIT2, str);
		return;
	}

		CLogin m_login; m_login.moveingorlogin = 1;
		if (m_login.DoModal() == IDOK)
		{
			
		}
		else
		{
			//CDialogEx::OnCancel();
			return;
		}

		DWORD dwCopied = 0;
		double stepnum = 0;
		CString szKeyValue;
		CString m_szFileName = L"./SettingsINI.ini";

		dwCopied = ::GetPrivateProfileString(L"360Correction", L"X", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
		stepnum = _wtof(szKeyValue.GetBuffer()); ForZAxisCorr[3] = stepnum;
		dwCopied = ::GetPrivateProfileString(L"360Correction", L"Y", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
		stepnum = _wtof(szKeyValue.GetBuffer()); ForZAxisCorr[4] = stepnum;
		dwCopied = ::GetPrivateProfileString(L"360Correction", L"Z", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
		stepnum = _wtof(szKeyValue.GetBuffer()); ForZAxisCorr[5] = stepnum;


		GetDlgItemText(IDC_360EDIT, str);
		if (m_report.BeamNum == 0 && m_report.BeamAngle[m_report.BeamNum] == "-1")
		{
			m_report.BeamAngle[m_report.BeamNum] = str;
		}

		if (m_report.BeamAngle[m_report.BeamNum] != str)
		{
			m_report.BeamNum++;
			m_report.BeamAngle[m_report.BeamNum] = str;
		}
		else if (m_report.BeamAngle[m_report.BeamNum] == str)
		{
			GetDlgItemText(IDC_XEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.PlanBeamInfo[m_report.BeamNum][0] = NUM;
			GetDlgItemText(IDC_YEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.PlanBeamInfo[m_report.BeamNum][1] = NUM;
			GetDlgItemText(IDC_ZEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.PlanBeamInfo[m_report.BeamNum][2] = NUM;
			GetDlgItemText(IDC_AXEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.PlanBeamInfo[m_report.BeamNum][3] = NUM;
			GetDlgItemText(IDC_AYEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.PlanBeamInfo[m_report.BeamNum][4] = NUM;
			GetDlgItemText(IDC_AZEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.PlanBeamInfo[m_report.BeamNum][5] = NUM;
		}

		if (m_report.BeamNum>0)
		{
			GetDlgItemText(IDC_RXEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum - 1][0] = NUM;
			GetDlgItemText(IDC_RYEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum - 1][1] = NUM;
			GetDlgItemText(IDC_RZEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum - 1][2] = NUM;
			GetDlgItemText(IDC_RAXEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum - 1][3] = NUM ;//从厘米转为毫米
			GetDlgItemText(IDC_RAYEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum - 1][4] = NUM ;
			GetDlgItemText(IDC_RAZEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum - 1][5] = NUM ;
		}

		

		m_report.BeamOper[m_report.BeamNum] = UserName;
		CTime tm; tm = CTime::GetCurrentTime();
		int year = tm.GetYear(); int month = tm.GetMonth(); int day = tm.GetDay(); int hour = tm.GetHour(); int minute = tm.GetMinute(); int second = tm.GetSecond();///分钟
		char time[200];
		if (minute<10 && second>10)
		{
			sprintf(time, "%d/%d/%d %d:0%d:%d ", year, month, day, hour, minute, second);
		}
		if (minute>10 && second<10)
		{
			sprintf(time, "%d/%d/%d %d:%d:0%d ", year, month, day, hour, minute, second);
		}
		if (minute<10 && second<10)
		{
			sprintf(time, "%d/%d/%d %d:0%d:0%d ", year, month, day, hour, minute, second);
		}
		if (minute>10 && second>10)
		{
			sprintf(time, "%d/%d/%d %d:%d:%d ", year, month, day, hour, minute, second);
		}

		m_report.BeamTime[m_report.BeamNum] = time;
		//GetDlgItemText(IDC_360EDIT, str);
		//m_report.BeamAngle[m_report.BeamNum]=str;
		for (size_t i = 0; i < 6; i++)
		{
			IfMoveInPlace[i] = 0;
		}
		GetDlgItemText(IDC_XEDIT2, str);
		NUM = _wtof(str.GetBuffer());
		m_report.PlanBeamInfo[m_report.BeamNum][0] = NUM;
		GetDlgItemText(IDC_YEDIT2, str);
		NUM = _wtof(str.GetBuffer());
		m_report.PlanBeamInfo[m_report.BeamNum][1] = NUM;
		GetDlgItemText(IDC_ZEDIT2, str);
		NUM = _wtof(str.GetBuffer());
		m_report.PlanBeamInfo[m_report.BeamNum][2] = NUM;
		GetDlgItemText(IDC_AXEDIT2, str);
		NUM = _wtof(str.GetBuffer());
		m_report.PlanBeamInfo[m_report.BeamNum][3] = NUM;
		GetDlgItemText(IDC_AYEDIT2, str);
		NUM = _wtof(str.GetBuffer());
		m_report.PlanBeamInfo[m_report.BeamNum][4] = NUM;
		GetDlgItemText(IDC_AZEDIT2, str);
		NUM = _wtof(str.GetBuffer());
		m_report.PlanBeamInfo[m_report.BeamNum][5] = NUM;

		if (TTS_coordinate_value[2]<=275&& TTS_coordinate_value[2]>=265)
		{
			str.Format(L"%.1f", 0);
			tab->SetDlgItemTextW(IDC_AZ360EDIT, str);
			//GetDlgItemText(IDC_XEDIT2, str);
			//NUM = _wtof(str.GetBuffer());
			//ForZAxisCorr[3] = 0;
		}
		else if (TTS_coordinate_value[2]>275 || TTS_coordinate_value[2]<265)
		{
			GetDlgItemText(IDC_360EDIT, str);
			NUM = _wtof(str.GetBuffer());//实际的PV Beam值
			double temp = 270 - NUM;
			if (temp>=180){temp -= 360;}
			if (temp<-180){temp += 360;}
			//ForZAxisCorr[3] = temp;
			str.Format(L"%.1f", temp);
			tab->SetDlgItemTextW(IDC_AZ360EDIT, str);

			TTS_coordinate_value[2] = TTS_coordinate_value[2]-NUM+270;
			if (TTS_coordinate_value[2]>275 || TTS_coordinate_value[2]<265)//忘记填PVBEAM的数值
			{
				temp = 270-TTS_coordinate_value[2];
				if (temp >= 180) { temp -= 360; }
				if (temp<-180) { temp += 360; }
				//ForZAxisCorr[3] = temp;
				str.Format(L"%.1f", temp);
				tab->SetDlgItemTextW(IDC_AZ360EDIT, str); 
				TTS_coordinate_value[2] = 270;	
			}
		}
		tab->GetDlgItemText(IDC_AZ360EDIT, str);
		ForZAxisCorr[2] =_wtof( str.GetBuffer());		
		//trans.Correct3(FIXED_coordinate_value, vector);
		//trans.init();
		//trans.CorrectinTTS(Correctdata, vector);
		translation.MatrixToSolve(TTS_coordinate_value,vector,0);

		translation.CorrectionFor360Zaxis(ForZAxisCorr, sssForZAxisCorr);
		for (size_t i = 0; i < 6; i++)
		{
			correctdatafor360[i] = sssForZAxisCorr[i];
		}

		//GetXYCORRData(vector);
			
			
		str.Format(L"%.3f", vector[0]); tab->SetDlgItemTextW(IDC_XEDIT, str);
		str.Format(L"%.3f", vector[1]); tab->SetDlgItemTextW(IDC_YEDIT, str);
		str.Format(L"%.3f", vector[2]-270); tab->SetDlgItemTextW(IDC_ZEDIT, str);
		//如果运用第三校正的话，以下平移数值应当不加上TransLatedata里面的值，因为这些值已经在西门子算法中加上了
		//str.Format(L"%.3f", TransLatedata[3] + vector[3]); this->SetDlgItemTextW(IDC_AXEDIT, str);
		//str.Format(L"%.3f", TransLatedata[4] + vector[4]); this->SetDlgItemTextW(IDC_AYEDIT, str);
		//str.Format(L"%.3f", TransLatedata[5] + vector[5]); this->SetDlgItemTextW(IDC_AZEDIT, str);
		str.Format(L"%.3f", vector[3]+ref[3]+ sssForZAxisCorr[3]); tab->SetDlgItemTextW(IDC_AXEDIT, str);
		str.Format(L"%.3f",vector[4]+ref[4]+ sssForZAxisCorr[4]); tab->SetDlgItemTextW(IDC_AYEDIT, str);
		str.Format(L"%.3f", vector[5]+ref[5]+ sssForZAxisCorr[5]); tab->SetDlgItemTextW(IDC_AZEDIT, str);

		//str.Format(L"%.1f", 270-Correctdata[2]); this->SetDlgItemTextW(IDC_AZ360EDIT, str);

		 
		//CWnd *cwnd = tab->GetWindow(GW_CHILD);//获取第一个子窗口指针，即Page1  
											   //CWnd *cwnd1 = cwnd->GetWindow(GW_HWNDNEXT);//获取第一个子窗口后的窗口，即Page2  
		//cwnd->GetDlgItemText(IDC_ZEDIT2, str);//获取Page2上Text控件的值  
		//str = L"SUSS";
		tab->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_movebeginBUTTON, BN_CLICKED), NULL);
		tab->SetWindowText(L"RCTS v0.2   UserName: " + UserName + "  Permission: " + permission);

	this->SetDlgItemTextW(IDC_XEDIT2, L"");
	this->SetDlgItemTextW(IDC_YEDIT2, L"");
	this->SetDlgItemTextW(IDC_ZEDIT2, L"");
	this->SetDlgItemTextW(IDC_AXEDIT2, L"");
	this->SetDlgItemTextW(IDC_AYEDIT2, L"");
	this->SetDlgItemTextW(IDC_AZEDIT2, L"");
}

void CMainUI::REOnBnClickedCorrbutton()//做六维校正的操作
{
	CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针

	if (TTS_coordinate_value[6]>275 || TTS_coordinate_value[6]<265)//忘记填PVBEAM的数值
	{
		double temp = 270 - TTS_coordinate_value[6];
		if (temp >= 180) { temp -= 360; }
		if (temp<-180) { temp += 360; }
		//ForZAxisCorr[3] = temp;
		str.Format(L"%.1f", temp);
		tab->SetDlgItemTextW(IDC_AZ360EDIT, str);
	}
	if (TTS_coordinate_value[6]<=275 && TTS_coordinate_value[6]>=265)//忘记填PVBEAM的数值
	{
		str.Format(L"%.1f", 0);
		tab->SetDlgItemTextW(IDC_AZ360EDIT, str);
	}


	str.Format(L"%.3f", vector[0]); tab->SetDlgItemTextW(IDC_XEDIT, str);
	str.Format(L"%.3f", vector[1]); tab->SetDlgItemTextW(IDC_YEDIT, str);
	str.Format(L"%.3f", vector[2] - 270); tab->SetDlgItemTextW(IDC_ZEDIT, str);
	str.Format(L"%.3f", vector[3] + ref[3] + sssForZAxisCorr[3]); tab->SetDlgItemTextW(IDC_AXEDIT, str);
	str.Format(L"%.3f", vector[4] + ref[4] + sssForZAxisCorr[4]); tab->SetDlgItemTextW(IDC_AYEDIT, str);
	str.Format(L"%.3f", vector[5] + ref[5] + sssForZAxisCorr[5]); tab->SetDlgItemTextW(IDC_AZEDIT, str);
	tab->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CTRLBUTTON, BN_CLICKED), NULL);
	//tab->SetWindowText(L"RCTS v0.2   UserName: " + UserName + "  Permission: " + permission);
}

void CMainUI::OnBnClickedTreatfinishbutton()
{
	int standbyorkeep;
	DWORD dwCopied = 0;
	double stepnum = 0;
	CString szKeyValue;
	CString m_szFileName = L"./SettingsINI.ini";
	dwCopied = ::GetPrivateProfileString(L"EndTreatment", L"M", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer()); standbyorkeep = stepnum;

	CLogin m_login;
	m_login.showWaring = true;
	if (m_login.DoModal() == IDOK)
	{

	}
	else
	{
		//CDialogEx::OnCancel();
		return;
	}
		for (size_t i = 0; i < 3; i++)
		{
			ref[i+3] = 0;
		}
		//m_treatmentmode.SetColor(BLACK, LTGRAY);
		double NUM;
		//if (m_report.BeamNum>0)
		//{
			GetDlgItemText(IDC_RXEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum ][0] = NUM;
			GetDlgItemText(IDC_RYEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum ][1] = NUM;
			GetDlgItemText(IDC_RZEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum ][2] = NUM;
			GetDlgItemText(IDC_RAXEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum ][3] = NUM;//从厘米转为毫米
			GetDlgItemText(IDC_RAYEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum ][4] = NUM ;
			GetDlgItemText(IDC_RAZEDIT2, str);
			NUM = _wtof(str.GetBuffer());
			m_report.ReBeamInfo[m_report.BeamNum ][5] = NUM ;
		//}

		ChairMode = 0;
		for (size_t i = 0; i < 3; i++)
		{
			single_move_speed_show[i] = 1;
		}
		for (size_t i = 3; i < 6; i++)
		{
			single_move_speed_show[i] = 10;
			ForZAxisCorr[i] = 0;
		}

		//this->GetDlgItem(IDC_CTRLBUTTON)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_TREATMODEBUTTON)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_ALIGNERBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_CORRBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_UPBUTTON)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_DOWNBUTTON)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_TREATFINISHBUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_PLANCOMBO)->EnableWindow(TRUE);

		//this->GetDlgItem(IDC_patientIDEDIT)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_STATICPExit)->ShowWindow(TRUE);
		trans.init();

		if (standbyorkeep==0)//standby
		{
			OnBnClickedUpbutton();
		}
		else if (standbyorkeep == 1)//keep iso standby
		{
			CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针 

			DWORD dwCopied = 0;
			double stepnum = 0;
			CString szKeyValue;
			CString m_szFileName = L"./SettingsINI.ini";
			/*dwCopied = ::GetPrivateProfileString(L"StepOn", L"iso", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
			stepnum = _wtof(szKeyValue.GetBuffer());
			stepnum = 270 - stepnum;
			if (stepnum > 180)stepnum = stepnum - 360;
			if (stepnum < -180)stepnum = stepnum + 360;
			str.Format(L"%.0f", stepnum); tab->SetDlgItemTextW(IDC_AZ360EDIT, str);*/
			dwCopied = ::GetPrivateProfileString(L"StepOn", L"pitch", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
			stepnum = _wtof(szKeyValue.GetBuffer());
			str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_XEDIT, str);
			dwCopied = ::GetPrivateProfileString(L"StepOn", L"roll", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
			stepnum = _wtof(szKeyValue.GetBuffer());
			str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_YEDIT, str);
			dwCopied = ::GetPrivateProfileString(L"StepOn", L"LeftRight", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
			stepnum = _wtof(szKeyValue.GetBuffer());
			str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AXEDIT, str);
			dwCopied = ::GetPrivateProfileString(L"StepOn", L"FrontBack", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
			stepnum = _wtof(szKeyValue.GetBuffer());
			str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AYEDIT, str);
			dwCopied = ::GetPrivateProfileString(L"StepOn", L"TopBottom", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
			stepnum = _wtof(szKeyValue.GetBuffer());
			str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AZEDIT, str);
			szKeyValue.ReleaseBuffer();

			tab->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CTRLBUTTON, BN_CLICKED), NULL);//OnBnClickedCtrlbutton();
		}
		
		CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针 
		/*str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_XEDIT, str); SetDlgItemTextW(IDC_AXEDIT2, str);
		str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_YEDIT, str); SetDlgItemTextW(IDC_AYEDIT2, str);
		str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_ZEDIT, str); SetDlgItemTextW(IDC_AZEDIT2, str);
		str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_AXEDIT, str); SetDlgItemTextW(IDC_XEDIT2, str);
		str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_AYEDIT, str); SetDlgItemTextW(IDC_YEDIT2, str);
		str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_AZEDIT, str); SetDlgItemTextW(IDC_ZEDIT2, L"270");
		str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_AZ360EDIT, str); 
		
		tab->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CTRLBUTTON, BN_CLICKED), NULL);*/
		tab->GetDlgItem(IDC_TAB1)->EnableWindow(TRUE);

		tab->GetDlgItem(IDC_STARTBUTTON)->EnableWindow(TRUE);
		tab->GetDlgItem(IDC_EnBUTTON)->EnableWindow(TRUE);
		//tab->GetDlgItem(IDC_LockBUTTON)->EnableWindow(TRUE);
		//tab->GetDlgItem(IDC_RESETBUTTON)->EnableWindow(TRUE);
		//tab->GetDlgItem(IDC_BUTTON10)->EnableWindow(TRUE);
		//tab->GetDlgItem(IDC_BUTTON13)->EnableWindow(TRUE);

		if (PisOpen)//保存相应的治疗过程数据
		{
			bool PlanExist = false;
			mysql_init(&m_sqlCon);//初始化数据库对象
			if (!mysql_real_connect(&m_sqlCon, "localhost", "root", "123456", "chair001", 3306, NULL, 0))//localhost:服务器地址，可以直接填入IP;root:账号;123:密码;test:数据库名;3306:网络端口  
			{
				AfxMessageBox(_T("Database connection failed!"));
				return;
			}
			else//连接成功则继续访问数据库，之后的相关操作代码基本是放在这里面的
			{
				(mysql_query(&m_sqlCon, "SELECT PlanName FROM treatmentreport"));//从users这个表格查询id
				m_res = mysql_store_result(&m_sqlCon);
				while (m_row = mysql_fetch_row(m_res))
				{
					str = m_row[0];
					if (m_report.PlanName == str)
					{
						PlanExist = true;
					}
				}
				if (!PlanExist)//这个计划不存在则插入一个新条目
				{
					str.Format(L"INSERT into treatmentreport (PlanName,PatientID,TreatTimes) VALUES(\"%s\",\"%s\",%d)", m_report.PlanName, m_report.PatientID, m_report.BeamNum);
					const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
					char c1[500] = { 0 };
					wcstombs(c1, wstr, wcslen(wstr));
					const char* cc1 = c1;
					mysql_query(&m_sqlCon, cc1);
					PlanExist = true;
				}
				if (PlanExist)
				{
					str.Format(L"SELECT TreatTimes FROM treatmentreport where PlanName = \"%s\"", m_report.PlanName);
					const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
					char c1[500] = { 0 };
					wcstombs(c1, wstr, wcslen(wstr));
					const char* cc1 = c1;
					mysql_query(&m_sqlCon, cc1);
					m_res = mysql_store_result(&m_sqlCon);
					m_row = mysql_fetch_row(m_res);
					str = m_row[0];
					int TreatTimes = _wtof(str.GetBuffer());

					str.Format(L"UPDATE treatmentreport SET TreatTimes=%d where PlanName = \"%s\"", TreatTimes + m_report.BeamNum+1, m_report.PlanName);
					wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
					c1[500] = { 0 };
					wcstombs(c1, wstr, wcslen(wstr));
					cc1 = c1;
					mysql_query(&m_sqlCon, cc1);

					for (size_t i = 0; i < m_report.BeamNum+1; i++)
					{
						str.Format(L"UPDATE treatmentreport SET B%d=\"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%s,%s,%s,\" where PlanName = \"%s\"", TreatTimes + i + 1,
							m_report.PlanBeamInfo[i][0], m_report.ReBeamInfo[i][0], m_report.PlanBeamInfo[i][1], m_report.ReBeamInfo[i][1],
							m_report.PlanBeamInfo[i][2], m_report.ReBeamInfo[i][2], m_report.PlanBeamInfo[i][3], m_report.ReBeamInfo[i][3],
							m_report.PlanBeamInfo[i][4], m_report.ReBeamInfo[i][4], m_report.PlanBeamInfo[i][5], m_report.ReBeamInfo[i][5],
							m_report.BeamName[i], m_report.BeamOper[i],m_report.BeamTime[i], m_report.PlanName);
						const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
						char c1[500] = { 0 };
						wcstombs(c1, wstr, wcslen(wstr));
						const char* cc1 = c1;
						mysql_query(&m_sqlCon, cc1);
						//m_res = mysql_store_result(&m_sqlCon);
						//m_row = mysql_fetch_row(m_res);
					}

				}
				mysql_close(&m_sqlCon);//关闭Mysql连接
			}
		}
}

void CMainUI::OnBnClickedUpbutton()//上人姿态
{
	ChairMode = 4;
	CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针 

	DWORD dwCopied = 0;
	double stepnum = 0;
	CString szKeyValue;
	CString m_szFileName = L"./SettingsINI.ini";
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"iso", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer());
	stepnum = 270 - stepnum; 
	if (stepnum > 180)stepnum = stepnum - 360;
	if (stepnum < -180)stepnum = stepnum + 360;
	str.Format(L"%.0f", stepnum); tab->SetDlgItemTextW(IDC_AZ360EDIT, str);
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"pitch", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer());
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_XEDIT, str);
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"roll", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer());
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_YEDIT, str);
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"LeftRight", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer());
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AXEDIT, str);
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"FrontBack", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer());
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AYEDIT, str);
	dwCopied = ::GetPrivateProfileString(L"StepOn", L"TopBottom", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer());
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AZEDIT, str);
	szKeyValue.ReleaseBuffer();

	tab->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CTRLBUTTON, BN_CLICKED), NULL);//OnBnClickedCtrlbutton();
}


void CMainUI::OnBnClickedDownbutton()
{
	ChairMode = 4;
	for (size_t i = 0; i < 6; i++)
	{
		ForZAxisCorr[i] = 0;
	}
	SetDlgItemTextW(IDC_360EDIT, L"270");
	CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针 

	DWORD dwCopied = 0;
	double stepnum = 0;
	CString szKeyValue;
	CString m_szFileName = L"./SettingsINI.ini";
	dwCopied = ::GetPrivateProfileString(L"ReferencePoint", L"lat", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer());// ref[3] = stepnum;
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AXEDIT, str);
	dwCopied = ::GetPrivateProfileString(L"ReferencePoint", L"long", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer()); //ref[4] = stepnum;
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AYEDIT, str);
	dwCopied = ::GetPrivateProfileString(L"ReferencePoint", L"vert", L"", szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
	stepnum = _wtof(szKeyValue.GetBuffer());// ref[5] = stepnum;
	str.Format(L"%.1f", stepnum); tab->SetDlgItemTextW(IDC_AZEDIT, str);
	//szKeyValue.ReleaseBuffer();




	szKeyValue.ReleaseBuffer();

	str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_XEDIT, str);
	str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_YEDIT, str);
	str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_ZEDIT, str);
	str.Format(L"%d", 5); tab->SetDlgItemTextW(IDC_AZ360EDIT, str);
	REset = true;
	//str.Format(L"%d", 270); SetDlgItemTextW(IDC_360EDIT, str);

	tab->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CTRLBUTTON, BN_CLICKED), NULL);//OnBnClickedCtrlbutton();


	for (size_t i = 0; i < 3; i++)
	{
		ref[i] = 0;
	}


	/*CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针 
	CString str;
	double num = 5;
	str.Format(L"%.1f", num);
	tab->SetDlgItemTextW(IDC_AZ360EDIT, str);
	REset = true;
	str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_XEDIT, str);
	str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_YEDIT, str);
	str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_ZEDIT, str);
	str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_AXEDIT, str);
	str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_AYEDIT, str);
	str.Format(L"%d", 0); tab->SetDlgItemTextW(IDC_AZEDIT, str);
	tab->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CTRLBUTTON, BN_CLICKED), NULL);*/

}
//BOOL CMainUI::PreTranslateMessage(MSG* pMsg)
//{
//	if (m_hAccel)
//	{
//		if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
//		{
//			return(TRUE);
//		}
//	}
//	return CDialog::PreTranslateMessage(pMsg);
//}

//LRESULT CMainUI::OnHotKey(WPARAM wParam, LPARAM lParam)
//{
//	if(wParam == 1000)
//		OnBnClickedTreatmodebutton();
//	//ToDo: add function 
//	return 0;
//}
//
//void CMainUI::OnDestroy()
//{
//	::UnregisterHotKey(GetSafeHwnd(), 1000);//销毁热键 
//}

void CMainUI::OnOK()
{
	//   CDialog::OnOK(); 
	CRect rect;
	m_patientidedit.GetWindowRect(&rect);
	CPoint  pt;
	GetCursorPos(&pt);

	bool ifFouse[7] = {false};

	//if (rect.PtInRect(pt)&&ChairMode==0)
	if (ChairMode == 0&&(!PisOpen))
	{
		//在m_patientidedit区域内
		OnStnClickedStaticpexit();
	}

	GetDlgItem(IDC_AXEDIT2)->GetWindowRect(&rect); GetCursorPos(&pt); ifFouse[0]=rect.PtInRect(pt);
	GetDlgItem(IDC_AYEDIT2)->GetWindowRect(&rect); GetCursorPos(&pt); ifFouse[1] = rect.PtInRect(pt);
	GetDlgItem(IDC_AZEDIT2)->GetWindowRect(&rect); GetCursorPos(&pt); ifFouse[2] = rect.PtInRect(pt);
	GetDlgItem(IDC_XEDIT2)->GetWindowRect(&rect); GetCursorPos(&pt); ifFouse[3] = rect.PtInRect(pt);
	GetDlgItem(IDC_YEDIT2)->GetWindowRect(&rect); GetCursorPos(&pt); ifFouse[4] = rect.PtInRect(pt);
	GetDlgItem(IDC_ZEDIT2)->GetWindowRect(&rect); GetCursorPos(&pt); ifFouse[5] = rect.PtInRect(pt);
	GetDlgItem(IDC_360EDIT)->GetWindowRect(&rect); GetCursorPos(&pt); ifFouse[6] = rect.PtInRect(pt);
	//if ((ifFouse[0]|| ifFouse[1] || ifFouse[2] || ifFouse[3] || ifFouse[4] || ifFouse[5] || ifFouse[6]) && ChairMode == 1)
	if (ChairMode == 1)
	{
		OnBnClickedCorrbutton();
	}
}

void CMainUI::OnEnSetfocuspatientidedit()
{
	GetDlgItemText(IDC_patientIDEDIT, str);
	if (str == L"Patient ID")
	{
		SetDlgItemTextW(IDC_patientIDEDIT, L"");
	}
}

void CMainUI::OnEnKillfocuspatientidedit()
{
	GetDlgItemText(IDC_patientIDEDIT, str);
	if (str == L"")
	{
		SetDlgItemTextW(IDC_patientIDEDIT, L"Patient ID");
	}
}

void CMainUI::LinkToESB2()
{
	//BasicHttpBinding();
	//CString url = _T("http://blog.csdn.net/qq_18297675/article/details/52240231?locationNum=2&fps=1");
	//str=GetHttpCode(url);
	//SetDlgItemTextW(IDC_patientIDEDIT, str);

	CoInitialize(NULL);//初始化com环境  

	ISoapSerializerPtr Serializer;
	ISoapReaderPtr Reader;
	ISoapConnectorPtr Connector;

	Connector.CreateInstance(__uuidof(HttpConnector30));   //创建对象  


	Connector->Property[_T("EndPointURL")] = _T("http://ws.webxml.com.cn//webservices/qqOnlineWebService.asmx");    //wsdl路径  
	Connector->Connect();

	////   Begin  the   message.  //消息体  
	//Connector->Property[_T("SoapAction")] = _T("http://WebXml.com.cn/qqCheckOnline");//函数体参数  
	Connector->BeginMessage();


	Serializer.CreateInstance(__uuidof(SoapSerializer30));
	Serializer->Init(_variant_t((IUnknown*)Connector->InputStream));

	//   Build  the   SOAP   Message.  
	//Serializer->StartHeader();
	Serializer->StartEnvelope(_T("Soap"), _T("http://www.w3.org/2001/XMLSchema-instance"), _T("utf-8"));
	Serializer->StartBody("");
	Serializer->StartElement(_T("qqCheckOnline"), _T("http://WebXml.com.cn/"), _T("utf-8"), _T("Soap"));//函数处理
	Serializer->StartElement(_T("qqCode"), _T(""), _T(""), _T("Soap"));
	Serializer->WriteString("aw");          //参数处理        
	Serializer->EndElement();
	Serializer->EndElement();
	Serializer->EndBody();
	Serializer->EndEnvelope();
	Connector->EndMessage();

	Reader.CreateInstance(__uuidof(SoapReader30));
	//wprintf(_T("here"));
	Reader->Load(_variant_t((IUnknown*)Connector->OutputStream), "");  //加载返回数据  
																	   //   Display  the   result.  
	MSXML2::IXMLDOMElementPtr pstr = Reader->RpcResult;
	char buff[1024] = { 0 };

	strncpy(buff, pstr->text, 1024);
	str.Format(L"%s", buff);
	//SetDlgItemTextW(IDC_STATIC4, str);

	CString        m_HttpCode;
	UINT           PageCode;   //CP_UTF8:65001 CP_ACP:0  转换代码用  
	PageCode = 65001; //因为我们的网址是UTF8格式，所以用65001；
	int nBufferSize = MultiByteToWideChar(PageCode, 0, (LPCSTR)buff, -1, NULL, 0);

	wchar_t *pBuffer = new wchar_t[nBufferSize + 1];
	memset(pBuffer, 0, (nBufferSize + 1) * sizeof(wchar_t));

	//gb2312转为unicode,则用CP_ACP  
	//gbk转为unicode,也用CP_ACP  
	//utf-8转为unicode,则用CP_UTF8  
	MultiByteToWideChar(PageCode, 0, (LPCSTR)buff, -1, pBuffer, nBufferSize * sizeof(wchar_t));

	m_HttpCode += pBuffer;
	//m_HttpCode += "\r\n";
	delete pBuffer;


	//多余。
	CFile file;
	file.Open(_T("test.txt"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);
	file.Write(m_HttpCode, m_HttpCode.GetLength());
	file.Close();

	//printf("Answer:%s\n", buff);
	CoUninitialize(); 
}



void CMainUI::LinkToESB()
{
	CoInitialize(NULL);//初始化com环境  

	ISoapSerializerPtr Serializer;
	ISoapReaderPtr Reader;
	ISoapConnectorPtr Connector;

	Connector.CreateInstance(__uuidof(HttpConnector30));   //创建对象  


	Connector->Property[_T("EndPointURL")] = _T("http://10.11.200.211/SHTHESB_Service/ESBService.asmx");    //wsdl路径
	Connector->Connect();

	////   Begin  the   message.  //消息体  

	Connector->BeginMessage();


	Serializer.CreateInstance(__uuidof(SoapSerializer30));
	Serializer->Init(_variant_t((IUnknown*)Connector->InputStream));

	//   Build  the   SOAP   Message.  

	Serializer->StartEnvelope(_T("ESBEnvelope"), _T("http://SHTH.ESB.TopSchemaV2"), _T("utf-8"));
	//Serializer->StartHeader("ESBHeader");
	//CTime tm; tm = CTime::GetCurrentTime();
	//int year = tm.GetYear(); int month = tm.GetMonth(); int day = tm.GetDay(); int hour = tm.GetHour(); int minute = tm.GetMinute(); int second = tm.GetSecond();
		//Serializer->StartElement(_T("HeaderControl"), _T("AppCode=\"APP097\" Password=\"sXV5vgiB\"  MessageCategory=\"SPHICB01A001\" Version=\"1\" CreateTime=\"2018-03-08 14:42:12\""), _T("utf-8"), _T("Soap"));//函数处理
		//Serializer->EndElement();
	//Serializer->EndHeader();

	//Serializer->StartBody("ESBBody");
		//Serializer->StartElement(_T("BodyControl"), _T("CallType=\"\""), _T("utf-8"), _T("Soap"));//函数处理
		//Serializer->EndElement();
		//Serializer->StartElement(_T("BusinessRequest"), _T(""), _T(""), _T("Soap"));
		//Serializer->WriteString("![CData[]]"); //参数处理   
		//Serializer->EndElement();
		//Serializer->StartElement(_T("shth:getPatState"), _T(""), _T(""), _T("soapenv"));
			//Serializer->StartElement(_T("arg0"), _T(""), _T(""), _T("Soap"));
				//Serializer->WriteString("&lt;getpatstate patientid='20019591'/&gt;"); //参数处理   
			//Serializer->EndElement();		
		//Serializer->EndElement();
	//Serializer->EndBody();
	Serializer->EndEnvelope();
	Connector->EndMessage();

	Reader.CreateInstance(__uuidof(SoapReader30));
	//wprintf(_T("here"));
	Reader->Load(_variant_t((IUnknown*)Connector->OutputStream), "");  //加载返回数据  
																	   //   Display  the   result.  
	MSXML2::IXMLDOMElementPtr pstr = Reader->RpcResult;
	char buff[1024] = { 0 };

	strncpy(buff, pstr->text, 1024);
	str.Format(L"%s", buff);

	CString        m_HttpCode;
	UINT           PageCode;   //CP_UTF8:65001 CP_ACP:0  转换代码用  
	PageCode = 65001; //因为我们的网址是UTF8格式，所以用65001；
	int nBufferSize = MultiByteToWideChar(PageCode, 0, (LPCSTR)buff, -1, NULL, 0);

	wchar_t *pBuffer = new wchar_t[nBufferSize + 1];
	memset(pBuffer, 0, (nBufferSize + 1) * sizeof(wchar_t));

	//gb2312转为unicode,则用CP_ACP  
	//gbk转为unicode,也用CP_ACP  
	//utf-8转为unicode,则用CP_UTF8  
	MultiByteToWideChar(PageCode, 0, (LPCSTR)buff, -1, pBuffer, nBufferSize * sizeof(wchar_t));

	m_HttpCode += pBuffer;
	//m_HttpCode += "\r\n";
	delete pBuffer;


	//多余。
	CFile file;
	file.Open(_T("ESBtest.txt"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);
	file.Write(m_HttpCode, m_HttpCode.GetLength());
	file.Close();

	//printf("Answer:%s\n", buff);
	CoUninitialize();
}

void CMainUI::ReadConfig()
{
	DWORD dwCopied = 0;
	CString szKeyValue;
	CString m_szFileName= L"./SettingsINI.ini";
	dwCopied = ::GetPrivateProfileString(L"LimitInTTS", L"pitch", L"",szKeyValue.GetBuffer(MAX_PATH), MAX_PATH, m_szFileName);
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

}

//主要接口，输入网址，获取代码
CString CMainUI::GetHttpCode(CString &url)
{
	CString        m_strError;  //接受错误信息
	CString        m_HttpCode;  //接受抓取的网页代码
	UINT           PageCode;   //CP_UTF8:65001 CP_ACP:0  转换代码用  

	PageCode = 65001; //因为我们的网址是UTF8格式，所以用65001；
	m_HttpCode = _T("");

	const TCHAR szHeaders[] = _T("Accept: _T/*\r\nUser-Agent: LCD's Infobay Http Client\r\n");

	//获取网页的初始化工作
	CInternetSession session(NULL, 0);
	CHttpFile *htmlFile = NULL;
	TCHAR sRecv[1024];  //接受缓存代码。

						//错误判断初始化
	DWORD dwServiceType = 0;
	DWORD  dwHttpRequestFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT;
	CString strServerName = _T("");
	CString strObject = _T("");
	INTERNET_PORT nPort = 0;
	CString StrContent = _T("");
	DWORD dwRetcode = -1;

	CHttpConnection *pServer = NULL;

	try
	{
		//if (!OnInitSession(session)) //判断链接是否成功；可以不要
		//{
			//return NULL;
		//}
		if (!AfxParseURL(url, dwServiceType, strServerName, strObject, nPort) || dwServiceType != INTERNET_SERVICE_HTTP)
		{
			m_strError = _T("非法的URL");
			AfxMessageBox(m_strError);
			return NULL;
		}

		pServer = session.GetHttpConnection(strServerName, nPort);


		if (pServer == NULL)
		{
			m_strError = _T("无法与服务器建立连接");
			AfxMessageBox(m_strError);
			return NULL;
		}
		//下面第一个可以为1  打开http链接
		htmlFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject,
			NULL, 1, NULL, NULL, dwHttpRequestFlags);

		if (htmlFile == NULL)
		{
			m_strError = _T("无法与服务器建立连接");
			AfxMessageBox(m_strError);
			return NULL;
		}

		/////////////////////////////////////////////////
		try
		{ //调用此成员函数添加一个或多个HTTP请求标头到HTTP请求处理。AddRequestHeaders
		  //
			if (!htmlFile->AddRequestHeaders(szHeaders) || !htmlFile->SendRequest())
			{
				m_strError = _T("网络错误－无法发送请求报头");
				AfxMessageBox(m_strError);
				return NULL;
			}
		}
		catch (CInternetException *ex)
		{
			StrContent.Empty();
			m_strError = _T("无法发送http报头,可能网络状况有问题");
			AfxMessageBox(m_strError);
			ex->Delete();
			return NULL;
		}
		//////////////////////////////////////////////////////////////////////////

		if (!htmlFile->QueryInfoStatusCode(dwRetcode))
		{
			m_strError = _T("网络错误－无法查询反馈代码");
			AfxMessageBox(m_strError);
			return NULL;
		}

		if (dwRetcode >= 200 && dwRetcode < 300)
		{
			try
			{
				//htmlFile = (CHttpFile*)session.OpenURL(url);//打开链接

				while (htmlFile->ReadString(sRecv, 1024))
				{
					// 编码转换，可解决中文乱码问题  
					//gb2312转为unicode,则用CP_ACP  
					//gbk转为unicode,也用CP_ACP  
					//utf-8转为unicode,则用CP_UTF8  

					int nBufferSize = MultiByteToWideChar(PageCode, 0, (LPCSTR)sRecv, -1, NULL, 0);

					wchar_t *pBuffer = new wchar_t[nBufferSize + 1];
					memset(pBuffer, 0, (nBufferSize + 1) * sizeof(wchar_t));

					//gb2312转为unicode,则用CP_ACP  
					//gbk转为unicode,也用CP_ACP  
					//utf-8转为unicode,则用CP_UTF8  
					MultiByteToWideChar(PageCode, 0, (LPCSTR)sRecv, -1, pBuffer, nBufferSize * sizeof(wchar_t));

					m_HttpCode += pBuffer;
					m_HttpCode += "\r\n";
					delete pBuffer;
				}

				htmlFile->Close();
				session.Close();
				delete htmlFile;

				//多余。
				CFile file;
				file.Open(_T("test2.txt"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);
				file.Write(m_HttpCode, m_HttpCode.GetLength());
				file.Close();

				return m_HttpCode;

			}
			catch (CInternetException* pEx)
			{
				m_strError = _T("接收数据错误");
				AfxMessageBox(m_strError);
				pEx->Delete();

				//因为是CString,所以返回NULL，而不是0；
				return NULL;
			}
			return StrContent;

		}
		else
		{
			//读取失败，将buffer清空。
			StrContent.Empty();

			//发送错误。
			AfxMessageBox(_T("未成功"));
			//OnProcessError(dwRetcode, session, pServer, htmlFile);

			return NULL;
		}

	}
	catch (CInternetException* pEx)
	{
		m_strError = _T("网络错误");
		AfxMessageBox(_T("网络错误"));
		pEx->Delete();
		return NULL;
	}

	return NULL;
}

void CMainUI::OnSavereport()
{
	const char *page_title = "Chair System Treatment Report";

	HPDF_Doc  pdf;
	HPDF_Font font, font2;
	HPDF_Page page;
	char fname[256];
	int ypos = 450;

	strcpy(fname, "D:\\");
	strcat(fname, "pdftest");
	strcat(fname, ".pdf");

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
	font = HPDF_GetFont(pdf, "SimHei", "GB-EUC-H");
	font2 = HPDF_GetFont(pdf, "SimSun", "GB-EUC-H");

	/* add a new page object. */
	page = HPDF_AddPage(pdf);

	status = HPDF_Page_SetFontAndSize(page, font, 30);
	float tw = HPDF_Page_TextWidth(page, page_title);
	status = HPDF_Page_BeginText(page);
	//show_description(page, 60, ypos - 50,
	//"RenderingMode=PDF_STROKE");
	HPDF_Page_SetTextRenderingMode(page, HPDF_FILL_CLIPPING);
	status = HPDF_Page_TextOut(page, (HPDF_Page_GetWidth(page) - tw) / 2,
		HPDF_Page_GetHeight(page) - 50, page_title);

	status = HPDF_Page_SetFontAndSize(page, font, 15);
	status = HPDF_Page_TextOut(page, 50, HPDF_Page_GetHeight(page) - 100, "Name  :");
	status = HPDF_Page_TextOut(page, 50, HPDF_Page_GetHeight(page) - 130, "Gender  :");
	status = HPDF_Page_TextOut(page, 50, HPDF_Page_GetHeight(page) - 160, "Date of brith  :");
	status = HPDF_Page_TextOut(page, 50, HPDF_Page_GetHeight(page) - 190, "ID  :");

	status = HPDF_Page_TextOut(page, 50, HPDF_Page_GetHeight(page) - 260, "Fraction sequence  :");
	status = HPDF_Page_TextOut(page, 50, HPDF_Page_GetHeight(page) - 290, "Treatment plan  :");

	status = HPDF_Page_EndText(page);

	/* save the document to a file */
	HPDF_SaveToFile(pdf, fname);

	/* clean up */
	HPDF_Free(pdf);
}


HBRUSH CMainUI::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  Change any attributes of the DC here   
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		switch (pWnd->GetDlgCtrlID())
		{
		case IDC_RXEDIT2:
			if (IfMoveInPlace[0]==0){ pDC->SetTextColor(RGB(255,255,0)); }
			else if (IfMoveInPlace[0] == 1) { pDC->SetTextColor(RGB(0, 255, 0)); }
			//pDC->SetTextColor(ShowColor);
			pDC->SetBkColor(RGB(255, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			return (HBRUSH)::GetStockObject(BLACK_BRUSH);
			break;
		case IDC_RYEDIT2:
			if (IfMoveInPlace[1] == 0) { pDC->SetTextColor(RGB(255, 255, 0)); }
			else if (IfMoveInPlace[1] == 1) { pDC->SetTextColor(RGB(0, 255, 0)); }
			pDC->SetBkColor(RGB(255, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			return (HBRUSH)::GetStockObject(BLACK_BRUSH);
			break;
		case IDC_RZEDIT2:
			if (IfMoveInPlace[2] == 0) { pDC->SetTextColor(RGB(255, 255, 0)); }
			else if (IfMoveInPlace[2] == 1) { pDC->SetTextColor(RGB(0, 255, 0)); }
			pDC->SetBkColor(RGB(255, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			return (HBRUSH)::GetStockObject(BLACK_BRUSH);
			break;
		case IDC_RAXEDIT2:
			if (IfMoveInPlace[3] == 0) { pDC->SetTextColor(RGB(255, 255, 0)); }
			else if (IfMoveInPlace[3] == 1) { pDC->SetTextColor(RGB(0, 255, 0)); }
			pDC->SetBkColor(RGB(255, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			return (HBRUSH)::GetStockObject(BLACK_BRUSH);
			break;
		case IDC_RAYEDIT2:
			if (IfMoveInPlace[4] == 0) { pDC->SetTextColor(RGB(255, 255, 0)); }
			else if (IfMoveInPlace[4] == 1) { pDC->SetTextColor(RGB(0, 255, 0)); }
			pDC->SetBkColor(RGB(255, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			return (HBRUSH)::GetStockObject(BLACK_BRUSH);
			break;
		case IDC_RAZEDIT2:
			if (IfMoveInPlace[5] == 0) { pDC->SetTextColor(RGB(255, 255, 0)); }
			else if (IfMoveInPlace[5] == 1) { pDC->SetTextColor(RGB(0, 255, 0)); }
			pDC->SetBkColor(RGB(255, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			return (HBRUSH)::GetStockObject(BLACK_BRUSH);
			break;
		default:
			break;
		}
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CMainUI::OnStnClickedPlanchoice()
{
	int nonullstr=0;
	m_plancombobox.GetWindowText(m_report.PlanName);
	if (m_report.PlanName=="")
	{
		return;
	}
	mysql_init(&m_sqlCon);//初始化数据库对象
	if (!mysql_real_connect(&m_sqlCon, "localhost", "root", "123456", "chair001", 3306, NULL, 0))//localhost:服务器地址，可以直接填入IP;root:账号;123:密码;test:数据库名;3306:网络端口  
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}
	else//连接成功则继续访问数据库，之后的相关操作代码基本是放在这里面的
	{
		str.Format(L"SELECT beam1,beam2,beam3,beam4,beam5,beam6,beam7,beam8,beam9,beam10 FROM plan where name = \"%s%s\"", m_report.PatientID, m_report.PlanName);

		const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
		char c1[500] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		//最后进行转换
		const char* cc1 = c1;

		mysql_query(&m_sqlCon, cc1);//从users这个表格查询密码"SELECT password FROM users where id = 1000"
									//获取结果集
		m_res = mysql_store_result(&m_sqlCon);
		m_row2 = mysql_fetch_row(m_res);
		
		for (size_t i = 0; i < 10; i++)
		{
			m_report.BeamAngle[i] = m_row2[i];
			nonullstr = i;
			if ( m_report.BeamAngle[i] == "")
			{			
				break;
			}
		}

		str.Format(L"SELECT beam1,beam2,beam3,beam4,beam5,beam6,beam7,beam8,beam9,beam10 FROM plan where name = \"%s%sN\"", m_report.PatientID,m_report.PlanName);

		wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
		for (int i = 0; i < 500; i++) { c1[i] = 0; }
		wcstombs(c1, wstr, wcslen(wstr));
		//最后进行转换
		cc1 = c1;

		mysql_query(&m_sqlCon, cc1);//从users这个表格查询密码"SELECT password FROM users where id = 1000"
									//获取结果集
		m_res = mysql_store_result(&m_sqlCon);
		m_row2 = mysql_fetch_row(m_res);

		for (size_t i = 0; i < 10; i++)
		{
			m_report.BeamName[i] = m_row2[i];
			nonullstr = i;
			if (m_report.BeamName[i] == "")
			{
				break;
			}
		}
		mysql_close(&m_sqlCon);//关闭Mysql连接
	}	
	if (isfirstshowbeam)
	{
		isfirstshowbeam = false;
		CRect rect; 
		m_beamlist.GetClientRect(&rect);
		m_beamlist.InsertColumn(0, _T("BeamName"), LVCFMT_CENTER, rect.Width() / 3 * 2);
		m_beamlist.InsertColumn(1, _T("ISO/°"), LVCFMT_CENTER, rect.Width() / 3);
	}
	m_beamlist.DeleteAllItems();
	for (size_t i = 0; i < nonullstr; i++)
	{
		m_beamlist.InsertItem(i, m_report.BeamName[i]);
		m_beamlist.SetItemText(i, 1, m_report.BeamAngle[i]);
	}
	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON8);
	((CStatic*)GetDlgItem(IDC_PLANChoice))->SetIcon(m_hIcon);
}


void CMainUI::OnNMDblclkCommlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_beamlist.GetFirstSelectedItemPosition();
	int nItem = m_beamlist.GetNextSelectedItem(pos);
	m_beamlist.SetItemData(nItem, COLOR_YELLOW);
	str=m_beamlist.GetItemText(nItem, 1);
	SetDlgItemText(IDC_360EDIT2, str);

	str = m_beamlist.GetItemText(nItem, 0);
	m_report.BeamName[m_report.BeamNum]=str;

	*pResult = 0;
}


void CMainUI::OnCbnSelchangePlancombo()
{
	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON9);
	((CStatic*)GetDlgItem(IDC_PLANChoice))->SetIcon(m_hIcon);
}

void CMainUI::OnBnClickedButton1()
{
	CString sr;
	mysql_init(&m_sqlCon);//初始化数据库对象
	if (!mysql_real_connect(&m_sqlCon, "localhost", "root", "123456", "chair001", 3306, NULL, 0))//localhost:服务器地址，可以直接填入IP;root:账号;123:密码;test:数据库名;3306:网络端口  
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}
	else//连接成功则继续访问数据库，之后的相关操作代码基本是放在这里面的
	{
	}
	UpdateData(TRUE);
	mysql_query(&m_sqlCon, "SELECT id FROM user2");//从user2这个表格查询id
	//获取结果集
	m_res = mysql_store_result(&m_sqlCon);

	//int i = 0;
	int id = 0;
	while (m_row = mysql_fetch_row(m_res))
	{
		for (size_t i = 0; i < 10000; i++)
		{
			str = m_row[0];
			if (i == _wtof(str.GetBuffer()))
			{
				sr.Format(L"%d", i);
				str.Format(L"UPDATE user2 SET password=AES_ENCRYPT(\"%s\", 'scuyf130916') where id = %d", sr, i);
				const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
				char c1[500] = { 0 };
				wcstombs(c1, wstr, wcslen(wstr));
				const char* cc1 = c1;
				mysql_query(&m_sqlCon, cc1);
			}
		}

	}
	AfxMessageBox(_T("完成!"));
	mysql_close(&m_sqlCon);//关闭Mysql连接
}


void CMainUI::OnStnClickedStaticpexit()
{
	if (!PisOpen)
	{
		if (ChairMode==1)
		{
			AfxMessageBox(_T("In treatflow can't change patient "));
			return;
		}
		CString result;
		PatientExist = false;
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

		GetDlgItemText(IDC_patientIDEDIT, m_report.PatientID);
		//UpdateData(TRUE);
		//查询数据
		(mysql_query(&m_sqlCon, "SELECT id FROM patient"));//从users这个表格查询id
		m_res = mysql_store_result(&m_sqlCon);
		while (m_row = mysql_fetch_row(m_res))
		{
			str = m_row[0];
			if (_wtof(m_report.PatientID.GetBuffer()) == _wtof(str.GetBuffer()))
			{
				PatientExist = true;
			}

		}
		if (!PatientExist)
		{
			AfxMessageBox(_T("There is no information about the patient!"));
			return;
		}

		//str.Format(L"SELECT beam1 FROM plan where name = %s", "plan001");
		str.Format(L"SELECT PatientName,birth,sex,plan1,plan2,plan3,plan4,plan5,plan6,plan7,plan8,plan9,plan10 FROM patient where id = \"%s\"", m_report.PatientID);

		const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
		char c1[500] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		//最后进行转换
		const char* cc1 = c1;

		if (PatientExist)
		{
			mysql_query(&m_sqlCon, cc1);//从users这个表格查询密码"SELECT password FROM users where id = 1000"								//获取结果集
			m_res = mysql_store_result(&m_sqlCon);
			while (m_row = mysql_fetch_row(m_res))
			{
				m_report.PatientName = m_row[0]; str.Format(L"%s%s", L"PatientName:  ", m_report.PatientName); SetDlgItemText(IDC_patientname, str);
				SetDlgItemText(IDC_patientid, L"PatientID:  " + m_report.PatientID);
				m_report.PatientBirth = m_row[1]; str.Format(L"%s%s", L"PatientBirth:  ", m_report.PatientBirth); SetDlgItemText(IDC_patientbirth, str);
				m_report.PatientGender = m_row[2]; str.Format(L"%s%s", L"PatientGender:  ", m_report.PatientGender); SetDlgItemText(IDC_patientsex, str);

				m_plancombobox.ResetContent();
				//m_report.PlanName = m_row[3];
				for (size_t i = 0; i < 10; i++)
				{
					result = m_row[3+i]; 
					//result.Mid(m_report.PatientID.GetLength(),result.GetLength());
					if(result!="")
					{
						m_plancombobox.AddString(result);
					}
					else if (result == "")
					{
						break;
					}
						
				}
			}
		}
		mysql_close(&m_sqlCon);//关闭Mysql连接
		m_beamlist.DeleteAllItems();
		//this->GetDlgItem(IDC_BUTTON11)->EnableWindow(FALSE);
		HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONNO);
		((CStatic*)GetDlgItem(IDC_STATICPExit))->SetIcon(m_hIcon);
		
		PisOpen = true;
		PID = m_report.PatientID;
		this->GetDlgItem(IDC_patientIDEDIT)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_TREATMODEBUTTON)->EnableWindow(TRUE);
		
	}
	else//关闭病人
	{
		if (IDYES == MessageBox(_T("Close the current patient?"), _T("Notice"), MB_YESNO))
		{
			PisOpen = false;
			SetDlgItemText(IDC_patientname, L"PatientName:");
			SetDlgItemText(IDC_patientid, L"PatientID:  ");
			SetDlgItemText(IDC_patientbirth, L"PatientBirth:  ");
			SetDlgItemText(IDC_patientsex, L"PatientGender:  ");
			m_beamlist.DeleteAllItems();
			m_plancombobox.ResetContent();
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONYES);
			((CStatic*)GetDlgItem(IDC_STATICPExit))->SetIcon(m_hIcon);
			m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON9);
			((CStatic*)GetDlgItem(IDC_PLANChoice))->SetIcon(m_hIcon);
			PID = L"Patient ID";
			SetDlgItemTextW(IDC_patientIDEDIT, PID);
			this->GetDlgItem(IDC_patientIDEDIT)->EnableWindow(TRUE);

			this->GetDlgItem(IDC_TREATMODEBUTTON)->EnableWindow(FALSE);
		}
	}
	
}


void CMainUI::OnNMCustomdrawCommlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码  
	NMCUSTOMDRAW nmCustomDraw = pNMCD->nmcd;
	switch (nmCustomDraw.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	{
		if (COLOR_BLUE == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(51, 153, 255);
			pNMCD->clrText = RGB(255, 255, 255);
		}
		else if (COLOR_YELLOW == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(51, 153, 255);      //背景颜色  
			pNMCD->clrText = RGB(255, 255, 255);     //文字颜色  
		}
		else if (COLOR_DEFAULT == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(255, 255, 255);
			pNMCD->clrText = RGB(0, 0, 0);
		}
		else
		{
			//  
		}
		break;
	}
	default:
	{
		break;
	}
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;       //必须有，不然就没有效果  
	*pResult |= CDRF_NOTIFYITEMDRAW;        //必须有，不然就没有效果  
	return;
}

void CMainUI::OnCancel()
{
}


void CMainUI::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	if (::GetKeyState(VK_CONTROL) < 0)
	{
		AfxMessageBox(L"Ctrl 键按下了。");
		return;
	}
	else
	{
		AfxMessageBox(L"Ctrl 键没按下。");
		return;
	}
	LinkToESB();
}
