// DOFComm.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "DOFComm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CDOFComm::CDOFComm(BYTE nDofID,LPCWSTR sz,WORD port)//实例化通信对象
{
	bEmergence = false;	//是否紧急状态

	memset(&ToDOFBuf,0,sizeof(DataToDOF));		//最近将发向运动平台的数据,设置存储空间ToDOFBuf，初始化为0，存储空间的大小为DataToDOF的size
	memset(&FromDOFBuf,0,sizeof(DataToHost));	//最近从运动平台获取到的数据


	ToDOFBuf.nCheckID = nDofID;
	FromDOFBuf.nDOFStatus = dof_stop;

	wcscpy_s(szIP,40,sz);//wcscpy_s是系统函数
	nPort = port;

	nSendCount = 0;
	nRecvCount = 0;
}

CDOFComm::~CDOFComm()
{
}

// CDOFComm 成员函数
bool CDOFComm::Init(HWND hWnd)//建立UDP广播服务器
{
	if (!AfxSocketInit())
		return false;

	if(!Create(SERVER_PORT,SOCK_DGRAM,FD_READ|FD_WRITE))	//SOCK_DGRAM:UDP,SOCK_STREAM :TCP
		return false;
	
	return true;
}

bool CDOFComm::Clean()
{	
	Close();
	return true;
}

bool CDOFComm::IsConnected()//判断当前是否通信连接
{
	return m_hSocket != SOCKET_ERROR;
}

BYTE CDOFComm::Status()	//获取状态
{
	if(!IsConnected())
		return dof_closed;

	if(bEmergence)
		return dof_emg;

    if(nRecvCount==0) FromDOFBuf.nDOFStatus=8;
	return FromDOFBuf.nDOFStatus;
}

bool CDOFComm::SendData(DataToDOF  *pData)//发送数据
{	
	if(pData != NULL && pData->nCheckID == ToDOFBuf.nCheckID && IsConnected())
	{
		if(bEmergence && pData->nCmd != S_CMD_Back2MID)//如果处于紧急状态或者目前位置不是中间态，则返回错误
			return false;

		if(pData != &ToDOFBuf)
		memcpy(&ToDOFBuf,pData,sizeof(DataToDOF));

		nSendCount++;
		int ii=0;
		
		ii=	SendTo(pData,sizeof(DataToDOF),CLIENT_PORT,TARGET_IP) ;//szIP,MSG_DONTROUTE，调用该成员函数sendto的数据发送到特定目标，若成功发送则返回1

		return ii>0;
	}

	return false;
}

void CDOFComm::OnReceive(int nErrorCode)//接收数据
{
	DataToHost dh;
	int a=-1,b=-1;
	a=ReceiveFrom(&dh,sizeof(DataToHost),CString(szIP),nPort,0);

	if((b=ReceiveFrom(&dh,sizeof(DataToHost),CString(szIP),nPort,MSG_DONTROUTE))==int(-1))
	//if(a!=0)
	{
		nRecvCount++;
		ProcData(&dh);
	}
    a=0;
}

void CDOFComm::ProcData(DataToHost *pData)
{
	//if(pData == NULL || pData->nCheckID != ToDOFBuf.nCheckID)
		//return;

	if(pData != &FromDOFBuf)
		memcpy(&FromDOFBuf,pData,sizeof(DataToHost));//复制缓冲区之间的字节。
}

void CDOFComm::CheckDOF()
{
	ToDOFBuf.nCmd = S_CMD_Check;	

	SendData(&ToDOFBuf);
}

void CDOFComm::ToWorking()//切换到工作状态
{
	if(Status() != dof_neutral)	//只有在中立位置才能切换到工作状态
		return;

	ToDOFBuf.nCmd = S_CMD_RUN;
    for (int i=0;i<6;i++)
    {
		ToDOFBuf.DOFs[i]=0.f;
    }
    for (int i=0;i<3;i++)
    {
		ToDOFBuf.Vxyz[i]=0.f;	
		ToDOFBuf.Axyz[i]=0.f;
    }
	SendData(&ToDOFBuf);
}

void CDOFComm::ToNeutral()//切换到中立状态
{
	ToDOFBuf.nCmd = S_CMD_Back2MID;

	SendData(&ToDOFBuf);
}

bool CDOFComm::Emergence()//急停
{
	ToDOFBuf.nCmd = S_CMD_ToMerg;

	bEmergence = true;

	SendData(&ToDOFBuf);

	return true;
}

bool CDOFComm::UnEmergence()//解除急停状态
{
	if(!bEmergence)
		return true;
	
	bEmergence = false;

	ToWorking();

	return true;
}