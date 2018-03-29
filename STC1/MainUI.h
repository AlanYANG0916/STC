#pragma once
#include "DOFComm.h"
#include "PC2PC.h"
#include <list>
#include <fstream>
#include "afxwin.h"
#include "afxcmn.h"
#include "SixDof.h"

#include <iostream>
#include <afxinet.h> //访问网络所需要的函数的头文件。

//#include "STC1Dlg.h"
#include "ColorButton.h"
#include "Translate.h"
#include "Translation.h"
#include "Login.h"

#import "D:\\msxml4.dll"
#import "C:\\Program Files (x86)\\Common Files\\MSSoap\\Binaries\\MSSOAP30.dll"

#include <string>  
#include <Windows.h>  
#include "mysql.h"//这个没什么好说的，mysql头文件自然要包含
//using namespace MSXML2;
using namespace MSSOAPLib30;
using std::string;

//#pragma comment(lib,"D:\documents\visual studio 2012\Projects\STC1（工控机）\Release")
//extern "C" __declspec(dllexport) void SoluteAttitude(float *m_Cylend, float *m_att);//正解
const COLORREF BLACK = RGB(1, 1, 1);
const COLORREF WHITE = RGB(255, 255, 255);
const COLORREF DKGRAY = RGB(128, 128, 128);
const COLORREF LLTGRAY = RGB(220, 220, 220);
const COLORREF LTGRAY = RGB(192, 192, 192);
const COLORREF YELLOW = RGB(255, 255, 0);
const COLORREF DKYELLOW = RGB(128, 128, 0);
const COLORREF RED = RGB(255, 0, 0);
const COLORREF DKRED = RGB(128, 0, 0);
const COLORREF BLUE = RGB(0, 0, 255);
const COLORREF LBLUE = RGB(192, 192, 255);
const COLORREF DKBLUE = RGB(0, 0, 128);
const COLORREF CYAN = RGB(0, 255, 255);
const COLORREF DKCYAN = RGB(0, 128, 128);
const COLORREF GREEN = RGB(0, 255, 0);
const COLORREF DKGREEN = RGB(0, 128, 0);
const COLORREF MAGENTA = RGB(255, 0, 255);
const COLORREF DKMAGENTA = RGB(128, 0, 128);
// CMainUI 对话框
#define COLOR_DEFAULT 0 //默认颜色
#define COLOR_YELLOW 1
#define COLOR_BLUE 2


//治疗相关的信息保存
struct TreatReportData {
	int BeamNum;//束流数量
	CString BeamName[50];//束流名称
	CString BeamAngle[50];
	CString BeamTime[50];//束流应用时间
	CString BeamOper[50];//束流操作员
	double PlanBeamInfo[50][6];//束流位置的信息
	double ReBeamInfo[50][6];//真实位置的信息

	CString PatientName;
	CString PatientID;
	CString PatientBirth;
	CString PatientGender;

	CString SequenceName;
	CString PlanName;
};

struct CorrectData{
	double PV0Corr=270;
	double PV1Corr=270;

	double PV1Angle=270;
	double Beam[20];
};


class CMainUI : public CDialogEx
{
	DECLARE_DYNAMIC(CMainUI)

public:
	CMainUI(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainUI();

	//HACCEL  m_hAccel;
	//virtual BOOL PreTranslateMessage(MSG *pMsg);
	//afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	//afx_msg void OnDestroy();
// 对话框数据
	enum { IDD = IDD_MAINDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//六自由度平台控制对象
	

	afx_msg void OnBnClickedAlignerbutton();//初始化DICOM
	afx_msg void OnBnClickedTreatmodebutton();//切换到treatmode
	afx_msg void OnBnClickedCorrbutton();//六维度校正
	afx_msg void REOnBnClickedCorrbutton();
	afx_msg void OnBnClickedTreatfinishbutton();//治疗完成
	afx_msg void OnOK();
	afx_msg void OnCancel();
	double FIXED_coordinate_value[7];
	

	double Transdata[6];
	double TransLatedata[6];//平移对准的数据保存区

	double Correctdata[6];//六维度校正数据保存区
	//double ref[6];//参考点的数值

	Translate trans;
	

	bool EStop;
	bool lockflag2;
	//PLC通信所用对象
	


		//安卓手机与电脑通信所用的TCP/IP对象
		//HTREEITEM m_Root;
		//CMySocket m_Svr;
		//CClientSock* pNewConn = new CClientSock;
		////CClientSock pNewConn;
		//void OnAccept(int nErrCode);
		//void OnClientClose(int nErrCode, CClientSock* pConn);
		//void OnRcv(CClientSock* pConn);
		//afx_msg void OnBtnStart();
		//afx_msg void OnBtnStop();
		//bool IsServerstart;
		//bool IsPhoneLink;
		//int SENDTOADcount;
		CString str;

		CString Pname, PID;
		afx_msg void OnBnClickedUpbutton();
		afx_msg void OnBnClickedDownbutton();


		afx_msg void OnEnSetfocuspatientidedit();

		afx_msg void OnEnKillfocuspatientidedit();


		
		 afx_msg void OnBnClickedButton11();
		 afx_msg void LinkToESB();
		 afx_msg void LinkToESB2();


		 afx_msg void ReadConfig();//读取配置文件
		 double ULimitInTTS[6];//在tts中对六维度的限制
		 double FLimitInTTS[6];//在tts中对六维度的限制,下限

		 CString GetHttpCode(CString &url);

		 CColorButton m_Lockcontrol2;
		 CColorButton m_EStopcontrol;
		 //CColorButton m_treatmentmode;

		 afx_msg void OnSavereport();


		 //char* CString2char(CString str)
		 //{
			// const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
			// char c1[256] = { 0 };
			// wcstombs(c1, wstr, wcslen(wstr));
			// //const char* fname = c1;
			// return c1;
		 //}
		 CListCtrl m_beamlist;
		 afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

		 MYSQL m_sqlCon;
		 MYSQL_RES* m_res;         //查询的返回结果集
		 MYSQL_ROW m_row;          //获取每一条记录
		 MYSQL_ROW m_row2;          //获取每一条记录
		 bool PatientExist;
		 bool isfirstshowbeam;
		 CComboBox m_plancombobox;
		 afx_msg void OnStnClickedPlanchoice();
		 CEdit m_patientidedit;
		 afx_msg void OnNMDblclkCommlist(NMHDR *pNMHDR, LRESULT *pResult);
		 afx_msg void OnCbnSelchangePlancombo();
		 afx_msg void OnBnClickedButton1();
		 
		 CStatic m_tr_wf_box;
		 afx_msg void OnStnClickedStaticpexit();
		 afx_msg void OnNMCustomdrawCommlist(NMHDR *pNMHDR, LRESULT *pResult);
		 //double FOR360corr[4];
		 afx_msg void OnBnClickedButton3();

		 double vector[6];
		 double sssForZAxisCorr[6];
};
