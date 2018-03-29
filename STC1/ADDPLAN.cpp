// ADDPLAN.cpp : 实现文件
//

#include "stdafx.h"
#include "STC1.h"
#include "ADDPLAN.h"
#include "afxdialogex.h"

#include <websocket.h>
#include <stdio.h>

#pragma comment(lib,"libmysql.lib")//附加依赖项，也可以在工程属性中设置


// ADDPLAN 对话框

IMPLEMENT_DYNAMIC(ADDPLAN, CDialogEx)

ADDPLAN::ADDPLAN(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADDPLANDIALOG, pParent)
	, ID(_T(""))
	, PlanName(_T(""))
	, m_beamname(_T(""))
	, m_beamangle(_T(""))
{
	BeamNum = 0;
}

ADDPLAN::~ADDPLAN()
{
}

void ADDPLAN::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IDEDIT, ID);
	DDX_Text(pDX, IDC_PLANNAMEEDIT, PlanName);
	DDX_Control(pDX, IDC_COMMLIST, ADDBeamList);
	DDX_Text(pDX, IDC_BEAMNAMEEDIT, m_beamname);
	DDX_Text(pDX, IDC_BEAMANGLEEDIT, m_beamangle);
	DDX_Control(pDX, IDC_EDIT5, m_edit);
	DDX_Control(pDX, IDC_PLANCOMBO, m_plancombo);
}


BEGIN_MESSAGE_MAP(ADDPLAN, CDialogEx)
	ON_BN_CLICKED(IDOK, &ADDPLAN::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &ADDPLAN::OnBnClickedButton1)
	ON_STN_CLICKED(IDC_FINDIDSTATIC, &ADDPLAN::OnStnClickedFindidstatic)
	ON_EN_KILLFOCUS(IDC_IDEDIT, &ADDPLAN::OnEnKillfocusIdedit)
	ON_EN_KILLFOCUS(IDC_PLANNAMEEDIT, &ADDPLAN::OnEnKillfocusPlannameedit)
	ON_NOTIFY(NM_RCLICK, IDC_COMMLIST, &ADDPLAN::OnNMRClickCommlist)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_COMMLIST, &ADDPLAN::OnNMCustomdrawCommlist)
	ON_EN_KILLFOCUS(IDC_EDIT5, &ADDPLAN::OnEnKillfocusEdit5)
	ON_NOTIFY(NM_DBLCLK, IDC_COMMLIST, &ADDPLAN::OnNMDblclkCommlist)
	ON_EN_SETFOCUS(IDC_EDIT5, &ADDPLAN::OnEnSetfocusEdit5)
	ON_STN_CLICKED(IDC_FINDNAMESTATIC, &ADDPLAN::OnStnClickedFindnamestatic)
END_MESSAGE_MAP()


// ADDPLAN 消息处理程序


BOOL ADDPLAN::OnInitDialog()
{
	CDialogEx::OnInitDialog();

		CRect rect;
		// 获取编程语言列表视图控件的位置和大小   
		ADDBeamList.GetClientRect(&rect);
		ADDBeamList.InsertColumn(0, _T("Beam Name"), LVCFMT_CENTER, rect.Width() / 3 * 2);
		ADDBeamList.InsertColumn(1, _T("Beam Angle/°"), LVCFMT_CENTER, rect.Width() / 3);

		if (isedit)
		{
			SetWindowText(L"EDIT PLAN");
			this->GetDlgItem(IDC_PLANNAMEEDIT)->ShowWindow(FALSE);
			this->GetDlgItem(IDC_PLANCOMBO)->ShowWindow(TRUE);

			this->GetDlgItem(IDOK)->EnableWindow(FALSE);
		}
		//GetDlgItem(IDC_EDIT5)->ShowWindow(FALSE);

	return TRUE;
}

