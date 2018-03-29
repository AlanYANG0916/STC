// Cali.cpp : 实现文件
//

#include "stdafx.h"
#include "STC1.h"
#include "Cali.h"
#include "MainUI.h"
#include "afxdialogex.h"
#include "STC1Dlg.h"
#include "QAIntPutDlg.h"

// CCali 对话框
//全局变量
 float single_move_speed_show[6];
 float m_para[18];
 int m_radiogroup2;
 extern double M_COORD[6];

 extern CString QAInPutData[8];
 extern unsigned long int BufferStopCount;
 extern bool BufferStop;
 extern UINT count0;     //主时钟计数器
 extern Sinmovetime sinmovetime;
 extern Mode mode;
IMPLEMENT_DYNAMIC(CCali, CDialogEx)

CCali::CCali(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCali::IDD, pParent)
	, m_radiogroup(0)
	, m_radiogroup3(0)
	, m_singlestep(0)
	, m_staytime(0)
{
	m_radiogroup2 = 1;
	m_commnum=0;
	firstlist = true;
	m_singlestep=0;
	m_staytime=1;
	m_commattuide.attu0=0.f;
	m_commattuide.attu1=0.f;
	m_commattuide.attu2=0.f;
	m_commattuide.attu3=0.f;
	m_commattuide.attu4=0.f;
	m_commattuide.attu5=0.f;
	m_commattuide.staytime=1.f;
	//CRect rect;     
    // 获取编程语言列表视图控件的位置和大小   
    //m_commlist.GetClientRect(&rect);
	//m_commlist.InsertColumn(0, _T("语言"), LVCFMT_CENTER, rect.Width()/3, 0);   
    //m_commlist.InsertColumn(1, _T("2012.02排名"), LVCFMT_CENTER, rect.Width()/3, 1);   
    //m_commlist.InsertColumn(2, _T("2011.02排名"), LVCFMT_CENTER, rect.Width()/3, 2);
	for (size_t i = 0; i < 6; i++)
	{
		mid[i]=0;
		m_speed[i]=1;
		single_move_speed_show[i] = 1;
		if (i>2)
		{
			m_speed[i] = 10;
			single_move_speed_show[i] = 10;
		}
	}
	m_speed[6] = 5;
	mid[6] = 0;
	for (size_t i = 0; i < 18; i++)
	{
		m_para[i] = 0;
	}
	//OnBnClickedButton9();
}

CCali::~CCali()
{
}

void CCali::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMMLIST, m_commlist);

	//DDX_Radio(pDX, IDC_XRADIO, m_radiogroup);
	DDX_Radio(pDX, IDC_TTSRADIO, m_radiogroup2);
	DDX_Radio(pDX, IDC_SMRADIO, m_radiogroup3);
	//DDX_Text(pDX, IDC_SINGLESTEPEDIT, m_singlestep);
	//DDX_Text(pDX, IDC_STAYTIMEEDIT, m_staytime);

	DDX_Text(pDX, IDC_XEDIT3, mid[0]);
	DDX_Text(pDX, IDC_YEDIT3, mid[1]);
	DDX_Text(pDX, IDC_ZEDIT3, mid[2]);
	DDX_Text(pDX, IDC_AXEDIT3, mid[3]);
	DDX_Text(pDX, IDC_AYEDIT3, mid[4]);
	DDX_Text(pDX, IDC_AZEDIT3, mid[5]);
	DDX_Text(pDX, IDC_AZ360EDIT2, mid[6]);

	DDX_Text(pDX, IDC_SPEEDXEDIT, m_speed[0]);
	DDX_Text(pDX, IDC_SPEEDYEDIT, m_speed[1]);
	DDX_Text(pDX, IDC_SPEEDZEDIT, m_speed[2]);
	DDX_Text(pDX, IDC_SPEEDAXEDIT, m_speed[3]);
	DDX_Text(pDX, IDC_SPEEDAYEDIT, m_speed[4]);
	DDX_Text(pDX, IDC_SPEEDAZEDIT, m_speed[5]);
	DDX_Text(pDX, IDC_SPEEDAZ360EDIT, m_speed[6]);
	
}


