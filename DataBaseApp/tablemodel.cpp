#include "tablemodel.h"

#define MAX_IMP_COLUMN  7
#define MAX_ABUT_COLUMN 5

TableModel::TableModel(ModelType type, QObject *parent) :
    QAbstractTableModel(parent)
{
    m_type = type;
    setSeries(NULL);
    m_b = true;
}


Qt::ItemFlags TableModel::flags ( const QModelIndex & index ) const
{
    Qt::ItemFlags flag = Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    if(index.column() == 0)
    {
        flag |= Qt::ItemIsUserCheckable;
    }
    return flag;
}
int	TableModel::columnCount ( const QModelIndex & parent) const
{
    if(m_type == Abutments)
        return MAX_ABUT_COLUMN;
    return MAX_IMP_COLUMN;
}

int	TableModel::rowCount ( const QModelIndex & parent) const
{
    int ret = 0;
    if(set)
    {
        if(m_type == Abutments)
            ret = m_pSeries->GetAbutment().size();
        else
            ret = m_pSeries->GetImplants().size();
    }
    return ret;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
                     int role) const
{
    if (role != Qt::DisplayRole || !set)
         return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if(section >= MAX_IMP_COLUMN)
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
            if(m_type == Abutments)
                string = "Da";
            break;
        case 3:
            string = "D2";
            if(m_type == Abutments)
                string = "La";
            break;
        case 4:
            string = "L1";
            if(m_type == Abutments)
                string = "Alpha";
            break;
        case 5:
            string = "L2";
            if(m_type == Abutments)
                string = "?";
            break;
        case 6:
            string = "Le";
            if(m_type == Abutments)
                string = "?";
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
    if(m_type == Implants)
    {
        QVector<db::DbImplant *> implants = QVector<db::DbImplant *>::fromStdVector(m_pSeries->GetImplants());

        if (index.row() >= implants.count() || index.column() >= MAX_IMP_COLUMN)
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
                    return QString::fromLocal8Bit(implants[r]->name);
                case 1:
                    return QString::fromLocal8Bit(implants[r]->artikul);
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
                    if(implants[r]->state == db::ObjState::Active)
                        return Qt::Checked;
                    else
                        return Qt::Unchecked;
                }
                return QVariant();
            }        
        case Qt::DecorationRole:
            if(c == 0)
            {
                return QIcon(":/files/Resources/note_icon.png");
            }
            return QVariant();
        case DataRole:
            return QVariant::fromValue(reinterpret_cast<void*>(implants[r]));
        }
    }
    else if(m_type == Abutments)
    {
        QVector<db::DbAbutment *> abutments = QVector<db::DbAbutment *>::fromStdVector(m_pSeries->GetAbutment());

        if (index.row() >= abutments.count() || index.column() >= MAX_ABUT_COLUMN)
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
                    return abutments[r]->name;
                case 1:
                    return abutments[r]->artikul;
                case 2:
                    return abutments[r]->D1;
                case 3:
                    return abutments[r]->L1;
                case 4:
                    return abutments[r]->Alpha;
                default:
                    return "?";
                }
            }
        case Qt::CheckStateRole:
            {
                if(c == 0)
                {
                    if(abutments[r]->state == db::ObjState::Active)
                        return Qt::Checked;
                    else
                        return Qt::Unchecked;
                }
                return QVariant();
            }
        case Qt::DecorationRole:
            if(c == 0)
            {
                return QIcon(":/files/Resources/note_icon.png");
            }
            return QVariant();        
        case DataRole:
             return QVariant::fromValue(reinterpret_cast<void*>(abutments[r]));
        }
    }
    return QVariant();
}



bool TableModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/ )
{
    if (!index.isValid() || !set)
        return false;

    if(m_type == Implants)
    {
        QVector<db::DbImplant *> implants = QVector<db::DbImplant *>::fromStdVector(m_pSeries->GetImplants());
        if (index.row() >= implants.count() || index.column() >= MAX_IMP_COLUMN)
            return false;
        if(role == Qt::CheckStateRole)
        {
            if(value.toBool())
                implants[index.row()]->state = db::ObjState::Active;
            else
                implants[index.row()]->state = db::ObjState::Nonactive;
            emit stateChanged();
            return true;
        }
    }
    else if(m_type == Abutments)
    {
        QVector<db::DbAbutment *> abutments = QVector<db::DbAbutment *>::fromStdVector(m_pSeries->GetAbutment());
        if (index.row() >= abutments.count() || index.column() >= MAX_ABUT_COLUMN)
            return false;
        if(role == Qt::CheckStateRole)
        {
            if(value.toBool())
                abutments[index.row()]->state = db::ObjState::Active;
            else
                abutments[index.row()]->state = db::ObjState::Nonactive;
            emit stateChanged();
            return true;
        }
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