void ADDPLAN::OnBnClickedOk()
{

	if (!isedit)//表示新建一个计划
	{
		if (PatientExist && !PlanExist)
		{
			UpdateData(TRUE);
			mysql_init(&m_sqlCon);//初始化数据库对象
			if (!mysql_real_connect(&m_sqlCon, "localhost", "root", "123456", "chair001", 3306, NULL, 0))//localhost:服务器地址，可以直接填入IP;root:账号;123:密码;test:数据库名;3306:网络端口  
			{
				AfxMessageBox(_T("Database connection failed!"));
				return;
			}
			else//连接成功则继续访问数据库，之后的相关操作代码基本是放在这里面的
			{
			}

			//查看当前计划数
			str.Format(L"SELECT PlanNum FROM patient where id = \"%s\"", ID);
			const wchar_t* wstr = (LPCTSTR)str; //一定得是unicode，否则这句话会错的
			char c1[500] = { 0 };
			wcstombs(c1, wstr, wcslen(wstr));
			const char* cc1 = c1;
			mysql_query(&m_sqlCon, cc1);
			m_res = mysql_store_result(&m_sqlCon);
			m_row = mysql_fetch_row(m_res);
			str = m_row[0];
			int PlanNum = _wtof(str.GetBuffer());
			//给病人插入新计划
			str.Format(L"UPDATE patient SET plan%d =\"%s\" where id = \"%s\"", PlanNum + 1, PlanName, ID);
			wstr = (LPCTSTR)str;//一定得是unicode，否则这句话会错的
			for (int i = 0; i < 500; i++) { c1[i] = 0; }
			wcstombs(c1, wstr, wcslen(wstr));
			cc1 = c1;
			mysql_query(&m_sqlCon, cc1);
			str.Format(L"UPDATE patient SET PlanNum =\"%d\" where id = \"%s\"", PlanNum + 1, ID);
			wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
			for (int i = 0; i < 500; i++) { c1[i] = 0; }
			wcstombs(c1, wstr, wcslen(wstr));
			cc1 = c1;
			mysql_query(&m_sqlCon, cc1);

			//向计划库插入新计划
			str.Format(L"INSERT into plan (name) VALUES(\"%s%s\")", ID,PlanName);
			wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
			for (int i = 0; i < 500; i++) { c1[i] = 0; }
			wcstombs(c1, wstr, wcslen(wstr));
			cc1 = c1;
			mysql_query(&m_sqlCon, cc1);
			str.Format(L"INSERT into plan (name) VALUES(\"%s%sN\")", ID,PlanName);
			wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
			for (int i = 0; i < 500; i++) { c1[i] = 0; }
			wcstombs(c1, wstr, wcslen(wstr));
			cc1 = c1;
			mysql_query(&m_sqlCon, cc1);

			for (size_t i = 0; i < BeamNum; i++)
			{
				str.Format(L"UPDATE plan SET beam%d =\"%s\" where name = \"%s%s\"", i + 1, BeamAngle[i],ID, PlanName);
				wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
				for (int i = 0; i < 500; i++) { c1[i] = 0; }
				wcstombs(c1, wstr, wcslen(wstr));
				cc1 = c1;
				mysql_query(&m_sqlCon, cc1);

				str.Format(L"UPDATE plan SET beam%d =\"%s\" where name = \"%s%sN\"", i + 1, BeamName[i],ID, PlanName);
				wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
				for (int i = 0; i < 500; i++) { c1[i] = 0; }
				wcstombs(c1, wstr, wcslen(wstr));
				cc1 = c1;
				mysql_query(&m_sqlCon, cc1);
			}
			AfxMessageBox(_T("Add plan successful！"));
			mysql_close(&m_sqlCon);//关闭Mysql连接
			CDialogEx::OnOK();
		}
		else
		{
			return;
		}
	}
	else if(isedit&&PlanExist)//编辑已有的计划
	{
		m_plancombo.GetWindowText(PlanName);
		mysql_init(&m_sqlCon);//初始化数据库对象
		if (!mysql_real_connect(&m_sqlCon, "localhost", "root", "123456", "chair001", 3306, NULL, 0))//localhost:服务器地址，可以直接填入IP;root:账号;123:密码;test:数据库名;3306:网络端口  
		{
			AfxMessageBox(_T("Database connection failed!"));
			return;
		}
		else//连接成功则继续访问数据库，之后的相关操作代码基本是放在这里面的
		{
			
		}

		for (size_t i = 0; i < BeamNum; i++)
		{
			str.Format(L"UPDATE plan SET beam%d =\"%s\" where name = \"%s%s\"", i + 1, BeamAngle[i],ID, PlanName);
			const wchar_t* wstr = (LPCTSTR)str; //一定得是unicode，否则这句话会错的
			char c1[500] = { 0 };
			wcstombs(c1, wstr, wcslen(wstr));
			const char* cc1 = c1;
			mysql_query(&m_sqlCon, cc1);

			str.Format(L"UPDATE plan SET beam%d =\"%s\" where name = \"%s%sN\"", i + 1, BeamName[i], ID,PlanName);
			wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
			for (int i = 0; i < 500; i++) { c1[i] = 0; }
			wcstombs(c1, wstr, wcslen(wstr));
			cc1 = c1;
			mysql_query(&m_sqlCon, cc1);
		}
		AfxMessageBox(_T("edit plan successful！"));
		mysql_close(&m_sqlCon);//关闭Mysql连接
		CDialogEx::OnOK();
	}
}