BEGIN_MESSAGE_MAP(CCali, CDialogEx)
	//ON_BN_CLICKED(IDC_NEXTBUTTON, &CCali::OnBnClickedNextbutton)
	//ON_BN_CLICKED(IDC_XRADIO, &CCali::OnBnClickedXradio)
	ON_BN_CLICKED(IDC_SAVEBUTTON, &CCali::OnBnClickedSavebutton)
	ON_BN_CLICKED(IDC_CLEANCOMMBUTTON, &CCali::OnBnClickedCleancommbutton)
	ON_BN_CLICKED(IDC_CTRLBUTTON2, &CCali::OnBnClickedCtrlbutton2)
	ON_BN_CLICKED(IDC_SINMRADIO, &CCali::OnBnClickedSinmradio)
	ON_BN_CLICKED(IDC_SMRADIO, &CCali::OnBnClickedSmradio)
	ON_BN_CLICKED(IDC_BUFFERSTOPBUTTON, &CCali::OnBnClickedBufferstopbutton)
	ON_BN_CLICKED(IDC_TTSRADIO, &CCali::OnBnClickedTtsradio)
	ON_BN_CLICKED(IDC_FIXRADIO, &CCali::OnBnClickedFixradio)
	ON_BN_CLICKED(IDC_BUTTON6, &CCali::OnBnClickedButton6)
	ON_NOTIFY(NM_RCLICK, IDC_COMMLIST, &CCali::OnNMRClickCommlist)
	ON_BN_CLICKED(IDC_BUTTON9, &CCali::OnBnClickedButton9)
	ON_STN_CLICKED(IDC_STATIC_PITCH_UP, &CCali::OnStnClickedStaticPitchUp)
	ON_STN_CLICKED(IDC_STATIC_PITCH_DOWN, &CCali::OnStnClickedStaticPitchDown)
	ON_STN_CLICKED(IDC_STATIC_ROLL_UP, &CCali::OnStnClickedStaticRollUp)
	ON_STN_CLICKED(IDC_STATIC_ROLL_DOWN, &CCali::OnStnClickedStaticRollDown)
	ON_STN_CLICKED(IDC_STATIC_ISO_UP, &CCali::OnStnClickedStaticIsoUp)
	ON_STN_CLICKED(IDC_STATIC_ISO_DOWN, &CCali::OnStnClickedStaticIsoDown)
	ON_STN_CLICKED(IDC_STATIC_LAT_UP, &CCali::OnStnClickedStaticLatUp)
	ON_STN_CLICKED(IDC_STATIC_LAT_DOWN, &CCali::OnStnClickedStaticLatDown)
	ON_STN_CLICKED(IDC_STATIC_LONG_UP, &CCali::OnStnClickedStaticLongUp)
	ON_STN_CLICKED(IDC_STATIC_LONG_DOWN, &CCali::OnStnClickedStaticLongDown)
	ON_STN_CLICKED(IDC_STATIC_VERT_UP, &CCali::OnStnClickedStaticVertUp)
	ON_STN_CLICKED(IDC_STATIC_VERT_DOWN, &CCali::OnStnClickedStaticVertDown)
	ON_STN_CLICKED(IDC_STATIC_360_UP, &CCali::OnStnClickedStatic360Up)
	ON_STN_CLICKED(IDC_STATIC_360_DOWN, &CCali::OnStnClickedStatic360Down)
	ON_BN_CLICKED(IDC_BUTTON1, &CCali::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCali 消息处理程序


void CCali::OnBnClickedNextbutton()
{
	// TODO: 在此添加控件通知处理程序代码
	m_commnum++;
	UpdateData(true);

	if(firstlist )
	{
		//int a=CMainUI::m_pc2pc.FromPCBuf.Vxyz[1];
		firstlist = false;
		CRect rect;     
		// 获取编程语言列表视图控件的位置和大小   
		m_commlist.GetClientRect(&rect);
		m_commlist.InsertColumn(0, _T("Name"), LVCFMT_CENTER, rect.Width() / 9*2);
		m_commlist.InsertColumn(1, _T("picth/°"), LVCFMT_CENTER, rect.Width() / 9);
		m_commlist.InsertColumn(2, _T("roll/°"), LVCFMT_CENTER, rect.Width() / 9);
		m_commlist.InsertColumn(3, _T("iso/°"), LVCFMT_CENTER, rect.Width() / 9);
		m_commlist.InsertColumn(4, _T("lat/mm"), LVCFMT_CENTER, rect.Width()/9); 
		m_commlist.InsertColumn(5, _T("long/mm"), LVCFMT_CENTER, rect.Width()/9);   
		m_commlist.InsertColumn(6, _T("vert/mm"), LVCFMT_CENTER, rect.Width()/9);
		m_commlist.InsertColumn(7, _T("iso360"), LVCFMT_CENTER, rect.Width() / 9);
		
	}

	switch (m_radiogroup)
	{
	case 0:
		m_commattuide.attu0=m_singlestep;
		m_commattuide.staytime=m_staytime;
		commctrl(m_commnum-1);
		break;
	case 1:
		m_commattuide.attu1=m_singlestep;
		m_commattuide.staytime=m_staytime;
		commctrl(m_commnum-1);
		break;
	case 2:
		m_commattuide.attu2=m_singlestep;
		m_commattuide.staytime=m_staytime;
		commctrl(m_commnum-1);
		break;
	case 3:
		m_commattuide.attu3=m_singlestep;
		m_commattuide.staytime=m_staytime;
		commctrl(m_commnum-1);
		break;
	case 4:
		m_commattuide.attu4=m_singlestep;
		m_commattuide.staytime=m_staytime;
		commctrl(m_commnum-1);
		break;
	case 5:
		m_commattuide.attu5=m_singlestep;
		m_commattuide.staytime=m_staytime;
		commctrl(m_commnum-1);
		break;
	case 6:
		m_commattuide.attu360 = m_singlestep;
		m_commattuide.staytime = m_staytime;
		commctrl(m_commnum - 1);
		break;
	}
	
}

void CCali::commctrl(int number)
{
	CString str;
	//m_commlist.InsertItem(number, _T("0"));
	str.Format(_T("%s"),m_commattuide.QAName);
	m_commlist.InsertItem(number, str);// m_commattuide.QAName);
	str.Format(_T("%.2f"),m_commattuide.attu0);
	m_commlist.SetItemText(number,1,str);
	str.Format(_T("%.2f"),m_commattuide.attu1);
	m_commlist.SetItemText(number,2,str);
	str.Format(_T("%.2f"),m_commattuide.attu2);
	m_commlist.SetItemText(number,3,str);
	str.Format(_T("%.2f"),m_commattuide.attu3);
	m_commlist.SetItemText(number,4,str);
	str.Format(_T("%.2f"),m_commattuide.attu4);
	m_commlist.SetItemText(number,5,str);
	str.Format(_T("%.2f"), m_commattuide.attu5);
	m_commlist.SetItemText(number,6, str);
	str.Format(_T("%.1f"),m_commattuide.attu360);
	m_commlist.SetItemText(number,7,str);
}


void CCali::OnBnClickedXradio()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	switch (m_radiogroup)
	{
	default:
		break;
	}
}

