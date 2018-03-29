// Login.cpp : 实现文件
//

#include "stdafx.h"
#include "STC1.h"
#include "Login.h"
#include "afxdialogex.h"

#include "afxdialogex.h"
#include "math.h"
#include "mmsystem.h"
#include<ATLBASE.H>
#include<iomanip>
#include <io.h>

//#include <SkinPPWTL.h>

//#pragma comment(lib,"SkinPPWTL.lib")
#pragma comment(lib,"libmysql.lib")//附加依赖项，也可以在工程属性中设置
// CLogin 对话框
CString UserName;
CString permission;

IMPLEMENT_DYNAMIC(CLogin, CDialogEx)

CLogin::CLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_password(_T(""))
{
	m_real_password = L"°";
	keyid = -1;
	NameExist = false;
	m_username= UserName;
	
}

CLogin::~CLogin()
{
}

void CLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_usernameEDIT, m_username);
	DDV_MaxChars(pDX, m_username, 32);
	DDX_Text(pDX, IDC_passwordEDIT, m_password);
	DDV_MaxChars(pDX, m_password, 32);
}


BEGIN_MESSAGE_MAP(CLogin, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &CLogin::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLogin::OnBnClickedCancel)
	ON_EN_SETFOCUS(IDC_passwordEDIT, &CLogin::OnEnSetfocuspasswordedit)
	ON_EN_SETFOCUS(IDC_usernameEDIT, &CLogin::OnEnSetfocususernameedit)
END_MESSAGE_MAP()


// CLogin 消息处理程序


BOOL CLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (showWaring)
	{
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
			_T("Arial"));                 // lpszFacename
		this->GetDlgItem(IDC_WaringSTATIC)->SetFont(&m_editFont);
		this->GetDlgItem(IDC_WaringSTATIC)->ShowWindow(TRUE);
	}
	if (moveingorlogin==1)
	{
		SetWindowText(L"Confirm Movement");
	}
	return true;
}

void CLogin::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (ChangePass)//是否是修改密码
	{
		CString NEWPass;
		GetDlgItemText(IDC_passwordEDIT,NEWPass);
		str.Format(L"UPDATE user2 SET password=AES_ENCRYPT(\"%s\", 'scuyf130916') where id = %s", NEWPass,m_username);
		const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
		char c1[500] = { 0 };
		wcstombs(c1, wstr, wcslen(wstr));
		const char* cc1 = c1;

		if (mysql_query(&m_sqlCon, cc1))
		{
			AfxMessageBox(TEXT("失败！"));
			return;
		}//从user2这个表格查询密码"SELECT password FROM user2 where id = 1000"
										//获取结果集
		m_res = mysql_store_result(&m_sqlCon);
		//m_row = mysql_fetch_row(m_res);

		mysql_close(&m_sqlCon);//关闭Mysql连接
		CDialogEx::OnOK();

	}
	else if(!ChangePass){
		if (m_password == m_real_password)
		{
			mysql_close(&m_sqlCon);//关闭Mysql连接
			UserName = m_username;
			CDialogEx::OnOK();
		}
		else
		{
			AfxMessageBox(_T("Password error, please check your username and password！"));
			SetDlgItemTextW(IDC_passwordEDIT, L"");
		}
	}
}


void CLogin::OnBnClickedCancel()
{
	if (CanClose)
	{
		CDialogEx::OnCancel();
	}
}

BOOL CLogin::SelectDB()
{
	//查询数据
	if (mysql_query(&m_sqlCon, "SELECT id FROM user2"))//从user2这个表格查询id
		return FALSE;
	//获取结果集
	m_res = mysql_store_result(&m_sqlCon);
	return TRUE;
}

void CLogin::GetDataFromDB()
{
	//获取记录
	int i = 0;
	int id = 0;
	while (m_row = mysql_fetch_row(m_res))
	{
		str = m_row[0];
		if (_wtof(m_username.GetBuffer()) == _wtof(str.GetBuffer()))
		{
			NameExist = true;
		}
		i++;
	}
	if (!NameExist)
	{
		//AfxMessageBox(_T("没有该用户的登记信息"));
		return;
	}
}


void CLogin::OnEnSetfocuspasswordedit()//获取真实的密码
{
	mysql_init(&m_sqlCon);//初始化数据库对象
	if (!mysql_real_connect(&m_sqlCon, "localhost", "root", "123456", "chair001", 3306, NULL, 0))//localhost:服务器地址，可以直接填入IP;root:账号;123:密码;test:数据库名;3306:网络端口  
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}
	else//连接成功则继续访问数据库，之后的相关操作代码基本是放在这里面的
	{
		//AfxMessageBox(_T("数据库连接成功!"));
	}

	UpdateData(TRUE);
	SelectDB();
	GetDataFromDB();

	//str.Format(L"SELECT password,permission FROM user2 where id = %s", m_username);
	str.Format(L"SELECT AES_DECRYPT(password, 'scuyf130916'),permission FROM user2 where id = %s", m_username);
	
	const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
	char c1[500] = { 0 };
	wcstombs(c1, wstr, wcslen(wstr));
	//最后进行转换
	const char* cc1 = c1;

	if (NameExist)
	{

		mysql_query(&m_sqlCon, cc1);//从user2这个表格查询密码"SELECT password FROM user2 where id = 1000"
										//获取结果集
			m_res = mysql_store_result(&m_sqlCon);
			while (m_row = mysql_fetch_row(m_res))
			{
				m_real_password = m_row[0];
				permission = m_row[1];
			}
		
	}

	/*str.Format(L"SELECT permission FROM user2 where id = %s", m_username);

	wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
	wcstombs(c1, wstr, wcslen(wstr));
	cc1 = c1;
	if (NameExist)
	{
		mysql_query(&m_sqlCon, cc1);//从user2这个表格查询密码"SELECT password FROM user2 where id = 1000"
										 //获取结果集
		m_res = mysql_store_result(&m_sqlCon);
		while (m_row2 = mysql_fetch_row(m_res))
		{
			permission = m_row2[0];
		}
	}*/
}

void CLogin::OnEnSetfocususernameedit()
{
	SetDlgItemTextW(IDC_passwordEDIT, L"");
}

HBRUSH CLogin::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  Change any attributes of the DC here   

		switch (pWnd->GetDlgCtrlID())
		{
		case IDC_WaringSTATIC:
			pDC->SetTextColor(RGB(255, 0, 0)); //设置文本颜色为
			pDC->SetBkColor(RGB(255, 255, 255));//设置文本背景颜色
			pDC->SetBkMode(TRANSPARENT);//OPAQUE
			//return (HBRUSH)::GetStockObject(RGB(255, 255, 255));//设置控件背景颜色
			break;
		default:
			break;
		}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
