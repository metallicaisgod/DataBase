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

template <typename T>
struct provider_filter_abutments: public std::unary_function<T, bool>
{
    //db::DbImplant* m_impl;
    //provider_filter_implants(){};
    bool operator()(const T prov)
    {
        // Check series
        if(!prov->SeriesCount())
            return true;
        std::list<DbSeries*>::iterator iser = prov->GetSeriesList().begin();
        for(; iser != prov->GetSeriesList().end(); iser++)
        {
            if(!(*(iser))->GetAbutment().empty())
                return true;
        }
        return false;
    }

};

typedef db::enumerator<db::t_ProvidersList::iterator,db::no_filter<db::DbProvider*> > t_ProviderEnumNF;
typedef db::enumerator<db::t_ProvidersList::iterator,provider_filter_abutments<db::DbProvider*> > t_ProviderEnumFA;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pImpTreeModel(NULL),
    m_pImpTableModel(NULL),
    m_pAbutTreeModel(NULL),
    m_pAbutTableModel(NULL)
{
    ui->setupUi(this);
    QWidgetList list;
    list << ui->tVImplants << ui->tabVImplants << ui->tVAbutments << ui->tabVAbutments;
    ui->frame_2->setWidgets(list, 2, 2);
    //m_pImplantsTreeModel = new QStandardItemModel();
    //m_pAbutmentsTreeModel = new QStandardItemModel();

    ui->mainToolBar->addAction(ui->actionLoad);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionAdd_Provider);
    ui->mainToolBar->addAction(ui->actionAdd_Series);
    ui->mainToolBar->addAction(ui->actionAdd_Implant);
    ui->mainToolBar->addAction(ui->actionAdd_Abutment);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionEdit_Note);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionRemove_Note);


    fileName = "..\\DataBase\\implants_db.xml";    
    iadb.LoadXml_All(fileName.toLocal8Bit().data());
    fillModels();
    //on_pB3DModel_clicked();
//
//    ui->splitterLeft->handle(1)->installEventFilter(this);
//    ui->splitterRight->handle(1)->installEventFilter(this);
//    connect(ui->splitterLeft, SIGNAL(splitterMoved(int,int)), SLOT(moveSplitter(int,int)));
//    connect(ui->splitterRight, SIGNAL(splitterMoved(int,int)), SLOT(moveSplitter(int,int)));
}

MainWindow::~MainWindow()
{
    iadb.SaveXml_All(fileName.toLocal8Bit().data());
    delete ui;
}

QList<double> fromVariantToDouble(QList<QVariant> list)
{
    QList<double> double_list;
    foreach(const QVariant & item, list)
        double_list << item.toDouble();
    return double_list;
}

QList<QVariant> fromDoubleToVariant(QList<double> list)
{
    QList<QVariant> variant_list;
    foreach(const double & item, list)
        variant_list << item;
    return variant_list;
}

void MainWindow::showEvent(QShowEvent * ev)
{
    if(ui->wOpenGL->isVisible())
    {
        QRect rect = geometry();
        m_openGLwidth = ui->wOpenGL->width();
        rect.setWidth(rect.width() - ui->wOpenGL->width() - ui->splitter->handleWidth());
        ui->wOpenGL->setVisible(false);
        setGeometry(rect);
    }
    QSettings settings("AI","DataBase");
    //QVariant var();
    QList<QVariant> list = settings.value("LeftHorizontalRatio", QVariant::fromValue(fromDoubleToVariant(ui->frame_2->horizontalRatio()))).toList();
    ui->frame_2->setHorizontalRatio(fromVariantToDouble(list));
    list = settings.value("LeftVerticalRatio", QVariant::fromValue(fromDoubleToVariant(ui->frame_2->verticalRatio()))).toList();
    ui->frame_2->setVerticalRatio(fromVariantToDouble(list));
    //settings.setValue("LeftGeometry", ui->widget->geometry());
    setGeometry(settings.value("Geometry", geometry()).toRect());
    ev->accept();
}

void MainWindow::closeEvent(QCloseEvent * ev)
{
    if(ui->wOpenGL->isVisible())
    {
        QRect rect = geometry();
        m_openGLwidth = ui->wOpenGL->width();
        rect.setWidth(rect.width() - ui->wOpenGL->width() - ui->splitter->handleWidth());
        ui->wOpenGL->setVisible(false);
        setGeometry(rect);
    }
    QSettings settings("AI","DataBase");
    //QVariant var();
    settings.setValue("LeftHorizontalRatio", QVariant::fromValue(fromDoubleToVariant(ui->frame_2->horizontalRatio())));
    settings.setValue("LeftVerticalRatio", QVariant::fromValue(fromDoubleToVariant(ui->frame_2->verticalRatio())));
    //settings.setValue("LeftGeometry", ui->widget->geometry());
    settings.setValue("Geometry", geometry());
    ev->accept();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    return QMainWindow::eventFilter(obj, ev);
}

