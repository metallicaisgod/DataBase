/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    treemodel.cpp

    Provides a simple tree model to show how to create and use hierarchical
    models.
*/

#include <QtGui>

#include "treeitem.h"
#include "treemodel.h"

//! [0]
TreeModel::TreeModel(QList<db::DbProvider*> providers, ModelType type, db::DbImplant * implant, QObject *parent)
    : QAbstractItemModel(parent)
{
    m_providers = providers;
    m_type = type;
    m_implant_filter = implant;
    rootItem = new TreeItem();
    setupModelData(providers, rootItem);
}
//! [0]

//! [1]
TreeModel::~TreeModel()
{
    delete rootItem;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex &parent) const
{
//    if (parent.isValid())
//        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
//    else
//        return rootItem->columnCount();
    return 1;
}
//! [2]

//! [3]
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        //return QVariant();
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        switch(item->itemType())
        {
        case RootItem:
            {
                QString rootName;
                switch(m_type)
                {
                case Implants:
                    rootName = "Implants";
                    break;
                case Abutments:
                    rootName = "Abutments";
                    break;
                }
                return rootName;
            }
        case ProviderItem: //providers
            {
                return QString(m_providers[index.row()]->name);
            }
        case SeriesItem:  //series
            {
                db::DbSeries * series = reinterpret_cast<db::DbSeries *>(item->data());
               // QByteArray encodedString(series->name);
               // QTextCodec *codec = QTextCodec::codecForName("UTF-8");
                QString svalue(series->name);
                return svalue;
            }
        }
    }
    else if(role == DataRole)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        //QMetaType::VoidStar
        return QVariant::fromValue(item->data());
    }
    else if(role == ItemTypeRole)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        //QMetaType::VoidStar
        return item->itemType();
    }
    else if(role == Qt::CheckStateRole)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        return item->state(m_type);
    }
    else if (role == Qt::DecorationRole)
    {
        //return QVariant();
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        switch(item->itemType())
        {
        case RootItem:
            return QIcon(":/files/Resources/folder_icon.png");
        case ProviderItem: //providers
            return QIcon(":/files/Resources/provider_icon.png");
        case SeriesItem:
            return QIcon(":/files/Resources/series_icon.png");
        }
    }
    return QVariant();
}
//! [3]

//! [4]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    Qt::ItemFlags fl = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsTristate | Qt::ItemIsUserCheckable;
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if(item->itemType() != RootItem)
    {
        fl |= Qt::ItemIsEditable;
    }
    return fl;
}
//! [4]

//! [5]
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
//    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
//        return rootItem->data(section);

    return QVariant();
}
//! [5]

//! [6]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
//! [7]

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
//! [8]
QModelIndex TreeModel::rootIndex()
{
    return createIndex(0, 0, rootItem->child(0));
}

void TreeModel::setupModelData(QList<db::DbProvider*> providers, TreeItem *parent)
{
    //QList<TreeItem*> parents;
    //QList<int> indentations;
    //parents << parent;
    //indentations << 0;
    if(m_type == Abutments && !m_implant_filter)
        return;

    TreeItem * root = new TreeItem(parent, NULL, RootItem);
    parent->appendChild(root);
    int number = 0;

    while (number < providers.count())
    {

        QList<db::DbSeries*> list = QList<db::DbSeries*>::fromStdList(providers.at(number)->GetSeriesList());

        TreeItem * provider = new TreeItem(root, reinterpret_cast<void *>(providers[number]), ProviderItem);
        for(int i = 0; i < list.count(); i++)
        {
            if(m_type == Implants)
            {
                if(list[i]->GetImplants().empty() && !list[i]->GetAbutment().empty())
                    continue;
                provider->appendChild(new TreeItem(provider, list[i], SeriesItem));
            }
            if(m_type == Abutments)
            {
//                if(!list[i]->GetImplants().empty() && list[i]->GetAbutment().empty())
//                    continue;
                if(m_implant_filter->GetSeries() == list[i] /*&& !list[i]->GetAbutment().empty()*/)
                {
                    provider->appendChild(new TreeItem(provider, list[i], SeriesItem));
                    continue;
                }
                bool found = false;
                std::vector<db::CompatibleSeries>::iterator icms= m_implant_filter->GetSeries()->m_CompSer.begin();
                for(; icms!=m_implant_filter->GetSeries()->m_CompSer.end();icms++)
                {
                    if( (0==strncmp((*icms).ser, list[i]->name, NAME_SIZE)) &&
                        (0==strncmp((*icms).prov, list[i]->GetProvider().name, NAME_SIZE)))
                    {
                        provider->appendChild(new TreeItem(provider, list[i], SeriesItem));
                        found = true;
                        break;
                    }
                }
                if(!found)
                {
                    QVector<db::DbAbutment *> abutList = QVector<db::DbAbutment *>::fromStdVector(list[i]->GetAbutment());
                    if(abutList.isEmpty())
                        continue;
                    for(int j = 0; j < abutList.count(); j++)
                    {
                    // Check szCompatibility (ABComp) compatibility
                        if(0==strncmp(m_implant_filter->szCompatibility, abutList[j]->szCompatibility, _MAX_PATH))
                        {
                            provider->appendChild(new TreeItem(provider, list[i], SeriesItem));
                            break;
                        }
                    }
                }
            }
        }
        root->appendChild(provider);
        number++;
    }
}

void TreeModel::Update()
{
    beginResetModel ();
    endResetModel ();
    //currentIndex()
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;
    if(role == Qt::CheckStateRole)
    {
        TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
        item->setState(m_type, value.toBool());
        emit stateChanged(index);
        return false;
    }
    else if(role == Qt::EditRole)
    {
        if(value.toString().isEmpty())
            return true;
        TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
        if(item->itemType() == ProviderItem)
        {
            strncpy(m_providers[index.row()]->name, value.toString().toLocal8Bit().data(), NAME_SIZE);
        }
        else if(item->itemType() == SeriesItem)
        {
            db::DbSeries * series = reinterpret_cast<db::DbSeries *>(item->data());
            if(series)
            {
                strncpy(series->name, value.toString().toLocal8Bit().data(), NAME_SIZE);
            }
        }

        emit dataChanged(index, index);
        return false;
    }
    return QAbstractItemModel::setData(index, value, role);
}
