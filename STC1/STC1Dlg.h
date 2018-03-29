
// STC1Dlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "Cali.h"
#include "Corr.h"
#include "MainUI.h"
//#include "DOFComm.h"

#include "AddPatient.h"
#include "ADDPlan.h"
#include "ADDUSER.h"
enum
{
	NormalMode = 0,
	TreatmentMode = 1,
	QAMode = 2,
};

struct mystruct {
	float vector[4];
};

//与安卓通信的数据结构
struct DatatoAD {
	BYTE AD1;
	BYTE AD2;
	BYTE AD3;
	BYTE AD4;
	float ADstate[6];
	float AD360;
};

struct DataFromAD {
	BYTE AD1;
	BYTE AD2;
	BYTE AD3;
	BYTE AD4;
	float ADstate[6];
	float AD360;
};


// CSTC1Dlg 对话框
class CSTC1Dlg : public CDialogEx
{
// 构造
public:
	CSTC1Dlg(CWnd* pParent = NULL);	// 标准构造函数
	HACCEL  m_hAccel;
	virtual BOOL PreTranslateMessage(MSG *pMsg);
// 对话框数据
	enum { IDD = IDD_STC1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnOK();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tab;
	CMainUI m_mainui;
	CCali m_cali;
	CCorr m_corr; 
	CDOFComm m_dof;
	CMenu menu;

	Attitude attitude;//给出位移指令
	Userdata userdata;
	Onoff onoff;

	Dataexchange datachange;
	//Mode mode;
	Userdi userdi;
	CEdit m_infoedit;
	bool six_enable;//六自由度平台使能标识符
	
	bool friststart;//初次开机标志位
	char S_comMotion;//选择哪种复合运动方式
	float desir_speed[6];
	float desir_acc[6];
	float single_move_speed[6];
	float re_attitude_for_show[6];
	DWORD m_nRecvCount;
    //float single_move_speed_show[6];

	afx_msg void OnBnClickedCtrlbutton();//控制按钮
	afx_msg void OnBnClickedStartbutton();//启动定时器
	afx_msg void OnBnClickedResetbutton();//复位
	afx_msg void OnBnClickedButton6();//初始化
	afx_msg void OnBnClickedButton10();//落下
	afx_msg void OnBnClickedButton9();//运行
	afx_msg void OnBnClickedEstopbutton();//急停
	afx_msg void timerout();
	afx_msg void GetRealData(float data[6])//做xy方向的平移校正
	{
		double x = data[3]; double y = data[4];
		if (x>0.2)
		{
			data[3] = 1.003*x+0.007*y;
		}
		else if (x<-0.2)
		{
			data[3] = 1.004*x+ 0.007*y;
		}
		if (y>0.2)
		{
			data[4] = 1.007*y-0.012*x;
		}
		else if (y<-0.2)
		{
			data[4] = 1.003*y-0.012*x;
		}
	}

	afx_msg void GetXYCORRData(float data[6])//做xy方向的平移校正
	{
		float x = data[3]; float y = data[4];

		data[3] = x - 0.007 *y;
		data[4] = y + 0.012*x;
	};

	bool IfMoveGet;
	CFont m_editFont;
	afx_msg void ChangeUI();

	UINT nMMTimer;

	UINT count1;
	CString result, str;
	UINT item;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);//窗口切换函数

	CString m_hJueDuiWeiZhi2;  //定位以前的数据绝对位置
	CString m_hJueDuiWeiZhi;  //绝对位置

	bool lockflag;			//滑轨锁定标志	
	bool enableflag;		//使能标志
	bool EthLinkOK;			//以太网连接标志
	bool EMSTOP;//紧急停机标志

	bool CanUse;
	
	//bool EStop;
	short Ret;				//PLC数据交换状态标识位	
	CString	Data[8];		//读PLC数据存储区
	double fangxiang;          //旋转方向标志位
	bool LR;          //旋转方向标志位，真表示顺时针，假表示逆时针



	afx_msg void RealAgleRealTime();
	afx_msg CString ToHdata(CString);
	afx_msg CString ToLdata(CString);
	CButton m_enbale_button;
	afx_msg void OnBnClickedEnbutton();
	afx_msg void beginmove();
	afx_msg void OnBnClickedLockbutton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// 使能按钮控制变量
	CColorButton m_Startcontrol;
	CColorButton m_Encontrol;
	CColorButton m_Lockcontrol;
	CColorButton m_ESTOPcontrol;


	afx_msg void CtrlSpeed();
	float maxtime ;//一次复杂运动需要花费的最大时间
	float speed[6];//调整速度的数值，用以存放运动以前的坐标值


	afx_msg void JudgeAction();
	afx_msg void ActionTip();
	afx_msg void OnBnClickedLogoutbutton();
	afx_msg void OnBnClickedPdfbutton();
	afx_msg void On32775();
	afx_msg void OnSavereport();
	afx_msg void OnSoftwareinfo();

	//char* CString2char(CString str);
	const char* CString2char(CString str)
	{
		const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
		char c1[256] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		const char* fname = c1;
		return c1;
	}
	afx_msg void OnMenuExit();
	afx_msg void OnStnClickedEnStatic();
	afx_msg void OnEditResetpassword();
	afx_msg void OnBnClickedButton13();
	MYSQL m_sqlCon;
	MYSQL_RES* m_res;         //查询的返回结果集
	MYSQL_ROW m_row;          //获取每一条记录
	afx_msg void OnBnClickedButton2();
	afx_msg void OnInputpatientAddpatient();
	afx_msg void OnInputpatientEditpatient();
	afx_msg void OnInputplanAddplan();
	afx_msg void OnInputplanEditplan();
	afx_msg void OnUserAdduser();
	afx_msg void OnBnClickedmovebeginbutton();
};