void ADDPLAN::OnBnClickedButton1()//添加beam信息
{
	if (editing)
	{
		UpdateData(TRUE);
		BeamName[p_edit] = m_beamname;
		BeamAngle[p_edit] = m_beamangle;

		ADDBeamList.DeleteItem(p_edit);
		ADDBeamList.InsertItem(p_edit, BeamName[p_edit]);// m_commattuide.QAName);
		ADDBeamList.SetItemText(p_edit, 1, BeamAngle[p_edit]);
		editing = false;
		ADDBeamList.SetItemData(p_edit, COLOR_DEFAULT);
		SetDlgItemText(IDC_BUTTON1, L"ADD beam");
	}
	else
	{
		UpdateData(TRUE);

		BeamName[BeamNum] = m_beamname;
		BeamAngle[BeamNum] = m_beamangle;

		ADDBeamList.InsertItem(BeamNum, BeamName[BeamNum]);// m_commattuide.QAName);
		ADDBeamList.SetItemText(BeamNum, 1, BeamAngle[BeamNum]);

		BeamNum++;
	}
}


void ADDPLAN::OnStnClickedFindidstatic()
{

}


void ADDPLAN::OnEnKillfocusIdedit()
{
	if (!isedit)
	{
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
		GetDlgItemText(IDC_IDEDIT, ID);
		(mysql_query(&m_sqlCon, "SELECT id FROM patient"));//从users这个表格查询id
		m_res = mysql_store_result(&m_sqlCon);
		while (m_row = mysql_fetch_row(m_res))
		{
			str = m_row[0];
			if (ID == str)
			{
				PatientExist = true;
			}
		}
		if (!PatientExist)
		{
			mysql_close(&m_sqlCon);//关闭Mysql连接
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONcha);
			((CStatic*)GetDlgItem(IDC_FINDIDSTATIC))->SetIcon(m_hIcon);
		}
		else if (PatientExist)
		{
			mysql_close(&m_sqlCon);//关闭Mysql连接

			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON8);
			((CStatic*)GetDlgItem(IDC_FINDIDSTATIC))->SetIcon(m_hIcon);
		}
	}
	else if (isedit)
	{
		PlanExist = false;
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
		CString result;
		GetDlgItemText(IDC_IDEDIT, ID);
		(mysql_query(&m_sqlCon, "SELECT id FROM patient"));//从users这个表格查询id
		m_res = mysql_store_result(&m_sqlCon);
		while (m_row = mysql_fetch_row(m_res))
		{
			str = m_row[0];
			if (ID == str)
			{
				PatientExist = true;
			}
		}
		if (PatientExist)
		{
			//mysql_close(&m_sqlCon);//关闭Mysql连接
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON8);
			((CStatic*)GetDlgItem(IDC_FINDIDSTATIC))->SetIcon(m_hIcon);
		}
		else if (!PatientExist)
		{
			mysql_close(&m_sqlCon);//关闭Mysql连接

			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONcha);
			((CStatic*)GetDlgItem(IDC_FINDIDSTATIC))->SetIcon(m_hIcon);
		}
		str.Format(L"SELECT PatientName,birth,sex,plan1,plan2,plan3,plan4,plan5,plan6,plan7,plan8,plan9,plan10 FROM patient where id = \"%s\"",ID );

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

				m_plancombo.ResetContent();
				//m_report.PlanName = m_row[3];
				for (size_t i = 0; i < 10; i++)
				{
					result = m_row[3 + i];
					if (result != "")
					{
						m_plancombo.AddString(result);
					}
					else if (result == "")
					{
						break;
					}
				}
			}
		}
		mysql_close(&m_sqlCon);//关闭Mysql连接

		HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON9);
		((CStatic*)GetDlgItem(IDC_FINDNAMESTATIC))->SetIcon(m_hIcon);
	}
	
}


