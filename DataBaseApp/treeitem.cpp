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
    treeitem.cpp

    A container for items of data supplied by the simple tree model.
*/

#include <QStringList>

#include "treeitem.h"

//! [0]
TreeItem::TreeItem(TreeItem *parent, db::DbSeries * data, bool root)
{
    parentItem = parent;
    itemData = data;
    m_root = root;
}
//! [0]

//! [1]
TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}
//! [1]

//! [2]
void TreeItem::appendChild(TreeItem *item)
{
    childItems.append(item);
}
//! [2]

//! [3]
TreeItem *TreeItem::child(int row)
{
    return childItems.value(row);
}
//! [3]

//! [4]
int TreeItem::childCount() const
{
    return childItems.count();
}
//! [4]

//! [5]
int TreeItem::columnCount() const
{
    return 1;
}
//! [5]

//! [6]
db::DbSeries * TreeItem::data() const
{
    return itemData;
}

//! [6]

//! [7]
TreeItem *TreeItem::parent()
{
    return parentItem;
}
//! [7]

//! [8]
int TreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
//! [8]
//! [9]
Qt::CheckState TreeItem::state(ModelType type)
{
    int count = childCount();
    if(count > 0)//providers
    {
        int i;
        int ucCount = 0;
        int pcCount = 0;
        for(i = 0; i < count; i++ )
        {
            switch(child(i)->state(type))
            {
            case Qt::Unchecked:
               ucCount++;
               break;
            case Qt::PartiallyChecked:
               pcCount ++;
               break;
            default:
                break;
            }
        }
        if(ucCount == count)
            return Qt::Unchecked;
        if(ucCount > 0 || pcCount > 0)
            return Qt::PartiallyChecked;
        return Qt::Checked;
    }
    else//series
    {
        if(type == Implants)
        {
           QVector<db::DbImplant *> implants = QVector<db::DbImplant *>::fromStdVector(data()->GetImplants());
           int i;
           int c = 0;
           for(i = 0; i < implants.count(); i ++)
           {
                if(implants.at(i)->state == db::ObjState::Nonactive)
                {
                    c++;
                }
           }
           if(c == implants.count())
               return Qt::Unchecked;
           if(c > 0)
               return Qt::PartiallyChecked;
           return Qt::Checked;
        }
        else
        {
            QVector<db::DbAbutment *> abutments = QVector<db::DbAbutment *>::fromStdVector(data()->GetAbutment());
            int i;
            int c = 0;
            for(i = 0; i < abutments.count(); i ++)
            {
                 if(abutments.at(i)->state == db::ObjState::Nonactive)
                 {
                     c++;
                 }
            }
            if(c == abutments.count())
                return Qt::Unchecked;
            if(c > 0)
                return Qt::PartiallyChecked;
            return Qt::Checked;
        }

    }
    return Qt::Checked;
}
//! [9]

void TreeItem::setState(ModelType type, bool state)
{
    int count = childCount();
    if(count > 0)//providers
    {
        int i;
        for(i = 0; i < count; i++ )
        {
            child(i)->setState(type, state);
        }
    }
    else//series
    {
        if(type == Implants)
        {
           QVector<db::DbImplant *> implants = QVector<db::DbImplant *>::fromStdVector(data()->GetImplants());
           int i;
           for(i = 0; i < implants.count(); i ++)
           {
               if(state)
                   implants[i]->state = db::ObjState::Active;
               else
                   implants[i]->state = db::ObjState::Nonactive;
           }
        }
        else
        {
            QVector<db::DbAbutment *> abutments = QVector<db::DbAbutment *>::fromStdVector(data()->GetAbutment());
            int i;
            for(i = 0; i < abutments.count(); i ++)
            {
                if(state)
                    abutments[i]->state = db::ObjState::Active;
                else
                    abutments[i]->state = db::ObjState::Nonactive;
            }
        }
    }
}
