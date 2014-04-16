#ifndef SPLITTERHANDLE_H
#define SPLITTERHANDLE_H

#include <QFrame>

class CSplitterHandle : public QFrame
{
	Q_OBJECT

public:
	CSplitterHandle(int n, Qt::Orientation orient, QWidget *parent);
	~CSplitterHandle();
	bool isCursorInside(){return m_inside;}
	bool isHandleCatched(){return m_catched;}
	int number(){return num;}
	Qt::Orientation orientation(){return m_orientation;}
signals:
	void cursorInside();
	void handleCatched();
	void handleMoved(int,Qt::Orientation,int);
protected:
	virtual void mouseMoveEvent(QMouseEvent * mev);
	virtual void mousePressEvent(QMouseEvent * mev);
	virtual void mouseReleaseEvent(QMouseEvent * mev);
	virtual void leaveEvent(QEvent * ev);
	virtual void enterEvent(QEvent * ev);
private:
	Qt::Orientation m_orientation;
	bool m_inside;
	bool m_catched;
	QPoint m_prevPos;
	int num;	
};

#endif // SPLITTERHANDLE_H
