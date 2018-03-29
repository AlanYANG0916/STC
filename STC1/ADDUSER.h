#pragma once
#include "winsock.h"//因为数据库是通过网络连接的，必须包含网络相关头文件
#include "mysql.h"//这个没什么好说的，mysql头文件自然要包含
#include <vector>
#include <string>

// ADDUSER 对话框

class ADDUSER : public CDialogEx
{
	DECLARE_DYNAMIC(ADDUSER)

public:
	ADDUSER(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ADDUSER();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDUSERDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnKillfocususernameedit();

	MYSQL m_sqlCon;
	MYSQL_RES* m_res;         //查询的返回结果集
	MYSQL_ROW m_row;          //获取每一条记录
	MYSQL_ROW m_row2;

	CString str;
	CString m_username;
	CString m_password;
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedCancel2();
};
