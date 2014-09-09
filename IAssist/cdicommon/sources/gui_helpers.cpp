#include "stdafx.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolBar>
namespace CDICommon
{
bool RemoveMenuItem(QMenu *in, QString &str)
{
    int cnt = in->actions().count();
    //int cnt = in->GetMenuItemCount();
	for(int i=cnt-1; i>=0; i--)
	{
		//MENUITEMINFO mi;
        QString strMenu;
        strMenu = in->actions().at(i)->text();
        if(0==str.compare(strMenu, Qt::CaseInsensitive))
		{
            in->removeAction(in->actions().at(i));
			return true;
		}
	}
	return false;
}

bool RemoveMenuItems(QMenu* inMenu, int *items, int count)
{
	// remove items
    QList <QAction*> removeList;
    for(int j=0;j < count; j++)
	{
        removeList << inMenu->actions().at(items[j]);
	}

    for(int j=0;j < count; j++)
    {
        inMenu->removeAction(removeList.at(j));
    }

	// remove leading separators
    //int id = inMenu->actions().at(0)->isSeparator();
    while(inMenu->actions().at(0)->isSeparator())
	{
        inMenu->removeAction(inMenu->actions().at(0));
        //id = inMenu->GetMenuItemID(0);
	}
	// remove trailing separators 
    int cnt = inMenu->actions().count();
    while(inMenu->actions().at(cnt-1)->isSeparator())
	{
        inMenu->removeAction(inMenu->actions().at(cnt-1));
        cnt = inMenu->actions().count();
        //id = inMenu->GetMenuItemID(0);
	}
	// remove doubled separators
	int n=0;
    bool id, id1;
	while(true)
	{
        cnt = inMenu->actions().count();
        id  = inMenu->actions().at(n)->isSeparator();
        id1 = inMenu->actions().at(n+1)->isSeparator();

        if(id && id1)
            inMenu->removeAction(inMenu->actions().at(n));
		n++;
        cnt = inMenu->actions().count();
		if(n >= cnt)
			break;
	}

	return true;
}

bool RemoveToolbarItems(QToolBar* in, int *items, int count)
{
	// number of buttons 
    //int btn_num = 0;
    //TBBUTTON button;


//	for(int i = 0; i < count; i++)
//	{
//		btn_num = 0;

//		// find item in toolbar by command ID
//		while(btn_num < in->actions().count())
//		{
//			in->GetToolBarCtrl().GetButton(btn_num,&button);
//			if( button.idCommand == items[i])
//				in->GetToolBarCtrl().DeleteButton(btn_num);
//			else
//				btn_num++;
//		}
//	}
    QList <QAction*> removeList;
    for(int j=0;j < count; j++)
    {
        removeList << in->actions().at(items[j]);
    }

    for(int j=0;j < count; j++)
    {
        in->removeAction(removeList.at(j));
    }
	return true;
}

//bool RemoveToolbarItems(CMFCToolBar* in, int *items, int count)
//{
//	// number of buttons
//	int btn_num = 0;

//	for(int i=0;i<count;i++)
//	{

//	//	in->EnableCustomizeButton(TRUE, 10011, "QWERTYASDF");
//		int	idx = in->CommandToIndex(items[i]);
//		if(idx !=-1)
//		{
//			//in->GetButton(idx)->EnableWindow();
//			in->RemoveButton(idx);
//		}
//		continue;

//		btn_num = 0;

//		// find item in toolbar by command ID
//		int btn_count = in->GetCount();
//		while(btn_num < btn_count)
//		{
//			//CMFCToolBarButton *pB = in->GetButtonInfo(btn_num);
//			//UINT btnID = pB->m_nID;
//			UINT nID,nStyle;
//			int iImg;
//			in->GetButtonInfo(btn_num, nID,nStyle,iImg);
//			if( nID == items[i])
//			{
//				in->RemoveButton(btn_num);
//				break;
//			}
//			else
//				btn_num++;
//		}
//	}
//	return true;
//};

} //end of namespace CDICommon
