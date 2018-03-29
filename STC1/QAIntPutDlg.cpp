// QAIntPutDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "STC1.h"
#include "QAIntPutDlg.h"
#include "afxdialogex.h"

CString QAInPutData[8];//全局变量
// QAIntPutDlg 对话框

IMPLEMENT_DYNAMIC(QAIntPutDlg, CDialogEx)

QAIntPutDlg::QAIntPutDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	
{

}

QAIntPutDlg::~QAIntPutDlg()
{
}

void QAIntPutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_QAPointNameEDIT, QAInPutData[0]);
	DDX_Text(pDX, IDC_QAXEDIT, QAInPutData[1]);
	DDX_Text(pDX, IDC_QAYEDIT, QAInPutData[2]);
	DDX_Text(pDX, IDC_QAZEDIT, QAInPutData[3]);
	DDX_Text(pDX, IDC_QAAXEDIT, QAInPutData[4]);
	DDX_Text(pDX, IDC_QAAYEDIT, QAInPutData[5]);
	DDX_Text(pDX, IDC_QAAZEDIT, QAInPutData[6]);
	DDX_Text(pDX, IDC_QA360EDIT, QAInPutData[7]);
}


BEGIN_MESSAGE_MAP(QAIntPutDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &QAIntPutDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &QAIntPutDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// QAIntPutDlg 消息处理程序


void QAIntPutDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	CDialogEx::OnOK();
}


void QAIntPutDlg::OnBnClickedCancel()
{
	if (IDYES == MessageBox(_T("Are you sure exit QA Point Editor without save？"), _T("WARNING!"), MB_YESNO))
	{
		CDialogEx::OnCancel();
	}
}
