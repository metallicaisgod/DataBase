#include "StdAfx.h"
#include "crosssplitterhandle.h"

CCrossSplitterHandle::CCrossSplitterHandle(int n, QWidget *parent)
	: QFrame(parent)
{
	num = n;
	m_catched = false;
	m_inside = false;
	setMouseTracking(true);
}

CCrossSplitterHandle::~CCrossSplitterHandle()
{

}

void CCrossSplitterHandle::mouseMoveEvent(QMouseEvent * mev)
{
	if(m_catched)
	{
		QPoint curPos = mev->pos();
		int dx, dy;
		dx = curPos.x() - m_prevPos.x();
		dy = curPos.y() - m_prevPos.y();
		emit handleMoved(num, dx, dy);		
	}
}
void CCrossSplitterHandle::mousePressEvent(QMouseEvent * mev)
{
	if(m_inside)
	{
		m_catched = true;
		emit handleCatched();
		m_prevPos = mev->pos();
	}
}
void CCrossSplitterHandle::mouseReleaseEvent(QMouseEvent * mev)
{
	m_catched = false;
	
}
void CCrossSplitterHandle::leaveEvent(QEvent * ev)
{
	m_inside = false;
}
void CCrossSplitterHandle::enterEvent(QEvent * ev)
{
	m_inside = true;
	setCursor(Qt::SizeAllCursor);
	emit cursorInside();
}