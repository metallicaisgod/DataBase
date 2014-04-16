#include "StdAfx.h"
#include "gridsplitter.h"

CGridSplitter::CGridSplitter(QWidget *parent)
	: QFrame(parent)
{
	m_iHandleWidth = 5;
	m_rows = 0;
	m_cols = 0;
}

CGridSplitter::~CGridSplitter()
{
	if(!m_WidgetsMap.isEmpty())
	{
		QMapIterator<int, SplitterWidget> i(m_WidgetsMap);
		while(i.hasNext())
		{
			i.next();
			delete i.value().widget;
		}
		m_WidgetsMap.clear();
	}
	if(!m_PointsList.isEmpty())
	{
		m_PointsList.clear();
	}
	if(!m_HandlesMap.isEmpty())
	{
		QMapIterator<int, CSplitterHandle*> i(m_HandlesMap);
		while(i.hasNext())
		{
			i.next();
			delete i.value();
		}
		m_HandlesMap.clear();
	}
	if(!m_CrossHandleMap.isEmpty())
	{
		QMapIterator<int, SplitterWidget> i(m_CrossHandleMap);
		while(i.hasNext())
		{
			i.next();
			delete i.value().widget;
		}
		m_CrossHandleMap.clear();
	}
}

bool caseInsensitiveLessThan(const QPoint &point1, const QPoint &point2)
 {
     return point1.x() < point2.x() || (point1.x() == point2.x() && point1.y() < point2.y());
 }

int CGridSplitter::setWidgets(QWidgetList widgetsList, int rows, int cols)
{
	if(widgetsList.isEmpty())
		return -2;
	int count = widgetsList.count();
	if(count != rows * cols)
		return -1;
	if(!m_WidgetsMap.isEmpty())
	{
		QMapIterator<int, SplitterWidget> i(m_WidgetsMap);
		while(i.hasNext())
		{
			i.next();
			delete i.value().widget;
		}
		m_WidgetsMap.clear();
	}
	if(!m_PointsList.isEmpty())
	{
		m_PointsList.clear();
	}
	if(!m_HandlesMap.isEmpty())
	{
		QMapIterator<int, CSplitterHandle*> i(m_HandlesMap);
		while(i.hasNext())
		{
			i.next();
			delete i.value();
		}
		m_HandlesMap.clear();
	}
	if(!m_CrossHandleMap.isEmpty())
	{
		QMapIterator<int, SplitterWidget> i(m_CrossHandleMap);
		while(i.hasNext())
		{
			i.next();
			delete i.value().widget;
		}
		m_CrossHandleMap.clear();
	}
			
	//int maxColumn = 0;
	//int maxRows = 0;
	for(int i = 0; i < cols - 1; i++)
	{
	 	CSplitterHandle * handle = new CSplitterHandle(i, Qt::Vertical, this);
		m_HandlesMap.insertMulti(i, handle);
		connect(handle, SIGNAL(handleMoved(int,Qt::Orientation,int)), SLOT(moveSplitter(int,Qt::Orientation,int)));
	}
	for(int i = 0; i < rows - 1; i++)
	{
		CSplitterHandle * handle = new CSplitterHandle(i, Qt::Horizontal, this);
		m_HandlesMap.insertMulti(i, handle);
		connect(handle, SIGNAL(handleMoved(int,Qt::Orientation,int)), SLOT(moveSplitter(int,Qt::Orientation,int)));
	}
	SplitterWidget wid;
	for(int r = 0; r < rows - 1; r++)
	{
		for(int c = 0; c < cols - 1; c++)
		{
			wid.pos = QPoint(c,r);
			wid.widget = new CCrossSplitterHandle(r * cols + c, this);
			m_CrossHandleMap.insert(r * cols + c, wid);
			connect(wid.widget, SIGNAL(handleMoved(int,int, int)), SLOT(moveCrossSplitter(int,int,int)));
		}
	}

	if(m_WidgetsMap.isEmpty())
	{
		int c = 0;
		for(int i = 0; i < rows; i++)
		{
			for(int j = 0; j < cols; j++)
			{
				wid.pos = QPoint(j,i);
				wid.widget = widgetsList.at(i * cols + j);
				m_WidgetsMap.insert(c, wid);
				m_PointsList<<wid.pos;
				wid.widget->setParent(this);
				c++;
			}
		}
		m_rows += rows;
		m_cols += cols;
	}
	snapToGrid();
	return 0;
}
void CGridSplitter::snapToGrid()
{
	if(m_PointsList.isEmpty())
		return;
	QRect rect = geometry();
	QPoint last = m_PointsList.last();
	int maxX = last.x();
	int maxY = last.y();
	//int count = m_PointsList.count();
	int widgetWidth = (rect.width() - m_iHandleWidth * maxX) / (maxX + 1);
	int widgetHeight = (rect.height() - m_iHandleWidth * maxY) / (maxY + 1);
	QMapIterator<int, SplitterWidget> i(m_WidgetsMap);
	while(i.hasNext())
	{
		i.next();
		QWidget * widget = i.value().widget;
		widget->setGeometry((m_iHandleWidth + widgetWidth) * i.value().pos.x(), (m_iHandleWidth + widgetHeight) * i.value().pos.y(), widgetWidth, widgetHeight);
	}
	for(int iter = 0; iter < maxX; iter++)
	{
		QList<CSplitterHandle*> list = m_HandlesMap.values(iter);
		CSplitterHandle* pHand = list.at(1);
		if(pHand->orientation() == Qt::Vertical)
		{
			pHand->setGeometry(iter * (m_iHandleWidth + widgetWidth) + widgetWidth,0, m_iHandleWidth, rect.height());			
		}
	}
	for(int iter = 0; iter < maxY; iter++)
	{
		QList<CSplitterHandle*> list = m_HandlesMap.values(iter);
		CSplitterHandle* pHand = list.at(0);
		if(pHand->orientation() == Qt::Horizontal)
		{
			pHand->setGeometry(0, iter*(m_iHandleWidth + widgetHeight) + widgetHeight, rect.width(), m_iHandleWidth);			
		}
	}
	QMapIterator<int, SplitterWidget> iter(m_CrossHandleMap);
	while(iter.hasNext())
	{
		iter.next();
		QWidget * widget = iter.value().widget;
		widget->setGeometry((m_iHandleWidth + widgetWidth) * iter.value().pos.x() + widgetWidth, (m_iHandleWidth + widgetHeight) * iter.value().pos.y() + widgetHeight, m_iHandleWidth, m_iHandleWidth);
	}
}
void CGridSplitter::setHandleWidth(int width)
{
	m_iHandleWidth = width;
	snapToGrid();
}
void CGridSplitter::mousePressEvent(QMouseEvent * mev)
{
}
void CGridSplitter::mouseMoveEvent(QMouseEvent * mev)
{
}