void MainWindow::on_pB3DModel_clicked()
{
    QRect rect = geometry();
    if(ui->wOpenGL->isVisible())
    {
        m_openGLwidth = ui->wOpenGL->width();
        rect.setWidth(rect.width() - ui->wOpenGL->width() - ui->splitter->handleWidth());
        ui->wOpenGL->setVisible(false);
        ui->pB3DModel->setText("3D Model >>");
    }
    else
    {
        rect.setWidth(rect.width() + m_openGLwidth + ui->splitter->handleWidth());
        ui->wOpenGL->setVisible(true);
        ui->pB3DModel->setText("3D Model <<");
    }
    setGeometry(rect);
}

void MainWindow::fillModels(/*QString fileName*/)
{
   //DbSeries * series;
    DbProvider * provider;
    QList<DbProvider *> providerList;

    t_ProviderEnumNF prov_enum = iadb.GetProvidersEnumerator(db::no_filter<db::DbProvider*>());

    while(prov_enum.MoveNext())
    {
        provider = prov_enum.GetCurrent();
        if(!providerList.contains(provider))
        {
            providerList << provider;
        }
    }

//    t_implantsEnumNFsp impl_enum = iadb.GetImplantsEnumerator(impl_length_comp<db::DbImplant>(), db::no_filter<db::DbImplant*>());
//    DbImplant* pImplant = NULL;
//    while (impl_enum->MoveNext())
//    {
//        pImplant = impl_enum->GetCurrent();
//        series = pImplant->GetSeries();
//        provider = &(series->GetProvider());
//        if(!providerList.contains(provider))
//        {
//            providerList << provider;
//        }
//    }
    if(m_pImpTreeModel) delete m_pImpTreeModel;
    m_pImpTreeModel = new TreeModel(providerList, Implants, this);
    //m_pImplantsTreeModel->appendRow(parentItem);
//    delete impl_enum;
    ui->tVImplants->setModel(m_pImpTreeModel);
    ui->tVImplants->expand(m_pImpTreeModel->rootIndex());
    if(m_pImpTableModel) delete m_pImpTableModel;
    m_pImpTableModel = new TableModel(Implants, this);
    ui->tabVImplants->setModel(m_pImpTableModel);
    ui->tabVImplants->horizontalHeader()->setVisible(false);
    ui->tabVImplants->resizeColumnsToContents();

    connect(m_pImpTableModel, SIGNAL(stateChanged()), this, SLOT(tableImplantsStateChanged()));
    connect(m_pImpTreeModel, SIGNAL(stateChanged(QModelIndex)), SLOT(treeImplantStateChanged(QModelIndex)));



    t_ProviderEnumFA prov_abut_enum = iadb.GetProvidersEnumerator(provider_filter_abutments<db::DbProvider*>());
    providerList.clear();
    while(prov_abut_enum.MoveNext())
    {
        provider = prov_abut_enum.GetCurrent();
        if(!providerList.contains(provider))
        {
            providerList << provider;
        }
    }
//    t_abutmentEnumNFsp abut_enum = iadb.GetAbutmentsEnumerator(abut_length_comp<db::DbAbutment>(), db::no_filter<db::DbAbutment*>());
//    db::DbAbutment* pAbut = NULL;
//    providerList.clear();
//    while (abut_enum->MoveNext())
//    {
//        pAbut = abut_enum->GetCurrent();
//        series = pAbut->GetSeries();
//        provider = &(series->GetProvider());
//        if(!providerList.contains(provider))
//        {
//            providerList << provider;
//        }
//        //printf("Abutment: Name=%s, Le=%f, L1=%f, D1=%f\n", pAbut->name, pAbut->L1, pAbut->D1);
//    }

//    delete abut_enum;
    if(m_pAbutTreeModel) delete m_pAbutTreeModel;
    m_pAbutTreeModel = new TreeModel(providerList, Abutments, this);
    ui->tVAbutments->setModel(m_pAbutTreeModel);
    ui->tVAbutments->expand(m_pAbutTreeModel->rootIndex());

    if(m_pAbutTableModel) delete m_pAbutTableModel;
    m_pAbutTableModel = new TableModel(Abutments, this);
    ui->tabVAbutments->setModel(m_pAbutTableModel);
    ui->tabVAbutments->horizontalHeader()->setVisible(false);
    ui->tabVAbutments->resizeColumnsToContents();

    connect(m_pAbutTableModel, SIGNAL(stateChanged()), SLOT(tableAbutmentsStateChanged()));
    connect(m_pAbutTreeModel, SIGNAL(stateChanged(QModelIndex)), SLOT(treeAbutmentsStateChanged(QModelIndex)));


}

