#pragma once
#include "winsock.h"//因为数据库是通过网络连接的，必须包含网络相关头文件
#include "mysql.h"//这个没什么好说的，mysql头文件自然要包含
#include <vector>
#include <string>


// CLogin 对话框

class CLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CLogin)

public:
	CLogin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLogin();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	MYSQL m_sqlCon;
	MYSQL_RES* m_res;         //查询的返回结果集
	MYSQL_ROW m_row;          //获取每一条记录
	MYSQL_ROW m_row2;

	afx_msg void OnBnClickedOk();
	CString str;
	CString m_username;
	CString m_password;
	CString m_real_password;
	bool NameExist;
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL SelectDB();
	afx_msg void GetDataFromDB();

	//vector <int> m_data[100];  //存放数据库记录，最大为100条
	afx_msg void OnEnSetfocuspasswordedit();
	int keyid;
	afx_msg void OnEnSetfocususernameedit();

	bool CanClose = true;
	bool showWaring= false;
	bool ChangePass=false;
	bool moveingorlogin;//0login,1move

	BOOL OnInitDialog();
	CFont m_editFont;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
