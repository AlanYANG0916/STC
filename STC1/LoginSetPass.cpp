// LoginSetPass.cpp : 实现文件
//

#include "stdafx.h"
#include "STC1.h"
#include "LoginSetPass.h"
#include "afxdialogex.h"

#pragma comment(lib,"libmysql.lib")//附加依赖项，也可以在工程属性中设置

extern CString UserName;

// LoginSetPass 对话框

IMPLEMENT_DYNAMIC(LoginSetPass, CDialogEx)

LoginSetPass::LoginSetPass(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{
	m_username = UserName;
	NEWPass = "";
	NEWPass2 = "";
}

LoginSetPass::~LoginSetPass()
{
}

void LoginSetPass::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_usernameEDIT, m_username);
	DDV_MaxChars(pDX, m_username, 32);
	//DDX_Text(pDX, IDC_passwordEDIT, m_password);
	//DDV_MaxChars(pDX, m_password, 32);
}


BEGIN_MESSAGE_MAP(LoginSetPass, CDialogEx)
	ON_BN_CLICKED(IDOK, &LoginSetPass::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &LoginSetPass::OnBnClickedCancel)
END_MESSAGE_MAP()


// LoginSetPass 消息处理程序


void LoginSetPass::OnBnClickedOk()
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
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	GetDlgItemText(IDC_passwordEDIT, NEWPass);
	GetDlgItemText(IDC_passwordEDIT2, NEWPass2);
	if (NEWPass!= NEWPass2)
	{
		AfxMessageBox(_T("Please try again , the two passwords are not consistent.!"));
		return;
	}
	
	str.Format(L"UPDATE user2 SET password=AES_ENCRYPT(\"%s\", 'scuyf130916') where id = %s", NEWPass, m_username);
	const wchar_t* wstr = (LPCTSTR)str;         //一定得是unicode，否则这句话会错的
	char c1[500] = { 0 };
	wcstombs(c1, wstr, wcslen(wstr));
	const char* cc1 = c1;

	mysql_query(&m_sqlCon, cc1);

	mysql_close(&m_sqlCon);//关闭Mysql连接
	CDialogEx::OnOK();
}


void LoginSetPass::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