void MainWindow::on_tVImplants_clicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pImpTreeModel->data(index, SeriesRole).value<void *>());
    //if(series)
    //{
    m_pImpTableModel->setSeries(series);
    ui->tabVImplants->horizontalHeader()->setVisible((bool)series);
    if(series && series->GetImplants().empty())
        ui->tabVImplants->horizontalHeader()->setVisible(false);
    ui->tabVImplants->resizeColumnsToContents();
    //}
}

void MainWindow::on_tVImplants_collapsed(const QModelIndex &index)
{
    if(ui->tabVImplants->model() && index.isValid())
    {
        if(m_pImpTreeModel->rowCount(index) > 0)
        {
            for(int i = 0; i < m_pImpTreeModel->rowCount(index); i++)
            {
                QModelIndex childIndex = m_pImpTreeModel->index(i, 0, index);
                db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pImpTreeModel->data(childIndex, SeriesRole).value<void *>());
                if(series == m_pImpTableModel->series())
                {
                    m_pImpTableModel->setSeries(NULL);
                    ui->tabVImplants->horizontalHeader()->setVisible(false);
                    //ui->tabVImplants->setCurrentIndex(index);
                    QItemSelectionModel * selectionModel = ui->tVImplants->selectionModel();
                    selectionModel->clearSelection();
                    selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Current);
                    return;
                }
            }
        }
        else
        {
            db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pImpTreeModel->data(index, SeriesRole).value<void *>());
            if(series == m_pImpTableModel->series())
            {
                m_pImpTableModel->setSeries(NULL);
                ui->tabVImplants->horizontalHeader()->setVisible(false);
            }
        }
    }
}

void MainWindow::tableImplantsStateChanged()
{
    QModelIndex idx = ui->tVImplants->currentIndex();
    m_pImpTreeModel->Update();
    ui->tVImplants->setCurrentIndex(idx);
}

void MainWindow::treeImplantStateChanged(QModelIndex index)
{
    if(!index.isValid())
        return;
    m_pImpTreeModel->Update();
    on_tVImplants_clicked(index);
    ui->tVImplants->setCurrentIndex(index);
    ui->tVImplants->expand(index);
}

void MainWindow::on_tVAbutments_clicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pAbutTreeModel->data(index, SeriesRole).value<void *>());
    //if(series)
    //{
    m_pAbutTableModel->setSeries(series);
    ui->tabVAbutments->horizontalHeader()->setVisible((bool)series);
    if(series && series->GetAbutment().empty())
        ui->tabVAbutments->horizontalHeader()->setVisible(false);
    ui->tabVAbutments->resizeColumnsToContents();
}

void MainWindow::on_tVAbutments_collapsed(const QModelIndex &index)
{
    if(ui->tabVAbutments->model() && index.isValid())
    {
        if(m_pAbutTreeModel->rowCount(index) > 0)
        {
            for(int i = 0; i < m_pAbutTreeModel->rowCount(index); i++)
            {
                QModelIndex childIndex = m_pAbutTreeModel->index(i, 0, index);
                db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pAbutTreeModel->data(childIndex, SeriesRole).value<void *>());
                if(series == m_pAbutTableModel->series())
                {
                    m_pAbutTableModel->setSeries(NULL);
                    ui->tabVAbutments->horizontalHeader()->setVisible(false);
                    //ui->tabVImplants->setCurrentIndex(index);
                    QItemSelectionModel * selectionModel = ui->tVAbutments->selectionModel();
                    selectionModel->clearSelection();
                    selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Current);
                    return;
                }
            }
        }
        else
        {
            db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pAbutTreeModel->data(index, SeriesRole).value<void *>());
            if(series == m_pAbutTableModel->series())
            {
                m_pAbutTableModel->setSeries(NULL);
                ui->tabVAbutments->horizontalHeader()->setVisible(false);
            }
        }
    }
}

void MainWindow::tableAbutmentsStateChanged()
{
    QModelIndex idx = ui->tVAbutments->currentIndex();
    m_pAbutTreeModel->Update();
    ui->tVAbutments->setCurrentIndex(idx);
}

void MainWindow::treeAbutmentsStateChanged(QModelIndex index)
{
    if(!index.isValid())
        return;
    m_pAbutTreeModel->Update();
    on_tVAbutments_clicked(index);
    ui->tVAbutments->setCurrentIndex(index);
    ui->tVAbutments->expand(index);
}



void MainWindow::on_actionAdd_Provider_triggered()
{
    iadb.AddProvider("\0");
    fillModels();
    QModelIndex index = m_pImpTreeModel->index(m_pImpTreeModel->rowCount(m_pImpTreeModel->rootIndex()) - 1, 0, m_pImpTreeModel->rootIndex());
    if(index.isValid())
    {
        ui->tVImplants->edit(index);
    }
}
