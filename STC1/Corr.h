#pragma once


// CCorr 对话框

class CCorr : public CDialogEx
{
	DECLARE_DYNAMIC(CCorr)

public:
	CCorr(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCorr();

// 对话框数据
	enum { IDD = IDD_CORRDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	afx_msg void ReadConfig();
	double FLimitInTTS[6];
	double ULimitInTTS[6];
	double StepOn[6];
	double RefPoint[3];
	double CORR[3];

public:
	afx_msg void OnBnClickedChangemotionrangebutton();
	void OnOK();

	int m_radiogroup;
};
