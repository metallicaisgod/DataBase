#include "tablemodel.h"

#define MAX_COLUMN 7

TableModel::TableModel(ModelType type, QObject *parent) :
    QAbstractTableModel(parent)
{
    m_type = type;
    setSeries(NULL);
    m_b = true;
}


Qt::ItemFlags TableModel::flags ( const QModelIndex & index ) const
{
    Qt::ItemFlags flag = Qt::ItemIsEnabled | Qt::ItemIsEditable;
    if(index.column() == 0)
    {
        flag |= Qt::ItemIsUserCheckable;
    }
    return flag;
}
int	TableModel::columnCount ( const QModelIndex & parent) const
{
    return MAX_COLUMN;
}

int	TableModel::rowCount ( const QModelIndex & parent) const
{
    int ret = 0;
    if(set)
        ret = m_pSeries->GetImplants().size();
    return ret;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
                     int role) const
{
    if (role != Qt::DisplayRole || !set)
         return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if(section >= MAX_COLUMN)
            return QVariant();
        QString string;
        switch(section)
        {
        case 0:
            string = "Model";
            break;
        case 1:
            string = "Article";
            break;
        case 2:
            string = "D1";
            break;
        case 3:
            string = "D2";
            break;
        case 4:
            string = "L1";
            break;
        case 5:
            string = "L2";
            break;
        case 6:
            string = "Le";
            break;
        default:
            string = "?";
            break;
        }
        return string;
    }
    else
        return QVariant();//QString("%1").arg(section + 1);
}

//void TableModel::setDir(QDir dir)
//{
//	m_Dir = dir;
//	//QString path = m_Dir.dirName();
//	sort(cur_sort_col, cur_sort_order);//fiList =  m_Dir.entryInfoList(m_filter, QDir::Files | QDir::NoDotAndDotDot);
//	set = true;
//	reset();//emit dataChanged(index(0,0), index(fiList.count(), MAX_COLUMN));
//}

QVariant TableModel::data ( const QModelIndex & index, int role) const
{
    if (!index.isValid() || !set)
        return QVariant();

    QVector<db::DbImplant *> implants = QVector<db::DbImplant *>::fromStdVector(m_pSeries->GetImplants());

    if (index.row() >= implants.count() || index.column() >= MAX_COLUMN)
        return QVariant();
    int r = index.row();
    int c = index.column();
    switch(role)
    {
    case Qt::DisplayRole:
        {
            //QVariant ret;
            switch(c)
            {
            case 0:
                return implants[r]->name;
            case 1:
                return implants[r]->artikul;
            case 2:
                return implants[r]->D1;
            case 3:
                return implants[r]->D2;
            case 4:
                return implants[r]->L1;
            case 5:
                return implants[r]->L2;
            case 6:
                return implants[r]->Le;
            default:
                return "?";
            }
        }
    case Qt::CheckStateRole:
    {
        if(c == 0)
        {
            if(m_b)
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
        return QVariant();
    }
//	case Qt::ToolTipRole:
//		return fiList.at(index.row()).completeBaseName();
//	case Qt::DecorationRole:
//		{
//			if(index.column() == 0)
//			{
//				return m_IconMap[fiList[index.row()].suffix()];
//			}
//		}
//		break;
    }
    return QVariant();
}



bool TableModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/ )
{
    if(role == Qt::CheckStateRole)
    {
        m_b = value.toBool();
        //return false;
    }
    return QAbstractTableModel::setData(index, value, role);
}

//void TableModel::Update()
//{
//	if(set)
//	{
//		if(!fiList.isEmpty())
//		{
//			QFileInfoList list = m_Dir.entryInfoList(m_filter, QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
//			if(fiList.count() == list.count())
//				return;
//		}
//		sort(cur_sort_col, cur_sort_order);
//	}
//}