void ADDPLAN::OnEnKillfocusPlannameedit()
{
	if (!isedit)
	{
		PlanExist = false;
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
		GetDlgItemText(IDC_PLANNAMEEDIT, PlanName);
		CString PlanName2;
		PlanName2.Format(L"%s%s",ID,PlanName);
		(mysql_query(&m_sqlCon, "SELECT name FROM plan"));//从users这个表格查询id
		m_res = mysql_store_result(&m_sqlCon);
		while (m_row = mysql_fetch_row(m_res))
		{
			str = m_row[0];
			if (PlanName2 == str)
			{
				PlanExist = true;
			}
		}
		if (PlanExist|| PlanName =="")
		{
			mysql_close(&m_sqlCon);//关闭Mysql连接
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONcha);
			((CStatic*)GetDlgItem(IDC_FINDNAMESTATIC))->SetIcon(m_hIcon);
		}
		else if (!PlanExist)
		{
			mysql_close(&m_sqlCon);//关闭Mysql连接

			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON8);
			((CStatic*)GetDlgItem(IDC_FINDNAMESTATIC))->SetIcon(m_hIcon);
		}
	}
	else if (isedit)//编辑计划
	{
		PlanExist = false;
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
		GetDlgItemText(IDC_PLANNAMEEDIT, PlanName);
		(mysql_query(&m_sqlCon, "SELECT name FROM plan"));//从users这个表格查询id
		m_res = mysql_store_result(&m_sqlCon);
		while (m_row = mysql_fetch_row(m_res))
		{
			str = m_row[0];
			if (PlanName == str)
			{
				PlanExist = true;
			}
		}
		if (PlanExist)
		{
			//mysql_close(&m_sqlCon);//关闭Mysql连接
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON8);
			((CStatic*)GetDlgItem(IDC_FINDNAMESTATIC))->SetIcon(m_hIcon);
			this->GetDlgItem(IDOK)->EnableWindow(TRUE);
			ADDBeamList.DeleteAllItems();
		}
		else if (!PlanExist || PlanName == "")
		{
			mysql_close(&m_sqlCon);//关闭Mysql连接
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONcha);
			((CStatic*)GetDlgItem(IDC_FINDNAMESTATIC))->SetIcon(m_hIcon);
			this->GetDlgItem(IDOK)->EnableWindow(FALSE);
			ADDBeamList.DeleteAllItems();
			return;
			
		}

		UpdateData(TRUE);
		//查看当前计划数


		str.Format(L"SELECT beam1,beam2,beam3,beam4,beam5,beam6,beam7,beam8,beam9,beam10 FROM plan where name = \"%s\"", PlanName);
		const wchar_t* wstr = (LPCTSTR)str; //一定得是unicode，否则这句话会错的
		char c1[500] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		const char* cc1 = c1;
		mysql_query(&m_sqlCon, cc1);
		m_res = mysql_store_result(&m_sqlCon);
		m_row = mysql_fetch_row(m_res);

		for (size_t i = 0; i < 10; i++)
		{
			BeamAngle[i] = m_row[i];
		}

		str.Format(L"SELECT beam1,beam2,beam3,beam4,beam5,beam6,beam7,beam8,beam9,beam10 FROM plan where name = \"%sN\"", PlanName);
		wstr = (LPCTSTR)str; //一定得是unicode，否则这句话会错的
		for (int i = 0; i < 500; i++) { c1[i] = 0; }
		wcstombs(c1, wstr, wcslen(wstr));
		cc1 = c1;
		mysql_query(&m_sqlCon, cc1);
		m_res = mysql_store_result(&m_sqlCon);
		m_row = mysql_fetch_row(m_res);

		for (size_t i = 0; i < 10; i++)
		{
			BeamName[i] = m_row[i];
			if (BeamName[i]=="")
			{
				BeamNum = i ;
				break;
			}
		}

		for (size_t i = 0; i < BeamNum; i++)
		{
			ADDBeamList.InsertItem(i, BeamName[i]);// m_commattuide.QAName);
			ADDBeamList.SetItemText(i, 1, BeamAngle[i]);
		}
		mysql_close(&m_sqlCon);//关闭Mysql连接
	}
}

