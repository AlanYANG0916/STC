#pragma once
#include "afxcmn.h"

//float single_move_speed_show111[6];

// CCali 对话框

 struct commattuide
{
	float attu0;   //姿态一
	float attu1;   //姿态二
	float attu2;   //姿态三
	float attu3;   //姿态四
	float attu4;   //姿态五
	float attu5;   //姿态六
	float attu360;
	int staytime;
	CString QAName;
};

class CCali : public CDialogEx
{
	DECLARE_DYNAMIC(CCali)

public:
	CCali(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCali();

// 对话框数据
	enum { IDD = IDD_CALIDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_commlist;
	bool firstlist;
	int m_commnum;
	afx_msg void OnBnClickedNextbutton();
	void commctrl(int number);
	commattuide m_commattuide;
	afx_msg void OnBnClickedXradio();
	int m_radiogroup;
	
	int m_radiogroup3;//选择运动模式

	float m_singlestep;
	int m_staytime;
	afx_msg void OnLvnItemchangedCommlist(NMHDR *pNMHDR, LRESULT *pResult);
	int m_Row;
	int m_Col;
	afx_msg void OnHdnItemdblclickCommlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSavebutton();
	afx_msg void OnBnClickedCleancommbutton();

	afx_msg void OnBnClickedCtrlbutton2();
protected:
	double mid[7];
	double m_speed[7];
public:
	afx_msg void OnBnClickedSinmradio();
	afx_msg void OnBnClickedSmradio();
	afx_msg void OnBnClickedBufferstopbutton();
	afx_msg void OnBnClickedTtsradio();
	afx_msg void OnBnClickedFixradio();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnNMRClickCommlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton9();
	afx_msg void OnStnClickedStaticPitchUp();
	afx_msg void OnStnClickedStaticPitchDown();
	afx_msg void OnStnClickedStaticRollUp();
	afx_msg void OnStnClickedStaticRollDown();
	afx_msg void OnStnClickedStaticIsoUp();
	afx_msg void OnStnClickedStaticIsoDown();
	afx_msg void OnStnClickedStaticLatUp();
	afx_msg void OnStnClickedStaticLatDown();
	afx_msg void OnStnClickedStaticLongUp();
	afx_msg void OnStnClickedStaticLongDown();
	afx_msg void OnStnClickedStaticVertUp();
	afx_msg void OnStnClickedStaticVertDown();
	afx_msg void OnStnDblclickStaticPitchDown();
	void OnOK();
	void OnCancel();
	afx_msg void OnStnClickedStatic360Up();
	afx_msg void OnStnClickedStatic360Down();
	afx_msg void OnBnClickedButton1();
};
