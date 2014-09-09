
#pragma once
#include <QWindow>
#include "ext_resources.h"
#include "thread.h"

// CProgressDialog dialog
namespace CDICommon 
{
class CProgressDialog : public QWindow//, public Runnable
{
    //DECLARE_DYNAMIC(CProgressDialog)

public:
    CProgressDialog(QWindow* pParent = NULL);   // standard constructor
    virtual ~CProgressDialog();

// Dialog Data	
    //enum { IDD = IDD_DLG_PROG };
	
protected:
	virtual void run();
    //virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	int m_num;
	void SetPercentT(int inPercent);
	void SetRangeT(int inMin, int inMax);
	static CProgressDialog* s_theDlg;
public:
static CProgressDialog* CreateTheDialog(QWindow* pParent=NULL, const char * inStr=NULL);
static int GetPercent();
static void SetPercent(int inPercent);
static void SetRange(int inMin, int inMax);
static void CloseTheDialog();
static void Knock();
    //virtual BOOL OnInitDialog();

	
    QCursor     m_waitCursor;
    //CWaitCursor m_w;
    //DECLARE_MESSAGE_MAP()
public:
    //afx_msg void OnPaint();
    QWindow* m_pPar;
	//Thread m_Thread;
	int m_tick;
	//afx_msg void OnTimer(UINT nIDEvent);
	
};

}; //end of namespace CDICommon
