#include "stdafx.h"
#include "PC2PC.h"


PC2PC::PC2PC(void)
{
	memset(&FromPCBuf,0,sizeof(DataFromPC));		//最近将发向运动平台的数据,设置存储空间ToDOFBuf，初始化为0，存储空间的大小为DataToDOF的size
	memset(&ToPCBuf,0,sizeof(DataToPC));	//最近从运动平台获取到的数据

}


PC2PC::~PC2PC(void)
{
}

bool PC2PC::Init(HWND hWnd)
{
	if (!AfxSocketInit())
		return false;
	//tcp服务器
	//if(!Create(PC_SERVER_PORT,SOCK_STREAM,FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE))	//SOCK_DGRAM:UDP,SOCK_STREAM :TCP
		//return false;

	//Listen();
	//udp服务器
	if(!Create(PC_SERVER_PORT,SOCK_DGRAM,FD_READ|FD_WRITE))	//SOCK_DGRAM:UDP,SOCK_STREAM :TCP
		return false;

	
	return true;
}


bool PC2PC::SendData(DataToPC  *pData)//发送数据
{	
	if(pData != NULL && pData->nCheckID == ToPCBuf.nCheckID && IsConnected())
	{
		//if(bEmergence && pData->nCmd != S_CMD_Back2MID)//如果处于紧急状态或者目前位置不是中间态，则返回错误
			//return false;

		if(pData != &ToPCBuf)
		memcpy(&ToPCBuf,pData,sizeof(DataToPC));

		//nSendCount++;
		int ii=0;
		
			
		ii=	SendTo(pData,sizeof(DataToPC),PC_CLIENT_PORT,PC_TARGET_IP) ;//szIP,MSG_DONTROUTE，调用该成员函数sendto的数据发送到特定目标，若成功发送则返回1

		return ii>0;
	}

	return false;
}

void PC2PC::OnReceive(int nErrorCode)//接收数据
{
	DataFromPC dh;
	int a=-1,b=-1;
	a=ReceiveFrom(&dh,sizeof(DataFromPC),CString(szIP),nPort,0);

	if((b=ReceiveFrom(&dh,sizeof(DataFromPC),CString(szIP),nPort,MSG_DONTROUTE))==int(-1))
	//if(a!=0)
	{
		nRecvCount++;
		ProcData(&dh);
	}
    a=0;
}

void PC2PC::ProcData(DataFromPC *pData)
{
	//if(pData == NULL || pData->nCheckID != ToDOFBuf.nCheckID)
		//return;

	if(pData != &FromPCBuf)
		memcpy(&FromPCBuf,pData,sizeof(DataFromPC));//复制缓冲区之间的字节。
}

bool PC2PC::IsConnected()
{
	return m_hSocket != SOCKET_ERROR;
}