void ADDPLAN::OnNMRClickCommlist(NMHDR *pNMHDR, LRESULT *pResult)//列表右键响应函数
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int index = pNMLV->iItem;
	if (index == -1)
		return;
	HMENU hMenu = ::CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, 10001, _T("Edit"));
	AppendMenu(hMenu, MF_STRING, 10002, _T("Delete"));
	CPoint pt;
	GetCursorPos(&pt); //获得当前鼠标位置
	UINT Cmd = (UINT)::TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, m_hWnd, NULL);//弹出菜单

	POSITION pos = ADDBeamList.GetFirstSelectedItemPosition();
	int nItem = ADDBeamList.GetNextSelectedItem(pos);

	//CString str;
	switch (Cmd)//响应点击的菜单
	{
	case 10001:

		ADDBeamList.SetItemData(nItem, COLOR_BLUE);

		editing = true;
		p_edit = nItem;
		m_beamname = BeamName[nItem];
		m_beamangle = BeamAngle[nItem];
		UpdateData(FALSE);
		SetDlgItemText(IDC_BUTTON1, L"Apply Edit");
		break;
	case 10002:
		ADDBeamList.DeleteItem(nItem);
		BeamNum--;

		mysql_init(&m_sqlCon);//初始化数据库对象
		if (!mysql_real_connect(&m_sqlCon, "localhost", "root", "123456", "chair001", 3306, NULL, 0))//localhost:服务器地址，可以直接填入IP;root:账号;123:密码;test:数据库名;3306:网络端口  
		{
			AfxMessageBox(_T("Database connection failed!"));
			return;
		}
		str.Format(L"UPDATE plan SET beam%d =\"%s\" where name = \"%s%s\"", BeamNum+1, "", ID, PlanName);
		const wchar_t* wstr = (LPCTSTR)str; //一定得是unicode，否则这句话会错的
		char c1[500] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		const char* cc1 = c1;
		mysql_query(&m_sqlCon, cc1);

		str.Format(L"UPDATE plan SET beam%d =\"%s\" where name = \"%s%sN\"", BeamNum+1, "", ID, PlanName);
		wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
		for (int i = 0; i < 500; i++) { c1[i] = 0; }
		wcstombs(c1, wstr, wcslen(wstr));
		cc1 = c1;
		mysql_query(&m_sqlCon, cc1);
		break;	
	}
	*pResult = 0;
}

void ADDPLAN::OnNMCustomdrawCommlist(NMHDR *pNMHDR, LRESULT *pResult)
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
		else if (COLOR_RED == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(255, 0, 0);       //背景颜色  
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

void ADDPLAN::OnEnKillfocusEdit5()
{
	CString tem;
	m_edit.GetWindowText(tem);    //得到用户输入的新的内容 
	SetDlgItemText(IDC_EDIT5, L"123");
	ADDBeamList.SetItemText(m_Row, m_Col, tem);   //设置编辑框的新内容
	
	//m_edit.ShowWindow(FALSE);  //应藏编辑框
	
}

void ADDPLAN::OnNMDblclkCommlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//OnEnKillfocusEdit5();
	/*NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	m_Row = pNMListView->iItem;//获得选中的行  
	m_Col = pNMListView->iSubItem;//获得选中列
	SetDlgItemText(IDC_EDIT5, L"123");

	CRect rc;
		ADDBeamList.GetSubItemRect(m_Row, m_Col, LVIR_LABEL, rc);//获得子项的RECT；  
		m_edit.SetParent(&ADDBeamList);//转换坐标为列表框中的坐标  
		m_edit.MoveWindow(rc);//移动Edit到RECT坐在的位置; 
		//ssss = ADDBeamList.GetItemText(0, 1);
		//this->SetDlgItemTextW(IDC_EDIT5, ssss);
		//this->GetDlgItem(IDC_EDIT5)->SetDlgItemTextW();//将该子项中的值放在Edit控件中 
		//m_edit.ShowWindow(TRUE);
		//m_edit.SetFocus();//设置Edit焦点  
		//m_edit.ShowCaret();//显示光标  
		//m_edit.SetSel(-1);//将光标移动到最后 

	//}
	*pResult = 0;*/
}


