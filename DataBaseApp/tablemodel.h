#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include "types.h"
#include "..\iadatabase\headers\IADataBase.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(ModelType type,QObject *parent = 0);

    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant	data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    int	rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    void setSeries(db::DbSeries * series)
    {
        m_pSeries = series;
        set = (bool)m_pSeries;
        reset();
    }
    db::DbSeries * series()
    {
        return m_pSeries;
    }

signals:
    void stateChanged();

//	void setDir(QDir dir);
//	void setDir(QString dir_path)
//	{
//		set = false;
//		QFileInfo fi(dir_path);
//		if(fi.isDir())
//		{
//			set = true;
//			QDir dir(dir_path);
//			setDir(dir);
//		}
//	}
//	void setNameFilters(QStringList filter)
//	{
//		m_filter.clear();
//		m_filter << filter;
//	}

    //virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);


    //QModelIndex	index(QString name);
    //void CreateIconMap();

public slots:
    void Update()
    {
        reset();
    }

private:
    ModelType m_type;
    db::DbSeries * m_pSeries;
    //QDir m_Dir;
    bool set;
    bool m_b;
    //QStringList m_filter;
    //QFileInfoList fiList;
    //int cur_sort_col;
    //Qt::SortOrder cur_sort_order;
    //QMap<QString, QIcon> m_IconMap;

};

#endif // TABLEMODEL_H
