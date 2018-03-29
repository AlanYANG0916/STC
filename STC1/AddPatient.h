#pragma once
#include "afxwin.h"
#include "mysql.h"//这个没什么好说的，mysql头文件自然要包含

// AddPatient 对话框

class AddPatient : public CDialogEx
{
	DECLARE_DYNAMIC(AddPatient)

public:
	AddPatient(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~AddPatient();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AddPatientDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	BOOL OnInitDialog();
public:

	MYSQL m_sqlCon;
	MYSQL_RES* m_res;         //查询的返回结果集
	MYSQL_ROW m_row;          //获取每一条记录

	CString str;

	afx_msg void OnBnClickedOk();
	CComboBox patientgender;
	CString ID;
	CString Name;
	CString Birth;

	bool PatientExist=false;
	CString Gender;

	bool edit=false;
	bool hadedit = false;
	afx_msg void OnEnKillfocuspatientidedit();
	afx_msg void OnBnClickedsearchbutton();
};