void CCali::OnBnClickedSavebutton()
{
	//CString defaultDir = L"D:\FileTest";   //默认打开的文件路径 
 //   CString fileName = L"test";         //默认打开的文件名  
 //   CString filter = L" 文本文件（*.csv） |*.csv| 文本文件（*.txt） | *.txt ||";   //文件过虑的类型  
 //   CFileDialog saveFileDlg(2,defaultDir, fileName, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, filter, this);  
 //   saveFileDlg.GetOFN().lpstrInitialDir = L"E:\\FileTest\\test";  
 //   INT_PTR result = saveFileDlg.DoModal();  
	CString filePath;// = defaultDir + "\\" + fileName;
 //   if(result == IDOK) {  
 //       filePath = saveFileDlg.GetPathName();
 //   }  
	if (m_radiogroup2==1)
	{
		filePath = L"./QASetPoint.csv"; if (m_commlist.GetItemCount() > 0)
		{
			//CString strFileName = _T("多步指令.csv");  
			CFile file;

			if (!file.Open(filePath, CFile::modeCreate | CFile::modeWrite))
			{
				MessageBox(_T("Load QA point fail"), _T("提示"));
				return;
			}

			CString strHead = _T("NAME,pitch,roll,iso,lat,long,vert,360iso,\r\n");

			file.Write(strHead, 2 * strHead.GetLength());


			for (int nItem = 0; nItem<m_commlist.GetItemCount(); nItem++)
			{
				CString strData;
				for (int i = 0; i<8; i++)
				{
					strData += m_commlist.GetItemText(nItem, i);

					if (i == 7)
					{
						strData += _T("\r\n");
					}
					else
					{
						strData += _T(",");
					}
				}
				file.Write(strData, 2 * strData.GetLength());
			}
			file.Close();
		}

	}else if (m_radiogroup2 == 0)
	{
		filePath = L"./QASetPoint2.csv";
		if (m_commlist.GetItemCount() > 0)
		{
			//CString strFileName = _T("多步指令.csv");  
			CFile file;

			if (!file.Open(filePath, CFile::modeCreate | CFile::modeWrite))
			{
				MessageBox(_T("Load QA point fail"), _T("提示"));
				return;
			}

			CString strHead = _T("NAME,pitch,roll,iso,lat,long,vert,\r\n");

			file.Write(strHead, 2 * strHead.GetLength());


			for (int nItem = 0; nItem<m_commlist.GetItemCount(); nItem++)
			{
				CString strData;
				for (int i = 0; i<7; i++)
				{
					strData += m_commlist.GetItemText(nItem, i);

					if (i == 6)
					{
						strData += _T("\r\n");
					}
					else
					{
						strData += _T(",");
					}
				}
				file.Write(strData, 2 * strData.GetLength());
			}
			file.Close();
		}
	}
	

	
}


void CCali::OnBnClickedCleancommbutton()
{
	// TODO: 在此添加控件通知处理程序代码
	//if (MessageBox.Show("你确定关闭窗口吗?","Application",MessageBoxButtons.YesNo,MessageBoxIcon.Information)==DialogResult.No)
            //{
                //e.Cancel = true;
	//OnBnClickedButton11();
            //}
	if(MessageBox(L"是否确认要清空当前所编辑的指令？",L"提示！", MB_OKCANCEL )==1)
	{ 
		//for (int i=7;i>-1;i--)
		//{
			//m_commlist.DeleteColumn(i);
		//}
		for (int i=m_commnum;i>-1;i--)
		{
			m_commlist.DeleteItem(i);
		}
		m_commnum=0;
		//for (size_t i = 0; i < length; i++)
		//{
			m_commattuide.attu0=0;
			m_commattuide.attu1 = 0;
			m_commattuide.attu2 = 0;
			m_commattuide.attu3 = 0;
			m_commattuide.attu4 = 0;
			m_commattuide.attu5 = 0;
			m_commattuide.attu360 = 0;
		//}

	}
}


