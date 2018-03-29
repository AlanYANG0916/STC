#pragma once
#include "afxsock.h"

const UINT PC_SERVER_PORT = 1000;
const UINT PC_CLIENT_PORT = 2000;
#define PC_TARGET_IP L"192.168.0.131"

struct DataToPC
{
	BYTE nCheckID;
	BYTE nDOFStatus;
	BYTE nRev0;
	BYTE nRev1;
	
	float attitude[6];		//实际姿态
	float para[6];				//错误代码
	float motor_code[6];		//电机码值
	float pcctrl[6];//工控机的指令

	int IsCtrl2Pc;
};

struct DataFromPC
{
	BYTE nCheckID;			//标识位
	BYTE nCmd;				//状态指令
	BYTE nAct;				//特效指令
	BYTE nReserved;	//保留

	float DOFs[6];			//{横摇，纵倾，航向，前向，侧向，升降}
	float Vxyz[3];			//{前向，侧向，升降}，向右为正，向下为正（速度）
	float Axyz[3];			//...（加速度）
};



class PC2PC :public CAsyncSocket
{
public:
	PC2PC(void);
	~PC2PC(void);

	wchar_t szIP[40];
	UINT nPort;

	bool Init(HWND hWnd);
	bool SendData(DataToPC  *pData);			
	void ProcData(DataFromPC *pData);
	bool IsConnected();
	virtual void OnReceive(int nErrorCode);

	
public:
	DataFromPC	FromPCBuf;				//最近将发向运动平台的数据(从工作站到工控站)
	DataToPC	ToPCBuf;		//最近从运动平台获取到的数据(从工控站到工作站)
	DWORD nSendCount;
	DWORD nRecvCount;
};

