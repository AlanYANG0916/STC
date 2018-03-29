// DOFComm.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include <deque> ///////////////////////////////////////////////////////////////////////////

const UINT SERVER_PORT = 10000;
const UINT CLIENT_PORT = 5000;
#define TARGET_IP L"192.168.0.125"
// const UINT CLIENT_PORT = 5500;		//虚拟下位机用
// #define TARGET_IP L"192.168.0.131"	//虚拟下位机用
#define MIN_PAR			0.000001f
#define ToDeg(rad)		(rad * 180.0 / PI)
#define ToRad(deg)		(deg / 180.0 * PI)
#define PI                    3.1416f
#define TIMER             10
#define MAXSPEED		30.0f
#define MAXANGV		10.f
#define att_send_flag	1.f
#define ljg	                0.f       //数据采集



struct Userdi
{
	BYTE pass_di_1;
	BYTE pass_di_2;
};

struct Mode
{
	int m_nVarIdx;
	int  flag_count;		//标识是哪种工作方式
	int  m_nAct;
};

struct Onoff
{
	char n_solution;	//是否正解
	char data_save;		//是否保存数据
	char set_button;	//标识按钮是否有效
	char back2mid;		//是否回中位
};

struct Sinmovetime
{
	float max;
	float test;
};
struct Userdata
{
	float send_do[6];		//数字输出控件变量
	float m_para[18];		//正弦姿态运动控件变量
	float eRR[6];			//错误代码显示
	float m_force[6];		//实际负载
	float m_eDOF[6];		//实际姿态
	BYTE ca_send_do[6];
	BYTE total_send_do;
};

enum ComMotion
{
	ComMotion_1=1,//复合运动一
	ComMotion_2=2,//复合运动二
	ComMotion_3=3,//复合运动三
	ComMotion_4=4 //复合运动四
};

/////////////////////////////////////////////////
typedef struct PlatAttPara
{
	float attu0;   //姿态一
	float attu1;   //姿态二
	float attu2;   //姿态三
	float attu3;   //姿态四
	float attu4;   //姿态五
	float attu5;   //姿态六
}PlatAttParaStruct;

typedef std::deque<PlatAttParaStruct>PlatAttParaDeque;
/////////////////////////////////////////////////

struct Dataexchange
{
	double eX;
	double eY;
	double eZ;
	double eH;
	double eP;
	double eR;
	
	double eAx;
	double eAy;
	double eAz;
	double eVx;
	double eVy;
	double eVz;
};

struct Attitude
{
	float end[6];		//单步运动中，由一个姿态切换到另一个姿态时的当前姿态
	float mid[6];		//单步运动中，由一个姿态切换到另一个姿态时的目标姿态
	float sen[6];		//单步运动中的指令姿态

	float iec[6];
};


struct DataToHost
{
	BYTE nCheckID;
	BYTE nDOFStatus;
	BYTE nRev0;
	BYTE nRev1;
	
	float attitude[6];		//
	float para[6];				//错误代码
	float motor_code[6];		//电机码值,//[0,1,2]理论姿态，[3,4,5]实际姿态
};

//运动平台状态定义
enum
{
	dof_stop		= 0,		//停止状态
	dof_sys_moving	= 1,		//运动到中立状态
	dof_neutral		= 2,		//中立状态
	dof_working		= 3,		//工作状态
	dof_setconf		= 3,		//修改参数

	dof_select=8,

	dof_check_id = 55,

	dof_closed = 253,
	dof_emg = 254,
	dof_err = 255
};

struct DataToDOF
{
	BYTE nCheckID;			//标识位
	BYTE nCmd;				//状态指令
	BYTE nAct;				//特效指令
	BYTE nReserved;	//保留

	float DOFs[6];			//{横摇，纵倾，航向，前向，侧向，升降}
	float Vxyz[3];			//{前向，侧向，升降}，向右为正，向下为正（速度）
	float Axyz[3];			//...（加速度）
};

enum COUNT_FLAG
{
	F_None=0,  
    F_SinMove=1,		//单步运行命令
	F_SinMotion=2,		//单轴正弦运动	
	F_ComMotion=3,		//复合运动一       
	F_DataMotion=4,		//文件运动       
	F_Jog=5,		//单缸手动 
};

//平台工作状态指令集
enum M_nCmd
{
	S_CMD_RUN=0,			//正常运行
	S_CMD_Check=1,
	S_CMD_Back2MID=2,		// 底回中立位
	S_CMD_ToMerg=3,			//紧急停机
	S_CMD_ToWork=4,			// 握手协议
	S_CMD_JOG=5,			//单缸手动
	S_CMD_Work=6,			//由低位上升到中位
	S_CMD_Stop=7,			//由中位回落到低位
	S_CMD_ChaConf=8,		//配置驱动器信息
	S_CMD_HOM=9,         
};

enum
{
	S_OFF = 0,
	S_ON  = 1
};

enum
{
	MOTOR_NONE = 0,
	MOTOR_LXM23 = 1,
	MOTOR_LXM32 = 2
};
//特殊动作定义


class CDOFComm : public CAsyncSocket
{
public:
	CDOFComm(BYTE nDofID = 55,LPCWSTR szIPs = TARGET_IP,WORD nPort = CLIENT_PORT);
	virtual ~CDOFComm();
	bool Init(HWND hWnd);				//初始化，并建立连接
	bool Clean();								
	bool IsConnected();
	BYTE Status();								//获取状态
	wchar_t szIP[40];
	UINT nPort;

public:
	bool SendData(DataToDOF  *pData);			
	void ProcData(DataToHost *pData);
	void CheckDOF();							//发送检测命令
	void ToNeutral();							//到中立位
	void ToWorking();
	bool Emergence();
	bool UnEmergence();	
	virtual void OnReceive(int nErrorCode);
protected:
	bool bEmergence;							//是否紧急状态
public:
	DataToDOF	ToDOFBuf;				//最近将发向运动平台的数据
	DataToHost	FromDOFBuf;			//最近从运动平台获取到的数据
	DWORD nSendCount;
	DWORD nRecvCount;


	static double DOFdata;
};