void CCali::OnBnClickedCtrlbutton2()
{
	UpdateData(TRUE);
	CString str;
	double TTS_coordinate_value[6];
	if (m_radiogroup2==0)//iec table top坐标系下
	{
		//this->GetDlgItem(IDC_CORRBUTTON)->EnableWindow(FALSE);
		double NUM;
		GetDlgItemText(IDC_XEDIT3, str);
		NUM = _wtof(str.GetBuffer());
		TTS_coordinate_value[0] = NUM;
		GetDlgItemText(IDC_YEDIT3, str);
		NUM = _wtof(str.GetBuffer());
		TTS_coordinate_value[1] = NUM;
		GetDlgItemText(IDC_ZEDIT3, str);
		NUM = _wtof(str.GetBuffer());
		TTS_coordinate_value[2] = 270;
		GetDlgItemText(IDC_AXEDIT3, str);
		NUM = _wtof(str.GetBuffer());
		TTS_coordinate_value[3] = NUM ;
		GetDlgItemText(IDC_AYEDIT3, str);
		NUM = _wtof(str.GetBuffer());
		TTS_coordinate_value[4] = NUM ;
		GetDlgItemText(IDC_AZEDIT3, str);
		NUM = _wtof(str.GetBuffer());
		TTS_coordinate_value[5] = NUM ;

		//在TTS中限位
		if ((TTS_coordinate_value[0]>360) || (TTS_coordinate_value[1]>360) || (TTS_coordinate_value[2]>360))
		{
			AfxMessageBox(_T("输入数值存在错误，请检查！"));
			return;
		}

		double vector[6];
		//trans.Correct3(FIXED_coordinate_value, vector);
		//trans.init();
		//trans.CorrectinTTS(Correctdata, vector);
		Translation translation;
		translation.Offset[3] = M_COORD[3];
		translation.Offset[4] = M_COORD[4];
		translation.Offset[5] = M_COORD[5];
		translation.MatrixToSolve(TTS_coordinate_value, vector,0);


		CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针 
		str.Format(L"%.3f", vector[0]); tab->SetDlgItemTextW(IDC_XEDIT, str);
		str.Format(L"%.3f", vector[1]); tab->SetDlgItemTextW(IDC_YEDIT, str);
		str.Format(L"%.3f", vector[2] - 270); tab->SetDlgItemTextW(IDC_ZEDIT, str);
		str.Format(L"%.3f", vector[3] ); tab->SetDlgItemTextW(IDC_AXEDIT, str);
		str.Format(L"%.3f", vector[4] ); tab->SetDlgItemTextW(IDC_AYEDIT, str);
		str.Format(L"%.3f", vector[5] ); tab->SetDlgItemTextW(IDC_AZEDIT, str);

		GetDlgItemText(IDC_ZEDIT3, str);
		NUM = _wtof(str.GetBuffer());
		NUM = 270 - NUM;
		if (NUM>180)
		{
			NUM -= 360;
		}
		if (NUM<-180)
		{
			NUM += 360;
		}
		str.Format(L"%.1f", NUM); tab->SetDlgItemTextW(IDC_AZ360EDIT, str);
		tab->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CTRLBUTTON, BN_CLICKED), NULL);

	}
	else if (m_radiogroup2 == 1)//table support坐标系下
	{
		if (m_radiogroup3==0)//单步运动
		{
			CString str;
			CTabCtrl *tab = (CTabCtrl*)GetParent();//获取父窗口即tab控件指针 
			str.Format(L"%.3f", mid[0]); tab->SetDlgItemTextW(IDC_XEDIT, str);
			str.Format(L"%.3f", mid[1]); tab->SetDlgItemTextW(IDC_YEDIT, str);
			str.Format(L"%.3f", mid[2]); tab->SetDlgItemTextW(IDC_ZEDIT, str);
			str.Format(L"%.3f", mid[3]); tab->SetDlgItemTextW(IDC_AXEDIT, str);
			str.Format(L"%.3f", mid[4]); tab->SetDlgItemTextW(IDC_AYEDIT, str);
			str.Format(L"%.3f", mid[5]); tab->SetDlgItemTextW(IDC_AZEDIT, str);
			str.Format(L"%.1f", mid[6]); tab->SetDlgItemTextW(IDC_AZ360EDIT, str);
			for (size_t i = 0; i < 6; i++)
			{
				//single_move_speed_show[i] = m_speed[i];
			}
			//str.Format(L"%.1f", single_move_speed_show[5]); tab->SetDlgItemTextW(IDC_AZ360EDIT, str);
			tab->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CTRLBUTTON, BN_CLICKED), NULL);
		}
		else if (m_radiogroup3 == 1)//正弦运动
		{
			CString str;
			double NUM;
			GetDlgItemText(IDC_XEDIT3, str); NUM = _wtof(str.GetBuffer()); m_para[0] = NUM;
			GetDlgItemText(IDC_YEDIT3, str); NUM = _wtof(str.GetBuffer()); m_para[1] = NUM;
			GetDlgItemText(IDC_ZEDIT3, str); NUM = _wtof(str.GetBuffer()); m_para[2] = NUM;
			GetDlgItemText(IDC_AXEDIT3, str); NUM = _wtof(str.GetBuffer()); m_para[3] = NUM;
			GetDlgItemText(IDC_AYEDIT3, str); NUM = _wtof(str.GetBuffer()); m_para[4] = NUM;
			GetDlgItemText(IDC_AZEDIT3, str); NUM = _wtof(str.GetBuffer()); m_para[5] = NUM;

			GetDlgItemText(IDC_SPEEDXEDIT, str); NUM = _wtof(str.GetBuffer()); m_para[6] = NUM;
			GetDlgItemText(IDC_SPEEDYEDIT, str); NUM = _wtof(str.GetBuffer()); m_para[7] = NUM;
			GetDlgItemText(IDC_SPEEDZEDIT, str); NUM = _wtof(str.GetBuffer()); m_para[8] = NUM;
			GetDlgItemText(IDC_SPEEDAXEDIT, str); NUM = _wtof(str.GetBuffer()); m_para[9] = NUM;
			GetDlgItemText(IDC_SPEEDAYEDIT, str); NUM = _wtof(str.GetBuffer()); m_para[10] = NUM;
			GetDlgItemText(IDC_SPEEDAZEDIT, str); NUM = _wtof(str.GetBuffer()); m_para[11] = NUM;

			GetDlgItemText(IDC_SPEEDXEDIT2, str); NUM = _wtof(str.GetBuffer()); m_para[12] = NUM;
			GetDlgItemText(IDC_SPEEDYEDIT2, str); NUM = _wtof(str.GetBuffer()); m_para[13] = NUM;
			GetDlgItemText(IDC_SPEEDZEDIT2, str); NUM = _wtof(str.GetBuffer()); m_para[14] = NUM;
			GetDlgItemText(IDC_SPEEDAXEDIT2, str); NUM = _wtof(str.GetBuffer()); m_para[15] = NUM;
			GetDlgItemText(IDC_SPEEDAYEDIT2, str); NUM = _wtof(str.GetBuffer()); m_para[16] = NUM;
			GetDlgItemText(IDC_SPEEDAZEDIT2, str); NUM = _wtof(str.GetBuffer()); m_para[17] = NUM;

			mode.flag_count = F_SinMotion;
			BufferStop = S_OFF;
			count0 = 0;
			sinmovetime.max = 1000;
		}
	}
}


