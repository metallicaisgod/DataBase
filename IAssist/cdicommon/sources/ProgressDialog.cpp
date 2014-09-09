// ProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ext_resources.h"
#include "cdi_str.h"
#include "ProgressDialog.h"

namespace CDICommon 
{
// CProgressDialog dialog
CProgressDialog* CProgressDialog::s_theDlg=NULL;

//IMPLEMENT_DYNAMIC(CProgressDialog, CDialog)
CProgressDialog::CProgressDialog(QWindow* pParent /*=NULL*/)
    : QWindow(pParent), m_num(0)// ,m_Thread(this)
{
	m_pPar = pParent;
	m_tick = 0;
} 

CProgressDialog::~CProgressDialog()
{
}

//void CProgressDialog::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//}

CProgressDialog* CProgressDialog::CreateTheDialog(QWindow* pParent, const char * inStr)
{
    //CWnd* pPar = AfxGetMainWnd();//(pParent==NULL) ? AfxGetMainWnd() : pParent;

    QWindow* pPar = qApp->focusWindow();
	
	s_theDlg = new CProgressDialog(pPar);

	//s_theDlg->m_pPar->SetTimer(4444,500,NULL);

    //s_theDlg->Create( pPar);
	if(inStr)	
        s_theDlg->setTitle(QString(inStr));

    s_theDlg->show();
	return s_theDlg;
}

void CProgressDialog::SetPercent(int inPercent)
{
	if(s_theDlg)
		s_theDlg->SetPercentT(inPercent);
}
void CProgressDialog::SetRange(int inMin, int inMax)
{
	if(s_theDlg)
		s_theDlg->SetRangeT(inMin, inMax);
}
void CProgressDialog::Knock()
{
	if(s_theDlg)
	{
		s_theDlg->m_num = (s_theDlg->m_num > 5)? 0 : s_theDlg->m_num;
        //s_theDlg->RedrawWindow();
		s_theDlg->m_num++;
	}
};
void CProgressDialog::CloseTheDialog()
{
	if(s_theDlg)
	{
		
		//s_theDlg->m_pPar->KillTimer(1234);
        s_theDlg->close();
    //	s_theDlg->DestroyWindow();
		delete s_theDlg;
		s_theDlg = NULL;
	}
}

//BEGIN_MESSAGE_MAP(CProgressDialog, CDialog)
//	ON_WM_PAINT()
//	//ON_WM_TIMER()
//END_MESSAGE_MAP()

//void CProgressDialog::OnTimer(UINT nIDEvent)
//{
//	if(1234==nIDEvent)
//	{
//		m_num = (m_num > 5)? 0 : m_num;
//		RedrawWindow();
//	}
//
//	CDialog::OnTimer(nIDEvent);
//}

const int KolesoW = 64;
const int KolesoH = 64;

void CProgressDialog::run()
{
	while (true)
	{
		Sleep(10);
		if(s_theDlg)
		{
			if(m_tick==100)
			{
				Knock();
				m_tick=0;
			}
			m_tick++;
		}
	}


};

void CProgressDialog::SetRangeT(int inMin, int inMax)
{
    //if(IDD==IDD_DLG_PROG1)
    //	return;
    //QProgressBar
    //CProgressCtrl *pProgress = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS);
    //pProgress->SetRange(inMin, inMax);
}

// CProgressDialog message handlers
void CProgressDialog::SetPercentT(int inPercent)
{
    //if(IDD==IDD_DLG_PROG1)
    //{
		//CProgressDialog::Knock();
        //return;
    //}
    //CProgressCtrl *pProgress = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS);
    //pProgress->SetPos(inPercent);
	m_num = (m_num > 5)? 0 : m_num;
    //pProgress->RedrawWindow();
	RECT rc = {10,10,10+KolesoW,10+KolesoH};
    //RedrawWindow();
}

int CProgressDialog::GetPercent()
{
//	if(IDD==IDD_DLG_PROG1)
//	{
//		return 0;
//	}
//	if(s_theDlg)
//	{
        //CProgressCtrl *pProgress = (CProgressCtrl *)(s_theDlg->GetDlgItem(IDC_PROGRESS));
        //return pProgress->GetPos();
//	}
//	else
    return 0;
}
//BOOL CProgressDialog::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//	//m_Thread.start();
//	//UINT_PTR st = SetTimer(1234,500, NULL);
//	return TRUE;
//}
//void CProgressDialog::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting

//	//SetTimer(1234,500, NULL);

//	//CBitmap theBmp;
//	//theBmp.LoadBitmap(IDB_KOLESO);

//	//CDC memdc;
//	//memdc.CreateCompatibleDC(&dc);

//	CBitmap *pOld = memdc.SelectObject(&theBmp);

//	CSize pos(1,1);

//	if(IDD == IDD_DLG_PROG1)
//	{
//		CRect rc;
//		m_pPar->GetClientRect(rc);
//		m_pPar->ClientToScreen(rc);
//		SetWindowPos(NULL, rc.right-KolesoW-3,rc.bottom-KolesoH-16, KolesoW, KolesoH, SWP_NOZORDER);
//		pos.SetSize(0,0);
//	}

//	dc.BitBlt( pos.cx, pos.cy, KolesoW, KolesoH, &memdc, m_num*KolesoW, 0, SRCCOPY);
//	m_num++;

//	if(pOld)
//		memdc.SelectObject(pOld);

//	memdc.DeleteDC();

//	// TODO: Add your message handler code here
//	// Do not call CDialog::OnPaint() for painting messages
//}
}; // end of namespace CDICommon
