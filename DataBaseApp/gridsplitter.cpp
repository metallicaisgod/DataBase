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
    m_horizontalRatio.clear();
    for(int i = 0; i < m_cols; i++)
    {
        m_horizontalRatio << 1./m_cols;
    }
    m_verticalRatio.clear();
    for(int i = 0; i < m_rows; i++)
    {
        m_verticalRatio << 1./m_rows;
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
    int widgetWidth = (rect.width() - m_iHandleWidth * maxX);
    int widgetHeight = (rect.height() - m_iHandleWidth * maxY);
    //QMapIterator<int, SplitterWidget> i(m_WidgetsMap);
    int prevWidth = 0;
    int prevHeight = 0;
    int row = 0;
    int col = 0;
    int prevX = 0;
    int prevY = 0;
    int width = 0;
    int height = 0;
    //while(i.hasNext())
    for(row = 0; row < m_rows; row ++)
	{
        for(col = 0; col < m_cols; col ++)
        {
            QWidget * widget = m_WidgetsMap.value(m_cols * row + col).widget;
            if(col == (m_cols - 1))
                width =  rect.width() -  prevX;
            else
                width = widgetWidth * m_horizontalRatio.at(col);
            if(row == (m_rows - 1))
                height = rect.height() - prevY;
            else
                height = widgetHeight * m_verticalRatio.at(row);
            widget->setGeometry(prevX, prevY, width, height);
            prevX += m_iHandleWidth + width;
        }
        //        prevWidth = 0;
       // prevHeight = height;
        prevX = 0;
        prevY += m_iHandleWidth + height;
	}
    prevWidth = 0;
    prevHeight = 0;
	for(int iter = 0; iter < maxX; iter++)
	{
		QList<CSplitterHandle*> list = m_HandlesMap.values(iter);
		CSplitterHandle* pHand = list.at(1);
        int width = widgetWidth * m_horizontalRatio.at(iter);
		if(pHand->orientation() == Qt::Vertical)
		{
            pHand->setGeometry(iter * (m_iHandleWidth + prevWidth) + width,0, m_iHandleWidth, rect.height());
		}
        prevWidth = width;
	}
	for(int iter = 0; iter < maxY; iter++)
	{
		QList<CSplitterHandle*> list = m_HandlesMap.values(iter);
		CSplitterHandle* pHand = list.at(0);
        int height = widgetHeight * m_verticalRatio.at(iter);
		if(pHand->orientation() == Qt::Horizontal)
		{
            pHand->setGeometry(0, iter*(m_iHandleWidth + prevHeight) + height, rect.width(), m_iHandleWidth);
		}
        prevHeight = height;
	}
    prevWidth = 0;
    prevHeight = 0;
	QMapIterator<int, SplitterWidget> iter(m_CrossHandleMap);
	while(iter.hasNext())
	{
		iter.next();
		QWidget * widget = iter.value().widget;
        int width = widgetWidth * m_horizontalRatio.at(iter.value().pos.x());
        int height = widgetHeight * m_verticalRatio.at(iter.value().pos.y());
        widget->setGeometry((m_iHandleWidth + width) * iter.value().pos.x() + width,
                            (m_iHandleWidth + height) * iter.value().pos.y() + height,
                            m_iHandleWidth, m_iHandleWidth);
//        prevWidth = width;
//        prevHeight = height;
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
    QRect rect = geometry();
    QPoint last = m_PointsList.last();
    int maxX = last.x();
    int maxY = last.y();
    //int count = m_PointsList.count();
    int widgetWidth = (rect.width() - m_iHandleWidth * maxX);
    int widgetHeight = (rect.height() - m_iHandleWidth * maxY);
    m_verticalRatio.clear();
    for(int row = 0; row < m_rows; row++)
    {
        QWidget * widget = m_WidgetsMap.value(m_cols * row).widget;
        m_verticalRatio << ((double)widget->height()) / ((double)widgetHeight);
    }
    m_horizontalRatio.clear();
    for(int col = 0; col < m_cols; col++)
    {
        QWidget * widget = m_WidgetsMap.value(col).widget;
        m_horizontalRatio << ((double)widget->width()) / ((double)widgetWidth);
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