void CCali::OnBnClickedSinmradio()
{
	this->GetDlgItem(IDC_SPEEDXEDIT2)->ShowWindow(TRUE);
	this->GetDlgItem(IDC_SPEEDYEDIT2)->ShowWindow(TRUE);
	this->GetDlgItem(IDC_SPEEDZEDIT2)->ShowWindow(TRUE);
	this->GetDlgItem(IDC_SPEEDAXEDIT2)->ShowWindow(TRUE);
	this->GetDlgItem(IDC_SPEEDAYEDIT2)->ShowWindow(TRUE);
	this->GetDlgItem(IDC_SPEEDAZEDIT2)->ShowWindow(TRUE);
	this->GetDlgItem(IDC_XIANGWEISTATIC)->ShowWindow(TRUE);
	this->GetDlgItem(IDC_BUFFERSTOPBUTTON)->ShowWindow(TRUE);
	SetDlgItemText(IDC_CTRLBUTTON2, L"开始");
	SetDlgItemText(IDC_FUZHISTATIC, L"幅值(°/mm)");
	SetDlgItemText(IDC_PINLVSTATIC, L"频率( Hz )");

	CString str; str.Format(L"%.1f",0.1);
	SetDlgItemText(IDC_SPEEDXEDIT, str); SetDlgItemText(IDC_SPEEDYEDIT, str); SetDlgItemText(IDC_SPEEDZEDIT, str);
	SetDlgItemText(IDC_SPEEDAXEDIT, str); SetDlgItemText(IDC_SPEEDAYEDIT, str); SetDlgItemText(IDC_SPEEDAZEDIT, str);
	str.Format(L"%d", 0);
	SetDlgItemText(IDC_SPEEDXEDIT2, str); SetDlgItemText(IDC_SPEEDYEDIT2, str); SetDlgItemText(IDC_SPEEDZEDIT2, str);
	SetDlgItemText(IDC_SPEEDAXEDIT2, str); SetDlgItemText(IDC_SPEEDAYEDIT2, str); SetDlgItemText(IDC_SPEEDAZEDIT2, str);
}


void CCali::OnBnClickedSmradio()
{
	// TODO: 在此添加控件通知处理程序代码
	this->GetDlgItem(IDC_SPEEDXEDIT2)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_SPEEDYEDIT2)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_SPEEDZEDIT2)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_SPEEDAXEDIT2)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_SPEEDAYEDIT2)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_SPEEDAZEDIT2)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_XIANGWEISTATIC)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_BUFFERSTOPBUTTON)->ShowWindow(FALSE);
	SetDlgItemText(IDC_CTRLBUTTON2, L"确认");
	SetDlgItemText(IDC_FUZHISTATIC, L"Planned");
	//SetDlgItemText(IDC_PINLVSTATIC, L"Speed(°,mm/s)");

	UpdateData(FALSE);
	//CString str; str.Format(L"%d", 1);
	//SetDlgItemText(IDC_SPEEDXEDIT, str); SetDlgItemText(IDC_SPEEDYEDIT, str); SetDlgItemText(IDC_SPEEDZEDIT, str);
	//SetDlgItemText(IDC_SPEEDAXEDIT, str); SetDlgItemText(IDC_SPEEDAYEDIT, str); SetDlgItemText(IDC_SPEEDAZEDIT, str);
}


void CCali::OnBnClickedBufferstopbutton()//缓冲停止
{
	// TODO: 在此添加控件通知处理程序代码
	BufferStop = S_ON;
	BufferStopCount = count0;
	sinmovetime.test = 10;
	//mode.flag_count = F_SinMove;
}


void CCali::OnBnClickedTtsradio()
{
	if (IDYES == MessageBox(_T("Are you sure want to switch coordinates? The device will move to the origin"), _T("提示"), MB_YESNO))
	{
	CString str;
	str.Format(L"%f", m_speed[0]); SetDlgItemText(IDC_SPEEDXEDIT, str);
	str.Format(L"%f", m_speed[1]); SetDlgItemText(IDC_SPEEDYEDIT, str);
	str.Format(L"%f", m_speed[2]); SetDlgItemText(IDC_SPEEDZEDIT, str);
	str.Format(L"%f", m_speed[3]); SetDlgItemText(IDC_SPEEDAXEDIT, str);
	str.Format(L"%f", m_speed[4]); SetDlgItemText(IDC_SPEEDAYEDIT, str);
	str.Format(L"%f", m_speed[5]); SetDlgItemText(IDC_SPEEDAZEDIT, str);
	UpdateData(TRUE);//控件值传给变量
	OnBnClickedSmradio();
	m_radiogroup3 = 0;
	for (size_t i = 0; i < 6; i++)
	{
		mid[i] = 0;
	}
	mid[2] = 270;
	UpdateData(FALSE);
	//this->GetDlgItem(IDC_SINMRADIO)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_AZ360EDIT3)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_RAZ360EDIT3)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_360STATIC)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_STATIC_360_UP)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_STATIC_360_DOWN)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_SPEEDAZ360EDIT)->ShowWindow(FALSE);

	//OnBnClickedCtrlbutton2();
	OnBnClickedButton9();
	//OnBnClickedButton1();
	}
}


