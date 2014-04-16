#ifndef CCROSSSPLITTERHANDLE_H
#define CCROSSSPLITTERHANDLE_H

#include <QFrame>

class CCrossSplitterHandle : public QFrame
{
	Q_OBJECT

public:
	CCrossSplitterHandle(int n, QWidget *parent);
	~CCrossSplitterHandle();
	bool isCursorInside(){return m_inside;}
	bool isHandleCatched(){return m_catched;}
	int number(){return num;}
signals:
	void cursorInside();
	void handleCatched();
	void handleMoved(int num, int dx, int dy);
protected:
	virtual void mouseMoveEvent(QMouseEvent * mev);
	virtual void mousePressEvent(QMouseEvent * mev);
	virtual void mouseReleaseEvent(QMouseEvent * mev);
	virtual void leaveEvent(QEvent * ev);
	virtual void enterEvent(QEvent * ev);
private:
	bool m_inside;
	bool m_catched;
	QPoint m_prevPos;
	int num;	
	
};

#endif // CCrossSplitterHandle_H
