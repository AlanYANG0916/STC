#if !defined(AFX_COLORBUTTON_H__75B1ECC5_D7D3_4443_8D8D_B801A6F0B699__INCLUDED_)
#define AFX_COLORBUTTON_H__75B1ECC5_D7D3_4443_8D8D_B801A6F0B699__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorButton window

class CColorButton : public CButton
{
	// Construction
public:
	CColorButton();

	// Attributes
public:

	// Operations
public:
	void SetColor(COLORREF text, COLORREF bkgnd,
		COLORREF disabled = GetSysColor(COLOR_BTNFACE),
		COLORREF light = GetSysColor(COLOR_3DLIGHT),
		COLORREF highlight = GetSysColor(COLOR_BTNHIGHLIGHT),
		COLORREF shadow = GetSysColor(COLOR_BTNSHADOW),
		COLORREF darkShadow = GetSysColor(COLOR_3DDKSHADOW));
	void SetColorToDefault();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	void DrawText(CDC *pDC, CRect rc, CString strCaption, COLORREF clrText);
	void DrawRim(CDC *pDC, CRect rc, int nState);
	void DrawBkgnd(CDC *pDC, CRect rc, COLORREF color);
	void DrawButton(CDC *pDC, const CRect& rc, int nState);
	void DrawLine(CDC *pDC, const CPoint& start, const CPoint& end, COLORREF color);

	enum
	{
		BUTTON_CLICK = 0x01,  // ��ť���״̬
		BUTTON_NORMAL = 0x02,  // ��ť����״̬
		BUTTON_FOCUS = 0x04,  // ��ť����״̬ 
	};

	COLORREF m_clrText;  //���ֵ���ɫ
	COLORREF m_clrBkgnd; //������ɫ
	COLORREF m_clrDisabledBkgnd; //��Чʱ������ɫ
	COLORREF m_clrLight;  // �߿���������ɫ
	COLORREF m_clrHighlight; // �߿���������ɫ
	COLORREF m_clrShadow; // �߿򰵴�����ɫ
	COLORREF m_clrDarkShadow; // �߿򰵴�����ɫ

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBUTTON_H__75B1ECC5_D7D3_4443_8D8D_B801A6F0B699__INCLUDED_)