void CCali::OnBnClickedFixradio()
{
	if (IDYES == MessageBox(_T("Are you sure want to switch coordinates? The device will move to the origin"), _T("提示"), MB_YESNO))
	{
		//this->GetDlgItem(IDC_motionstyleSTATIC)->ShowWindow(TRUE);
		//this->GetDlgItem(IDC_SMRADIO)->ShowWindow(TRUE);
		//this->GetDlgItem(IDC_SINMRADIO)->ShowWindow(TRUE);
		this->GetDlgItem(IDC_AZ360EDIT3)->ShowWindow(TRUE);
		this->GetDlgItem(IDC_RAZ360EDIT3)->ShowWindow(TRUE);
		this->GetDlgItem(IDC_360STATIC)->ShowWindow(TRUE);
		this->GetDlgItem(IDC_STATIC_360_UP)->ShowWindow(TRUE);
		this->GetDlgItem(IDC_STATIC_360_DOWN)->ShowWindow(TRUE);
		this->GetDlgItem(IDC_SPEEDAZ360EDIT)->ShowWindow(TRUE);
		UpdateData(TRUE);//控件值传给变量
		for (size_t i = 0; i < 6; i++)
		{
			mid[i] = 0;
		}
		UpdateData(FALSE);
		OnBnClickedButton9();
		//OnBnClickedButton1();
		//OnBnClickedCtrlbutton2();
	}
}


void CCali::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码

	QAIntPutDlg m_QAinput;
	if (m_radiogroup2==0)
	{
		//m_QAinput->GetDlgItem(IDC_QA360EDIT)->ShowWindow(FALSE);
	}
	if (m_QAinput.DoModal() == IDOK)
	{		
		if (firstlist )
		{
			firstlist = false;
			CRect rect;
			// 获取编程语言列表视图控件的位置和大小   
			m_commlist.GetClientRect(&rect);
			m_commlist.InsertColumn(0, _T("名字"), LVCFMT_CENTER, rect.Width() / 9*2);
			m_commlist.InsertColumn(1, _T("picth/°"), LVCFMT_CENTER, rect.Width() / 9);
			m_commlist.InsertColumn(2, _T("roll/°"), LVCFMT_CENTER, rect.Width() / 9);
			m_commlist.InsertColumn(3, _T("iso/°"), LVCFMT_CENTER, rect.Width() / 9);
			m_commlist.InsertColumn(4, _T("lat/mm"), LVCFMT_CENTER, rect.Width() / 9);
			m_commlist.InsertColumn(5, _T("long/mm"), LVCFMT_CENTER, rect.Width() / 9);
			m_commlist.InsertColumn(6, _T("vert/mm"), LVCFMT_CENTER, rect.Width() / 9);
			m_commlist.InsertColumn(7, _T("iso360"), LVCFMT_CENTER, rect.Width() / 9);

		}
		m_commattuide.attu0 =  _wtof(QAInPutData[1].GetBuffer());
		m_commattuide.attu1 = _wtof(QAInPutData[2].GetBuffer());
		m_commattuide.attu2 = _wtof(QAInPutData[3].GetBuffer());
		m_commattuide.attu3 = _wtof(QAInPutData[4].GetBuffer());
		m_commattuide.attu4 = _wtof(QAInPutData[5].GetBuffer());
		m_commattuide.attu5 = _wtof(QAInPutData[6].GetBuffer());
		m_commattuide.attu360 = _wtof(QAInPutData[7].GetBuffer());
		m_commattuide.QAName =QAInPutData[0];
		commctrl(m_commnum);
		m_commnum++;
		OnBnClickedSavebutton();
	}
	else
	{
		//CDialogEx::OnCancel();
		//return FALSE;
	}
}


void CCali::OnNMRClickCommlist(NMHDR *pNMHDR, LRESULT *pResult)//列表右键响应函数
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int index = pNMLV->iItem;
	if (index == -1)
		return;
	HMENU hMenu = ::CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, 10001, _T("编辑"));
	AppendMenu(hMenu, MF_STRING, 10002, _T("应用"));
	AppendMenu(hMenu, MF_STRING, 10003, _T("删除"));
	CPoint pt;
	GetCursorPos(&pt); //获得当前鼠标位置
	UINT Cmd = (UINT)::TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, m_hWnd, NULL);//弹出菜单

	POSITION pos = m_commlist.GetFirstSelectedItemPosition();
	int nItem = m_commlist.GetNextSelectedItem(pos);
	QAIntPutDlg m_QAinput;
	//CString str;
	switch (Cmd)//响应点击的菜单
	{
	case 10001:
		// TODO: 在此添加控件通知处理程序代码
		for (size_t i = 0; i < 8; i++)
		{
			QAInPutData[i] = m_commlist.GetItemText(nItem, i);
		}
		if (m_QAinput.DoModal() == IDOK)
		{
			//m_commnum++;
			m_commlist.DeleteItem(nItem);
			m_commnum--;
			m_commattuide.attu0 = _wtof(QAInPutData[1].GetBuffer());
			m_commattuide.attu1 = _wtof(QAInPutData[2].GetBuffer());
			m_commattuide.attu2 = _wtof(QAInPutData[3].GetBuffer());
			m_commattuide.attu3 = _wtof(QAInPutData[4].GetBuffer());
			m_commattuide.attu4 = _wtof(QAInPutData[5].GetBuffer());
			m_commattuide.attu5 = _wtof(QAInPutData[6].GetBuffer());
			m_commattuide.attu360 = _wtof(QAInPutData[7].GetBuffer());
			m_commattuide.QAName = QAInPutData[0];
			commctrl(nItem);
			m_commnum++;
			OnBnClickedSavebutton();
		}

		break;
	case 10002:
		if (m_radiogroup3==1)
		{
			AfxMessageBox(_T("应当在单步模式下进行QA应用"));
		}
		else if (m_radiogroup3 == 0)
		{
			for (size_t i = 0; i < 8; i++)
			{
				mid[i] = _wtof(m_commlist.GetItemText(nItem, i+1).GetBuffer());
			}
			UpdateData(FALSE);//第一个速度会变成0的BUG
		}
		break;
	case 10003:
		m_commlist.DeleteItem(nItem);
		m_commnum--;
		OnBnClickedSavebutton();
		break;
	}
	*pResult = 0;
}


