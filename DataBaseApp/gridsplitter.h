#ifndef GRIDSPLITTER_H
#define GRIDSPLITTER_H

#include <QFrame>
#include "splitterhandle.h"
#include "crosssplitterhandle.h"


class CGridSplitter : public QFrame
{
	Q_OBJECT
	
public:
	CGridSplitter(QWidget *parent);
	~CGridSplitter();
	//count of widgetsList must even and equal rows * cols
	//return 0 - OK, -1 - wrong count, -2 - widgetsList is empty
	struct SplitterWidget
	{
		QPoint pos;
		QWidget * widget;
	};
	int setWidgets(QWidgetList widgetsList, int rows, int cols);
	void setHandleWidth(int width);
	int handleWidth(){return m_iHandleWidth;}
    QList<double> verticalRatio()
    {
        return m_verticalRatio;
    }
    QList<double> horizontalRatio()
    {
        return m_horizontalRatio;
    }
    void setVerticalRatio(QList<double> ratio)
    {
        m_verticalRatio = ratio;
    }
    void setHorizontalRatio(QList<double> ratio)
    {
        m_horizontalRatio = ratio;
    }

public slots:
	void snapToGrid();
	void moveSplitter(int, Qt::Orientation, int);
	void moveCrossSplitter(int,int,int);

protected:
	virtual void mouseMoveEvent(QMouseEvent * mev);
	virtual void mousePressEvent(QMouseEvent * mev);
	virtual void mouseReleaseEvent(QMouseEvent * mev);
	virtual void resizeEvent(QResizeEvent * ev);

private:
	QMap<int, SplitterWidget> m_WidgetsMap;
	int m_iHandleWidth;
	QList<QPoint> m_PointsList;
	QMap<int, CSplitterHandle*> m_HandlesMap;
	QMap<int, SplitterWidget> m_CrossHandleMap;
	int m_rows;
	int m_cols;
    QList<double> m_horizontalRatio;
    QList<double> m_verticalRatio;
};

#endif // GRIDSPLITTER_H
