#pragma once
#include "afxwin.h"
#include "mysql.h"//这个没什么好说的，mysql头文件自然要包含
#include "afxcmn.h"


#define COLOR_DEFAULT 0 //默认颜色
#define COLOR_RED 1
  //红色
#define COLOR_BLUE 2
  //蓝色

// ADDPLAN 对话框

class ADDPLAN : public CDialogEx
{
	DECLARE_DYNAMIC(ADDPLAN)

public:
	ADDPLAN(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ADDPLAN();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDPLANDIALOG };
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
public:

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnStnClickedFindidstatic();

	BOOL PatientExist=false;
	BOOL PlanExist = false;
	CString ID;
	afx_msg void OnEnKillfocusIdedit();
	afx_msg void OnEnKillfocusPlannameedit();
	CString PlanName;
	CListCtrl ADDBeamList;

	CString BeamName[20];
	CString BeamAngle[20];
	int BeamNum;
	CString m_beamname;
	CString m_beamangle;

	
	CEdit m_edit;
	bool editing=false;
	bool isedit=false;//修改计划的标识符
	int p_edit;
	afx_msg void OnNMCustomdrawCommlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickCommlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEdit5();

	int m_Row=0; 
	int m_Col=0;
	afx_msg void OnNMDblclkCommlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnSetfocusEdit5();
	CComboBox m_plancombo;
	afx_msg void OnStnClickedFindnamestatic();
};