void ADDPLAN::OnEnSetfocusEdit5()
{
	//str = ADDBeamList.GetItemText(m_Row, m_Col);
	//SetDlgItemText(IDC_EDIT5, L"");
}


void ADDPLAN::OnStnClickedFindnamestatic()
{
	// TODO: 在此添加控件通知处理程序代码
	if (isedit)
	{
		HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON8);
		((CStatic*)GetDlgItem(IDC_FINDNAMESTATIC))->SetIcon(m_hIcon);

		PlanExist = false;
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
		//GetDlgItemText(IDC_PLANNAMEEDIT, PlanName);

		m_plancombo.GetWindowText(PlanName);
		CString PlanName2;
		PlanName2.Format(L"%s%s", ID, PlanName);
		(mysql_query(&m_sqlCon, "SELECT name FROM plan"));//从users这个表格查询id
		m_res = mysql_store_result(&m_sqlCon);
		while (m_row = mysql_fetch_row(m_res))
		{
			str = m_row[0];
			if (PlanName2 == str)
			{
				PlanExist = true;
			}
		}
		if (PlanExist)
		{
			//mysql_close(&m_sqlCon);//关闭Mysql连接
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON8);
			((CStatic*)GetDlgItem(IDC_FINDNAMESTATIC))->SetIcon(m_hIcon);
			this->GetDlgItem(IDOK)->EnableWindow(TRUE);
			ADDBeamList.DeleteAllItems();
		}
		else if (!PlanExist || PlanName == "")
		{
			mysql_close(&m_sqlCon);//关闭Mysql连接
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICONcha);
			((CStatic*)GetDlgItem(IDC_FINDNAMESTATIC))->SetIcon(m_hIcon);
			this->GetDlgItem(IDOK)->EnableWindow(FALSE);
			ADDBeamList.DeleteAllItems();
			return;
		}

		//UpdateData(TRUE);
		//查看当前计划数


		str.Format(L"SELECT beam1,beam2,beam3,beam4,beam5,beam6,beam7,beam8,beam9,beam10 FROM plan where name = \"%s%s\"",ID, PlanName);
		const wchar_t* wstr = (LPCTSTR)str; //一定得是unicode，否则这句话会错的
		char c1[500] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		const char* cc1 = c1;
		mysql_query(&m_sqlCon, cc1);
		m_res = mysql_store_result(&m_sqlCon);
		m_row = mysql_fetch_row(m_res);

		for (size_t i = 0; i < 10; i++)
		{
			BeamAngle[i] = m_row[i];
		}

		str.Format(L"SELECT beam1,beam2,beam3,beam4,beam5,beam6,beam7,beam8,beam9,beam10 FROM plan where name = \"%s%sN\"", ID,PlanName);
		wstr = (LPCTSTR)str; //一定得是unicode，否则这句话会错的
		for (int i = 0; i < 500; i++) { c1[i] = 0; }
		wcstombs(c1, wstr, wcslen(wstr));
		cc1 = c1;
		mysql_query(&m_sqlCon, cc1);
		m_res = mysql_store_result(&m_sqlCon);
		m_row = mysql_fetch_row(m_res);

		for (size_t i = 0; i < 10; i++)
		{
			BeamName[i] = m_row[i];
			if (BeamName[i] == "")
			{
				BeamNum = i;
				break;
			}
		}

		for (size_t i = 0; i < BeamNum; i++)
		{
			ADDBeamList.InsertItem(i, BeamName[i]);// m_commattuide.QAName);
			ADDBeamList.SetItemText(i, 1, BeamAngle[i]);
		}
		mysql_close(&m_sqlCon);//关闭Mysql连接
	}
}
