#pragma once
namespace CDICommon
{
    bool RemoveMenuItem(QMenu *in, QString &str);
    bool RemoveMenuItems(QMenu *in, int *items, int count);
    bool RemoveToolbarItems(QToolBar *in, int *items, int count);
    //bool RemoveToolbarItems(CMFCToolBar *in, int *items, int count);

} //end of namespace CDICommon