void CCali::OnBnClickedButton9()//载入QA数据
{
		//TCHAR szFilter[] = _T("文本文件(*.csv)|*.csv|所有文件(*.*)|*.*||");  
		//CFileDialog fileDlg(TRUE, _T("csv"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilter, this);
		CString strFilePath;
		CFile file;
 
		//if (IDOK == fileDlg.DoModal())
		//{
			// 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里 
			//strFilePath = fileDlg.GetPathName();
			//SetDlgItemText(IDC_COMMFILENAMESTATIC, strFilePath);
		//}

		if (m_radiogroup2==1)
		{
			strFilePath = L"./QASetPoint.csv";
			if (!file.Open(strFilePath, CFile::modeRead))
			{
				MessageBox(L"can not open file!");
				return;
			}

			//for (int i = 7; i>-1; i--)
			//{
			//m_commlist.DeleteColumn(i);
			//}
			for (int i = m_commlist.GetItemCount(); i>-1; i--)
			{
				m_commlist.DeleteItem(i);
			}
			if (firstlist)
			{
				firstlist = false;
				CRect rect;
				// 获取编程语言列表视图控件的位置和大小   
				m_commlist.GetClientRect(&rect);
				m_commlist.InsertColumn(0, _T("Name"), LVCFMT_CENTER, rect.Width() / 9 * 2);
				m_commlist.InsertColumn(1, _T("picth/°"), LVCFMT_CENTER, rect.Width() / 9);
				m_commlist.InsertColumn(2, _T("roll/°"), LVCFMT_CENTER, rect.Width() / 9);
				m_commlist.InsertColumn(3, _T("iso/°"), LVCFMT_CENTER, rect.Width() / 9);
				m_commlist.InsertColumn(4, _T("lat/mm"), LVCFMT_CENTER, rect.Width() / 9);
				m_commlist.InsertColumn(5, _T("long/mm"), LVCFMT_CENTER, rect.Width() / 9);
				m_commlist.InsertColumn(6, _T("vert/mm"), LVCFMT_CENTER, rect.Width() / 9);
				m_commlist.InsertColumn(7, _T("iso360"), LVCFMT_CENTER, rect.Width() / 9);
			}
			CString result, str;
			result = "";
			char buffer[4096];
			int row = 0;
			int line = 0;
			file.Read(buffer, file.GetLength());
			m_commlist.InsertItem(row, result);

			for (int i = 0; i<(file.GetLength()); i++)
			{
				if (buffer[i] == 0x0A)//换行
				{
					m_commlist.SetItemText(row, line, result);
					row++;
					result = L"";
					line = -1;
				}
				else if (buffer[i] == 0x2C)//逗号
				{
					if (line == -1)
					{
						m_commlist.InsertItem(row, result);
						line = 0;
					}
					str.Format(_T("%d"), line);
					m_commlist.SetItemText(row, line, result);
					result = L"";
					line++;
				}
				else if (buffer[i] == 0x00)
				{

				}
				else
				{
					result += buffer[i];
				}
			}
			m_commlist.DeleteItem(0);//删除标题行
			m_commnum = m_commlist.GetItemCount();
		}
		else if (m_radiogroup2 == 0)
		{
			strFilePath = L"./QASetPoint2.csv";
			if (!file.Open(strFilePath, CFile::modeRead))
			{
				MessageBox(L"can not open file!");
				return;
			}

			//for (int i = 7; i>-1; i--)
			//{
			//m_commlist.DeleteColumn(i);
			//}
			for (int i = m_commlist.GetItemCount(); i>-1; i--)
			{
				m_commlist.DeleteItem(i);
			}
			CString result, str;
			result = "";
			char buffer[4096];
			int row = 0;
			int line = 0;
			file.Read(buffer, file.GetLength());
			m_commlist.InsertItem(row, result);

			for (int i = 0; i<(file.GetLength()); i++)
			{
				if (buffer[i] == 0x0A)//换行
				{
					m_commlist.SetItemText(row, line, result);
					row++;
					result = L"";
					line = -1;
				}
				else if (buffer[i] == 0x2C)//逗号
				{
					if (line == -1)
					{
						m_commlist.InsertItem(row, result);
						line = 0;
					}
					str.Format(_T("%d"), line);
					m_commlist.SetItemText(row, line, result);
					result = L"";
					line++;
				}
				else if (buffer[i] == 0x00)
				{

				}
				else
				{
					result += buffer[i];
				}
			}
			m_commlist.DeleteItem(0);//删除标题行
			m_commnum = m_commlist.GetItemCount();
		}
		

		
}

