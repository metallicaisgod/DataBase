#include "StdAfx.h"
#include "splitterhandle.h"

CSplitterHandle::CSplitterHandle(int n, Qt::Orientation orient, QWidget *parent)
	: QFrame(parent)
{
	num = n;
	m_orientation = orient;
	m_inside = false;
	m_catched = false;
	setMouseTracking(true);
}

CSplitterHandle::~CSplitterHandle()
{

}

void CSplitterHandle::mouseMoveEvent(QMouseEvent * mev)
{
	if(m_catched)
	{
		QPoint curPos = mev->pos();
		int def;
		if(m_orientation == Qt::Vertical)
		{
			def = curPos.x() - m_prevPos.x();
			emit handleMoved(num,m_orientation,def);
		}
		else
		{
			def = curPos.y() - m_prevPos.y();
			emit handleMoved(num,m_orientation,def);
		}		
	}
}
void CSplitterHandle::mousePressEvent(QMouseEvent * mev)
{
	if(m_inside)
	{
		m_catched = true;
		m_prevPos = mev->pos();
		emit handleCatched();
	}
}
void CSplitterHandle::mouseReleaseEvent(QMouseEvent * mev)
{
	if(m_inside)
		m_catched = false;
}
void CSplitterHandle::leaveEvent(QEvent * ev)
{
	m_inside = false;
}
void CSplitterHandle::enterEvent(QEvent * ev)
{
	m_inside = true;
	if(m_orientation == Qt::Vertical)
		setCursor(Qt::SplitHCursor);
	else
		setCursor(Qt::SplitVCursor);
	emit cursorInside();
}