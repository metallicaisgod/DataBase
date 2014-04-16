#include "stdafx.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"



using namespace db;

template <typename T>
struct impl_length_comp: public std::binary_function<T, T, bool>
{
    bool operator()(const T* l, const T* r)
    {
        return l->L1+l->L2 < r->L1+r->L2;
    }
};
template <typename T>
struct abut_length_comp: public std::binary_function<T, T, bool>
{
    bool operator()(const T* l, const T* r)
    {
        return l->L1 < r->L1;
    }
};

template <typename T>
struct compatible_abutment: public std::unary_function<T, bool>
{
    db::DbImplant* m_impl;
    compatible_abutment(db::DbImplant* im):m_impl(im){};
    bool operator()(const T ab)
    {
        // Check series
        if(m_impl->GetSeries() == ab->GetSeries())
            return true;

        // Check szCompatibility (ABComp) compatibility
        if(0==strncmp(m_impl->szCompatibility, ab->szCompatibility, _MAX_PATH))
            return true;

        // Check CompatibleSeries
        std::vector<CompatibleSeries>::iterator icms= m_impl->GetSeries()->m_CompSer.begin();
        for(; icms!=m_impl->GetSeries()->m_CompSer.end();icms++)
        {
            if( (0==strncmp((*icms).ser, ab->GetSeries()->name, NAME_SIZE)) &&
                (0==strncmp((*icms).prov, ab->GetSeries()->GetProvider().name, NAME_SIZE)))
                return true;
        }

        return false;
    }

};

typedef db::enumerator<db::t_AbutmentList::iterator, compatible_abutment<db::DbAbutment*> > t_abutmentEnumComp;
typedef t_abutmentEnumComp *t_abutmentEnumCompPtr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWidgetList list;
    list << ui->tVImplants << ui->tabVImplants << ui->tVAbutments << ui->tabVAbutments;
    ui->frame_2->setWidgets(list, 2, 2);
    m_pImplantsTreeModel = new QStandardItemModel();
    m_pAbutmentsTreeModel = new QStandardItemModel();
    fillModels("m:\\Downloads\\iaaaa\\IA_Database\\implants_db.xml");
//
//    ui->splitterLeft->handle(1)->installEventFilter(this);
//    ui->splitterRight->handle(1)->installEventFilter(this);
//    connect(ui->splitterLeft, SIGNAL(splitterMoved(int,int)), SLOT(moveSplitter(int,int)));
//    connect(ui->splitterRight, SIGNAL(splitterMoved(int,int)), SLOT(moveSplitter(int,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{

//        if(obj == ui->splitterLeft->handle(1))
//        {
//            if(ev->type() == QEvent::MouseButtonPress)
//            {
//                m_bLeftSplitterCatched = true;
//                m_bRightSplitterCatched = false;
//            }
//            else if(ev->type() == QEvent::MouseButtonRelease)
//            {
//                m_bLeftSplitterCatched = false;
//                m_bRightSplitterCatched = false;
//            }
//        }
//        else if(obj == ui->splitterRight->handle(1))
//        {
//            if(ev->type() == QEvent::MouseButtonPress)
//            {
//                m_bLeftSplitterCatched = false;
//                m_bRightSplitterCatched = true;
//            }
//            else if(ev->type() == QEvent::MouseButtonRelease)
//            {
//                m_bLeftSplitterCatched = false;
//                m_bRightSplitterCatched = false;
//            }
//        }
    return QMainWindow::eventFilter(obj, ev);
}


//void MainWindow::moveSplitter(int pos, int index)
//{
//    if(m_bLeftSplitterCatched)
//    {
//        ui->splitterRight->handle(index)->move(ui->splitterRight->handle(index)->x(), pos);
//        QRect rect = ui->tabVImplants->geometry();
//        rect.setHeight(pos);
//        ui->tabVImplants->setGeometry(rect);
//        rect = ui->tabVAbutments->geometry();
//        rect.setTop(ui->tabVImplants->height() + ui->splitterRight->handleWidth());
//        ui->tabVAbutments->setGeometry(rect);
//    }
//    else if(m_bRightSplitterCatched)
//    {
//        ui->splitterLeft->handle(index)->move(ui->splitterLeft->handle(index)->x(), pos);
//        QRect rect = ui->tVImplants->geometry();
//        rect.setHeight(pos);
//        ui->tVImplants->setGeometry(rect);
//        rect = ui->tVAbutments->geometry();
//        rect.setTop(ui->tVImplants->height() + ui->splitterLeft->handleWidth());
//        ui->tVAbutments->setGeometry(rect);
//    }
//}

void MainWindow::on_pB3DModel_clicked()
{
    if(ui->wOpenGL->isVisible())
    {
        ui->wOpenGL->setVisible(false);
        ui->pB3DModel->setText("3D Model >>");
    }
    else
    {
        ui->wOpenGL->setVisible(true);
        ui->pB3DModel->setText("3D Model <<");
    }
}

void MainWindow::fillModels(QString fileName)
{
      //iadb.LoadXml_All("c:\\Users\\Kola\\AppData\\Roaming\\CDI\\IA_Database\\dbfiles\\");
    iadb.LoadXml_All(fileName.toLocal8Bit().data());

    t_implantsEnumNFsp impl_enum = iadb.GetImplantsEnumerator(impl_length_comp<db::DbImplant>(), db::no_filter<db::DbImplant*>());
    DbImplant* pImplant = NULL;
    QStandardItem * parentItem = new QStandardItem("Implants");
    parentItem->setCheckable(true);
    parentItem->setCheckState(Qt::Checked);
    parentItem->setEditable(false);

    QStringList providerList;
    DbSeries * series;
    DbProvider provider;

    while (impl_enum->MoveNext())
    {
        pImplant = impl_enum->GetCurrent();        
            series = pImplant->GetSeries();
            provider = series->GetProvider();
            if(!providerList.contains(provider.name,Qt::CaseInsensitive))
            {
                QStandardItem * providerItem = new QStandardItem(provider.name);
                providerItem->setCheckable(true);
                providerItem->setCheckState(Qt::Checked);
                providerItem->setEditable(false);
                parentItem->setChild(providerList.count(), providerItem);
                providerList << provider.name;
                QList<DbSeries*> seriesList;
                seriesList = seriesList.fromStdList(provider.GetSeriesList());
                for(int i = 0; i < seriesList.count(); i ++)
                {
                    QStandardItem * serItem = new QStandardItem(seriesList[i]->name);
                    serItem->setCheckable(true);
                    serItem->setCheckState(Qt::Checked);
                    serItem->setEditable(true);
                    providerItem->setChild(i, serItem);
                }
            }
        printf("Implant: Name=%s, Le=%f, L1=%f, L2=%f\n", pImplant->name, pImplant->Le, pImplant->L1, pImplant->L2);
    }
    m_pImplantsTreeModel->appendRow(parentItem);
    delete impl_enum;
    ui->tVImplants->setModel(m_pImplantsTreeModel);
}
