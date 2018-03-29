#if !defined(AFX_CLIENTSOCK_H__A80C259D_F59B_4079_AC2F_5E18FA850C77__INCLUDED_)
#define AFX_CLIENTSOCK_H__A80C259D_F59B_4079_AC2F_5E18FA850C77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSock.h : header file
//

class CMainUI;

/////////////////////////////////////////////////////////////////////////////
// CClientSock command target

class CClientSock : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CClientSock();
	virtual ~CClientSock();
	void SetDlg(CMainUI* pDlg);

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSock)
	public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSock)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
private:
	CMainUI* m_pDlg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCK_H__A80C259D_F59B_4079_AC2F_5E18FA850C77__INCLUDED_)