//箭头步进函数
void CCali::OnStnClickedStaticPitchUp()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RXEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.1f",NUM+ m_speed[0]); SetDlgItemText(IDC_XEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON4);
	((CStatic*)GetDlgItem(IDC_STATIC_PITCH_UP))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
	((CStatic*)GetDlgItem(IDC_STATIC_PITCH_UP))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticPitchDown()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RXEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.1f", NUM -m_speed[0]); SetDlgItemText(IDC_XEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
	((CStatic*)GetDlgItem(IDC_STATIC_PITCH_DOWN))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	((CStatic*)GetDlgItem(IDC_STATIC_PITCH_DOWN))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticRollUp()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RYEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.1f", NUM + m_speed[1]); SetDlgItemText(IDC_YEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON4);
	((CStatic*)GetDlgItem(IDC_STATIC_ROLL_UP))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
	((CStatic*)GetDlgItem(IDC_STATIC_ROLL_UP))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticRollDown()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RYEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.1f", NUM -m_speed[1]); SetDlgItemText(IDC_YEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
	((CStatic*)GetDlgItem(IDC_STATIC_ROLL_DOWN))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	((CStatic*)GetDlgItem(IDC_STATIC_ROLL_DOWN))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticIsoUp()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RZEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.1f", NUM + m_speed[2]); SetDlgItemText(IDC_ZEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON4);
	((CStatic*)GetDlgItem(IDC_STATIC_ISO_UP))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
	((CStatic*)GetDlgItem(IDC_STATIC_ISO_UP))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticIsoDown()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RZEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.1f", NUM - m_speed[2]); SetDlgItemText(IDC_ZEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
	((CStatic*)GetDlgItem(IDC_STATIC_ISO_DOWN))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	((CStatic*)GetDlgItem(IDC_STATIC_ISO_DOWN))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticLatUp()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RAXEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.3f", NUM + m_speed[3]); SetDlgItemText(IDC_AXEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON4);
	((CStatic*)GetDlgItem(IDC_STATIC_LAT_UP))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
	((CStatic*)GetDlgItem(IDC_STATIC_LAT_UP))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticLatDown()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RAXEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.3f", NUM - m_speed[3]); SetDlgItemText(IDC_AXEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
	((CStatic*)GetDlgItem(IDC_STATIC_LAT_DOWN))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	((CStatic*)GetDlgItem(IDC_STATIC_LAT_DOWN))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticLongUp()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RAYEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.3f", NUM + m_speed[4]); SetDlgItemText(IDC_AYEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON4);
	((CStatic*)GetDlgItem(IDC_STATIC_LONG_UP))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
	((CStatic*)GetDlgItem(IDC_STATIC_LONG_UP))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticLongDown()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RAYEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.3f", NUM - m_speed[4]); SetDlgItemText(IDC_AYEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
	((CStatic*)GetDlgItem(IDC_STATIC_LONG_DOWN))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	((CStatic*)GetDlgItem(IDC_STATIC_LONG_DOWN))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticVertUp()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RAZEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.3f", NUM + m_speed[5]); SetDlgItemText(IDC_AZEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON4);
	((CStatic*)GetDlgItem(IDC_STATIC_VERT_UP))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
	((CStatic*)GetDlgItem(IDC_STATIC_VERT_UP))->SetIcon(m_hIcon);
}


void CCali::OnStnClickedStaticVertDown()
{
	UpdateData(TRUE);
	CString str;
	double NUM;
	GetDlgItemText(IDC_RAZEDIT3, str);
	NUM = _wtof(str.GetBuffer());
	str.Format(L"%.3f", NUM -m_speed[5]); SetDlgItemText(IDC_AZEDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
	((CStatic*)GetDlgItem(IDC_STATIC_VERT_DOWN))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	((CStatic*)GetDlgItem(IDC_STATIC_VERT_DOWN))->SetIcon(m_hIcon);
}

void CCali::OnStnClickedStatic360Up()
{
	UpdateData(TRUE); 
	if (m_speed[6]<3)
	{
		m_speed[6] = 3;
	}
	CString str;
	double NUM;
	GetDlgItemText(IDC_RAZ360EDIT3, str);
	NUM = _wtof(str.GetBuffer());
	NUM = NUM + m_speed[6];
	if (NUM>180)
	{
		NUM = NUM - 360;
	}
	str.Format(L"%.1f", NUM); SetDlgItemText(IDC_AZ360EDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON4);
	((CStatic*)GetDlgItem(IDC_STATIC_360_UP))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
	((CStatic*)GetDlgItem(IDC_STATIC_360_UP))->SetIcon(m_hIcon);
}



void CCali::OnStnClickedStatic360Down()
{
	UpdateData(TRUE);
	if (m_speed[6]<3)
	{
		m_speed[6] = 3;
	}
	CString str;
	double NUM;
	GetDlgItemText(IDC_RAZ360EDIT3, str);
	NUM = _wtof(str.GetBuffer());

	NUM = NUM - m_speed[6];
	if (NUM<-180)
	{
		NUM = NUM + 360;
	}
	str.Format(L"%.1f", NUM); SetDlgItemText(IDC_AZ360EDIT3, str);
	OnBnClickedCtrlbutton2();

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
	((CStatic*)GetDlgItem(IDC_STATIC_360_DOWN))->SetIcon(m_hIcon);
	Sleep(200);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	((CStatic*)GetDlgItem(IDC_STATIC_360_DOWN))->SetIcon(m_hIcon);
}
void CCali::OnOK()
{
	//   CDialog::OnOK(); 
	OnBnClickedCtrlbutton2();
}


void CCali::OnBnClickedButton1()
{
	CString str;
	GetDlgItemText(IDC_RXEDIT3, str);SetDlgItemText(IDC_XEDIT3, str);
	GetDlgItemText(IDC_RYEDIT3, str); SetDlgItemText(IDC_YEDIT3, str);
	GetDlgItemText(IDC_RZEDIT3, str); SetDlgItemText(IDC_ZEDIT3, str);
	GetDlgItemText(IDC_RAXEDIT3, str); SetDlgItemText(IDC_AXEDIT3, str);
	GetDlgItemText(IDC_RAYEDIT3, str); SetDlgItemText(IDC_AYEDIT3, str);
	GetDlgItemText(IDC_RAZEDIT3, str); SetDlgItemText(IDC_AZEDIT3, str);
	GetDlgItemText(IDC_RAZ360EDIT3, str); SetDlgItemText(IDC_AZ360EDIT3, str);
}

void CCali::OnCancel()
{
}