void CGridSplitter::mouseReleaseEvent(QMouseEvent * mev)
{
}
void CGridSplitter::resizeEvent(QResizeEvent * ev)
{
	QFrame::resizeEvent(ev);
	snapToGrid();
}
void CGridSplitter::moveSplitter(int num, Qt::Orientation orient, int def)
{

	QList<CSplitterHandle*> list = m_HandlesMap.values(num);
	for(int i = 0; i < list.size(); i++)
	{
		CSplitterHandle* pHand = list.at(i);
		if(pHand->orientation() == orient)
		{
			if(orient == Qt::Vertical && pHand->x() + def < width() - m_iHandleWidth - 1 && pHand->x() + def > 1)
				pHand->move(pHand->x() + def, 0);			
			else if(orient == Qt::Horizontal && pHand->y() + def < height() - m_iHandleWidth - 1 && pHand->y() + def > 1)
				pHand->move(0, pHand->y() + def);
			else 
				return;
		}
	}
	QMapIterator<int, SplitterWidget> iter(m_CrossHandleMap);
	while(iter.hasNext())
	{
		iter.next();
		if(iter.value().pos.x() == num && orient == Qt::Vertical)
		{
			QWidget * handle = iter.value().widget;
			handle->move(handle->x() + def, handle->y());
		}
		else if(iter.value().pos.y() == num && orient == Qt::Horizontal)
		{
			QWidget * handle = iter.value().widget;
			handle->move(handle->x(), handle->y() + def);
		}
	}

	QMapIterator<int, SplitterWidget> i(m_WidgetsMap);
	while(i.hasNext())
	{
		i.next();
		if(orient == Qt::Vertical && i.value().pos.x() == num)
		{
			QWidget * widget = i.value().widget;

			widget->resize(widget->width() + def, widget->height());
		}
		else if(orient == Qt::Vertical && i.value().pos.x() == num + 1)
		{			
			QWidget * widget = i.value().widget;
			widget->setGeometry(widget->x()+ def, widget->y(), widget->width() - def, widget->height());
		}
		else if(orient == Qt::Horizontal && i.value().pos.y() == num)
		{
			QWidget * widget = i.value().widget;

			widget->resize(widget->width(), widget->height() + def);
		}
		else if(orient == Qt::Horizontal && i.value().pos.y() == num + 1)
		{			
			QWidget * widget = i.value().widget;
			widget->setGeometry(widget->x(), widget->y()+ def, widget->width(), widget->height() - def);
		}

		//	QWidget * widget = i.value().widget;
		//widget->setGeometry((m_iHandleWidth + widgetWidth) * i.value().pos.x(), (m_iHandleWidth + widgetHeight) * i.value().pos.y(), widgetWidth, widgetHeight);
	}
}

void CGridSplitter::moveCrossSplitter(int num, int dx, int dy)
{
	QMap<int, SplitterWidget>::iterator i = m_CrossHandleMap.find(num);
	if(i!=m_CrossHandleMap.end())
	{
		QPoint handlePos = i.value().pos;
		moveSplitter(handlePos.x(), Qt::Vertical, dx);
		moveSplitter(handlePos.y(), Qt::Horizontal, dy);
	}
}
