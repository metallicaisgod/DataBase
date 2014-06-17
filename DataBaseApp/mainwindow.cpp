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

    m_pImplantDialog = new ImplantDialog(this);
    m_pImplantDialog->hide();
    m_pAbutmentDialog  = new AbutmentDialog(this);
    m_pAbutmentDialog->hide();

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

    QSettings settings("IA", "Database");
    fileName = settings.value("fileName", fileName).toString();

    if(fileName.isEmpty())
    {
        QSettings set(QSettings::NativeFormat, QSettings::SystemScope, "CDI Soft", "IA_DEMO");
        fileName = set.value("ImplantDB", "").toString();
    }
    if(fileName.isEmpty())
    {
        QMessageBox::critical(this, "Load database error!", "Load database error!");
        return;
    }

    //fileName = "..\\DataBase\\implants_db.xml";
    //iadb.LoadXml_All(fileName.toLocal8Bit().data());
    iadb.ImportFromCSV("d:\\test.csv_lst");
    fillModels(Implants);
    fillModels(Abutments);
}

MainWindow::~MainWindow()
{
    QSettings settings("IA", "Database");
    settings.setValue("fileName", fileName);
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
    QSettings settings("IA","DataBase");
    //QVariant var();
    QList<QVariant> list = settings.value("LeftHorizontalRatio", QVariant::fromValue(fromDoubleToVariant(ui->frame_2->horizontalRatio()))).toList();
    ui->frame_2->setHorizontalRatio(fromVariantToDouble(list));
    list = settings.value("LeftVerticalRatio", QVariant::fromValue(fromDoubleToVariant(ui->frame_2->verticalRatio()))).toList();
    ui->frame_2->setVerticalRatio(fromVariantToDouble(list));
    //settings.setValue("LeftGeometry", ui->widget->geometry());
    setGeometry(settings.value("Geometry", geometry()).toRect());

    ev->accept();
    ui->tVImplants->setFocus();
    if(m_pImpTreeModel)
        ui->tVImplants->setCurrentIndex(m_pImpTreeModel->rootIndex());
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
    QSettings settings("IA","DataBase");
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

void MainWindow::fillModels(ModelType modelType)
{
   //DbSeries * series;
    DbProvider * provider;
    QList<DbProvider *> providerList;

    if(modelType == Implants)
    {

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
        m_pImpTreeModel = new TreeModel(providerList, modelType, NULL,this);
        //m_pImplantsTreeModel->appendRow(parentItem);
    //    delete impl_enum;
        ui->tVImplants->setModel(m_pImpTreeModel);
        ui->tVImplants->expand(m_pImpTreeModel->rootIndex());
        if(m_pImpTableModel) delete m_pImpTableModel;
        m_pImpTableModel = new TableModel(modelType, this);
        ui->tabVImplants->setModel(m_pImpTableModel);
        ui->tabVImplants->horizontalHeader()->setVisible(false);
        ui->tabVImplants->resizeColumnsToContents();

        connect(m_pImpTableModel, SIGNAL(stateChanged()), this, SLOT(tableImplantsStateChanged()));
        connect(m_pImpTreeModel, SIGNAL(stateChanged(QModelIndex)), SLOT(treeImplantStateChanged(QModelIndex)));

    }
    else if(modelType == Abutments)
    {        
        QModelIndex index = ui->tabVImplants->currentIndex();
        if(!index.isValid())
            return;
        DbImplant * implant = reinterpret_cast<DbImplant *>(m_pImpTableModel->data(index, DataRole).value<void*>());
//        t_ProviderEnumFA prov_abut_enum = iadb.GetProvidersEnumerator(provider_filter_abutments<db::DbProvider*>());
//        //providerList.clear();
//        while(prov_abut_enum.MoveNext())
//        {
//            provider = prov_abut_enum.GetCurrent();
//            if(!providerList.contains(provider))
//            {
//                providerList << provider;
//            }
//        }
        t_abutmentEnumCompPtr abut_enum = iadb.GetAbutmentsEnumerator(abut_length_comp<db::DbAbutment>(), compatible_abutment<db::DbAbutment*>(implant));
        db::DbAbutment* pAbut = NULL;
        DbSeries * series = NULL;
        DbProvider * provider;
        providerList.clear();
        while (abut_enum->MoveNext())
        {
            pAbut = abut_enum->GetCurrent();
            series = pAbut->GetSeries();
            provider = &(series->GetProvider());
            if(!providerList.contains(provider))
            {
                providerList << provider;
            }
            //printf("Abutment: Name=%s, Le=%f, L1=%f, D1=%f\n", pAbut->name, pAbut->L1, pAbut->D1);
        }

        delete abut_enum;
        if(m_pAbutTreeModel) delete m_pAbutTreeModel;
        m_pAbutTreeModel = new TreeModel(providerList, modelType, implant,this);
//        m_pAbutTreeModel->setImplantForFilter(implant);
        ui->tVAbutments->setModel(m_pAbutTreeModel);
        ui->tVAbutments->expand(m_pAbutTreeModel->rootIndex());

        if(m_pAbutTableModel) delete m_pAbutTableModel;
        m_pAbutTableModel = new TableModel(modelType, this);
        ui->tabVAbutments->setModel(m_pAbutTableModel);
        ui->tabVAbutments->horizontalHeader()->setVisible(false);
        ui->tabVAbutments->resizeColumnsToContents();

        connect(m_pAbutTableModel, SIGNAL(stateChanged()), SLOT(tableAbutmentsStateChanged()));
        connect(m_pAbutTreeModel, SIGNAL(stateChanged(QModelIndex)), SLOT(treeAbutmentsStateChanged(QModelIndex)));
    }

    disableActions(RootItem, modelType);
}

void MainWindow::on_tVImplants_clicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    ItemType itemType = (ItemType)m_pImpTreeModel->data(index, ItemTypeRole).toInt();
    db::DbSeries * series = NULL;
    if(itemType == SeriesItem)
    {
        series = reinterpret_cast<db::DbSeries *>(m_pImpTreeModel->data(index, DataRole).value<void *>());
    }
    //if(series)
    //{
    m_pImpTableModel->setSeries(series);
    ui->tabVImplants->horizontalHeader()->setVisible((bool)series);
    if(series && series->GetImplants().empty())
        ui->tabVImplants->horizontalHeader()->setVisible(false);
    ui->tabVImplants->resizeColumnsToContents();
    ui->tabVImplants->setCurrentIndex(m_pImpTableModel->index(0,0));
    fillModels(Abutments);
    disableActions(itemType, Implants);
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
                ItemType itemType = (ItemType)m_pImpTreeModel->data(childIndex, ItemTypeRole).toInt();
                if(itemType == SeriesItem)
                {
                    db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pImpTreeModel->data(childIndex, DataRole).value<void *>());
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
        }
//        else
//        {
//            db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pImpTreeModel->data(index, SeriesRole).value<void *>());
//            if(series == m_pImpTableModel->series())
//            {
//                m_pImpTableModel->setSeries(NULL);
//                ui->tabVImplants->horizontalHeader()->setVisible(false);
//            }
//        }
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
    ItemType itemType = (ItemType)m_pAbutTreeModel->data(index, ItemTypeRole).toInt();
    db::DbSeries * series = NULL;
    if(itemType == SeriesItem)
    {
        series = reinterpret_cast<db::DbSeries *>(m_pAbutTreeModel->data(index, DataRole).value<void *>());
    }
    m_pAbutTableModel->setSeries(series);
    ui->tabVAbutments->horizontalHeader()->setVisible((bool)series);
    if(series && series->GetAbutment().empty())
        ui->tabVAbutments->horizontalHeader()->setVisible(false);
    ui->tabVAbutments->resizeColumnsToContents();
    disableActions(itemType, Abutments);
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
                ItemType itemType = (ItemType)m_pAbutTreeModel->data(childIndex, ItemTypeRole).toInt();
                if(itemType == SeriesItem)
                {
                    db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pAbutTreeModel->data(childIndex, DataRole).value<void *>());
                    if(series == m_pAbutTableModel->series())
                    {
                        m_pAbutTableModel->setSeries(NULL);
                        ui->tabVAbutments->horizontalHeader()->setVisible(false);
                        QItemSelectionModel * selectionModel = ui->tVAbutments->selectionModel();
                        selectionModel->clearSelection();
                        selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Current);
                        return;
                    }
                }
            }
        }
//        else
//        {
//            db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pAbutTreeModel->data(index, SeriesRole).value<void *>());
//            if(series == m_pAbutTableModel->series())
//            {
//                m_pAbutTableModel->setSeries(NULL);
//                ui->tabVAbutments->horizontalHeader()->setVisible(false);
//            }
//        }
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
    QWidget * fWidget = focusWidget();
    if(fWidget == ui->tabVAbutments || fWidget == ui->tVAbutments)
    {
        fillModels(Abutments);
        QModelIndex idx = m_pAbutTreeModel->index(m_pAbutTreeModel->rowCount(m_pAbutTreeModel->rootIndex()) - 1, 0, m_pAbutTreeModel->rootIndex());
        if(idx.isValid())
        {
            ui->tVAbutments->scrollTo(idx);
            ui->tVAbutments->edit(idx);
        }
    }
    else
    {
        fillModels(Implants);
        QModelIndex idx = m_pImpTreeModel->index(m_pImpTreeModel->rowCount(m_pImpTreeModel->rootIndex()) - 1, 0, m_pImpTreeModel->rootIndex());
        if(idx.isValid())
        {
            ui->tVImplants->scrollTo(idx);
            ui->tVImplants->edit(idx);
        }
    }
}

void MainWindow::disableActions(ItemType itemType, ModelType modelType)
{
    switch(itemType)
    {
    case RootItem:
        ui->actionAdd_Provider->setDisabled(false);
        ui->actionAdd_Series->setDisabled(true);
        ui->actionAdd_Abutment->setDisabled(true);
        ui->actionAdd_Implant->setDisabled(true);
        ui->actionEdit_Note->setDisabled(true);
        ui->actionRemove_Note->setDisabled(true);
        break;
    case ProviderItem:
        ui->actionAdd_Provider->setDisabled(false);
        ui->actionAdd_Series->setDisabled(false);
        ui->actionAdd_Abutment->setDisabled(true);
        ui->actionAdd_Implant->setDisabled(true);
        ui->actionEdit_Note->setDisabled(false);
        ui->actionRemove_Note->setDisabled(false);
        break;
    case SeriesItem:
        ui->actionAdd_Provider->setDisabled(false);
        ui->actionAdd_Series->setDisabled(true);
        ui->actionAdd_Abutment->setDisabled(true);
        ui->actionAdd_Implant->setDisabled(true);
        if(modelType == Implants)
            ui->actionAdd_Implant->setDisabled(false);
        if(modelType == Abutments)
            ui->actionAdd_Abutment->setDisabled(false);
        ui->actionEdit_Note->setDisabled(false);
        ui->actionRemove_Note->setDisabled(false);
        break;
    }
}

void MainWindow::on_tabVImplants_clicked(const QModelIndex &idx)
{
    disableActions(SeriesItem, Implants);
}

void MainWindow::on_tabVAbutments_clicked(const QModelIndex &idx)
{
    disableActions(SeriesItem, Abutments);
}

void MainWindow::on_actionAdd_Series_triggered()
{
    QWidget * fWidget = focusWidget();
    if(fWidget == ui->tabVAbutments || fWidget == ui->tVAbutments)
    {
        QModelIndex provIdx = ui->tVAbutments->currentIndex();
        if(!provIdx.isValid())
            return;
        int row = provIdx.row();
        DbProvider * provider = reinterpret_cast<DbProvider *>(m_pAbutTreeModel->data(provIdx, DataRole).value<void *>());
        if(!provider)
            return;
        provider->AddSeries("\0");
        fillModels(Abutments);
        provIdx = m_pAbutTreeModel->index(row, 0, m_pAbutTreeModel->rootIndex());
        QModelIndex idx = m_pAbutTreeModel->index(m_pAbutTreeModel->rowCount(provIdx) - 1, 0, provIdx);
        if(idx.isValid())
        {
            ui->tVAbutments->expand(provIdx);
            ui->tVAbutments->scrollTo(idx);
            ui->tVAbutments->edit(idx);
        }
    }
    else
    {
        QModelIndex provIdx = ui->tVImplants->currentIndex();
        if(!provIdx.isValid())
            return;
        int row = provIdx.row();
        DbProvider * provider = reinterpret_cast<DbProvider *>(m_pImpTreeModel->data(provIdx, DataRole).value<void *>());
         if(!provider)
            return;
        provider->AddSeries("\0");
        fillModels(Implants);
        provIdx = m_pImpTreeModel->index(row, 0, m_pImpTreeModel->rootIndex());
        QModelIndex idx = m_pImpTreeModel->index(m_pImpTreeModel->rowCount(provIdx) - 1, 0, provIdx);
        if(idx.isValid())
        {
            ui->tVImplants->expand(provIdx);
            ui->tVImplants->scrollTo(idx);
            ui->tVImplants->edit(idx);
        }
    }
}

void MainWindow::on_actionAdd_Implant_triggered()
{
    QModelIndex seriesIdx = ui->tVImplants->currentIndex();
    if(!seriesIdx.isValid())
        return;
    DbSeries * series = reinterpret_cast<DbSeries *>(m_pImpTreeModel->data(seriesIdx, DataRole).value<void*>());
    if(series)
    {
        DbImplant & implant = series->AddImplant(ImplantData());
        m_pImpTableModel->setSeries(series);
        ui->tabVImplants->setCurrentIndex(m_pImpTableModel->index(m_pImpTableModel->rowCount() - 1, 0));
        ui->tabVImplants->scrollToBottom();
        m_pImplantDialog->setImplant(&implant);
        int ret = m_pImplantDialog->exec();
        if(ret == QDialog::Rejected)
            series->RemoveImplant(implant);
        ui->tabVImplants->horizontalHeader()->setVisible(!(series->GetImplants().empty()));
        ui->tabVImplants->resizeColumnsToContents();
        m_pImpTableModel->Update();
    }
}

void MainWindow::on_actionAdd_Abutment_triggered()
{
    QModelIndex seriesIdx = ui->tVAbutments->currentIndex();
    if(!seriesIdx.isValid())
        return;
    DbSeries * series = reinterpret_cast<DbSeries *>(m_pAbutTreeModel->data(seriesIdx, DataRole).value<void*>());
    if(series)
    {
        DbAbutment & abutment = series->AddAbutment(AbutmentData());
        m_pAbutTableModel->setSeries(series);
        ui->tabVAbutments->setCurrentIndex(m_pAbutTableModel->index(m_pAbutTableModel->rowCount() - 1, 0));
        ui->tabVAbutments->scrollToBottom();
        m_pAbutmentDialog->setAbutment(&abutment);
        int ret = m_pAbutmentDialog->exec();
        if(ret == QDialog::Rejected)
        {
            series->RemoveAbutment(abutment);
        }
        ui->tabVAbutments->horizontalHeader()->setVisible(!(series->GetAbutment().empty()));
        ui->tabVAbutments->resizeColumnsToContents();
        m_pAbutTableModel->Update();
    }
}

void MainWindow::on_actionEdit_Note_triggered()
{
    QWidget * fWidget = focusWidget();
    if(fWidget == ui->tVImplants)
    {
        QModelIndex idx = ui->tVImplants->currentIndex();
        if(!idx.isValid())
            return;
        ItemType type = (ItemType)m_pImpTreeModel->data(idx, ItemTypeRole).toInt();
        if(type != RootItem)
            ui->tVImplants->edit(idx);
    }
    else if(fWidget == ui->tVAbutments)
    {
        QModelIndex idx = ui->tVAbutments->currentIndex();
        if(!idx.isValid())
            return;
        ItemType type = (ItemType)m_pAbutTreeModel->data(idx, ItemTypeRole).toInt();
        if(type != RootItem)
            ui->tVAbutments->edit(idx);
    }
    else if(fWidget == ui->tabVImplants)
    {
        QModelIndex idx = ui->tabVImplants->currentIndex();
        if(!idx.isValid())
            return;
        DbImplant * implant = reinterpret_cast<DbImplant*>(m_pImpTableModel->data(idx, DataRole).value<void*>());
        DbImplant tempImplant(implant->GetSeries());
        tempImplant = *implant;
        m_pImplantDialog->setImplant(&tempImplant);
        int ret = m_pImplantDialog->exec();
        if(ret == QDialog::Accepted)
        {
            *implant = tempImplant;
        }
        m_pImpTableModel->Update();
        ui->tabVImplants->setCurrentIndex(idx);
        ui->tabVImplants->resizeColumnsToContents();
    }
    else if(fWidget == ui->tabVAbutments)
    {
        QModelIndex idx = ui->tabVAbutments->currentIndex();
        if(!idx.isValid())
            return;
        DbAbutment * abutment = reinterpret_cast<DbAbutment*>(m_pAbutTableModel->data(idx, DataRole).value<void*>());
        DbAbutment tempAbutment(abutment->GetSeries());
        tempAbutment = *abutment;
        m_pAbutmentDialog->setAbutment(&tempAbutment);
        int ret = m_pAbutmentDialog->exec();
        if(ret == QDialog::Accepted)
        {
            *abutment = tempAbutment;
        }
        m_pAbutTableModel->Update();
        ui->tabVAbutments->setCurrentIndex(idx);
        ui->tabVAbutments->resizeColumnsToContents();
    }
}

void MainWindow::on_actionRemove_Note_triggered()
{
    QWidget * fWidget = focusWidget();
    if(fWidget == ui->tVImplants)
    {
        QModelIndex idx = ui->tVImplants->currentIndex();
        if(!idx.isValid())
            return;
        if(QMessageBox::question(this, "IA DataBase", "Remove all data?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        {
            ItemType type = (ItemType)m_pImpTreeModel->data(idx, ItemTypeRole).toInt();
            if(type == ProviderItem)
            {
                 DbProvider * provider = reinterpret_cast<DbProvider*>(m_pImpTreeModel->data(idx, DataRole).value<void*>());
                 if(provider)
                     iadb.RemoveProvider(*provider);
                 fillModels(Implants);
                 fillModels(Abutments);
                 ui->tVImplants->expand(m_pImpTreeModel->rootIndex());
                 ui->tVImplants->setCurrentIndex(m_pImpTreeModel->rootIndex());
    //             m_pImpTableModel->setSeries(NULL);
    //             m_pImpTableModel->Update();
    //             ui->tabVImplants->horizontalHeader()->setVisible(false);
            }
            if(type == SeriesItem)
            {

                QModelIndex parentIndex = m_pImpTreeModel->parent(idx);
                int row = parentIndex.row();
                DbSeries * series = reinterpret_cast<DbSeries*>(m_pImpTreeModel->data(idx, DataRole).value<void*>());
                if(!series)
                    return;
                DbProvider & provider = series->GetProvider();
                provider.RemoveSeries(*series);
                fillModels(Implants);
                fillModels(Abutments);
                parentIndex = m_pImpTreeModel->index(row, 0, m_pImpTreeModel->rootIndex());
                if(parentIndex.isValid())
                {
                    ui->tVImplants->setCurrentIndex(parentIndex);
                    ui->tVImplants->expand(parentIndex);
                }
                m_pImpTableModel->setSeries(NULL);
                m_pImpTableModel->Update();
                ui->tabVImplants->horizontalHeader()->setVisible(false);
                disableActions(ProviderItem, Implants);
            }
        }
    }
    else if(fWidget == ui->tVAbutments)
    {
        QModelIndex idx = ui->tVAbutments->currentIndex();
        if(!idx.isValid())
            return;
        if(QMessageBox::question(this, "IA DataBase", "Remove all data?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        {
            ItemType type = (ItemType)m_pAbutTreeModel->data(idx, ItemTypeRole).toInt();
            if(type == ProviderItem)
            {
                 DbProvider * provider = reinterpret_cast<DbProvider*>(m_pAbutTreeModel->data(idx, DataRole).value<void*>());
                 if(provider)
                     iadb.RemoveProvider(*provider);
                 fillModels(Implants);
                 fillModels(Abutments);
                 ui->tVAbutments->expand(m_pAbutTreeModel->rootIndex());
                 ui->tVAbutments->setCurrentIndex(m_pAbutTreeModel->rootIndex());
    //             m_pImpTableModel->setSeries(NULL);
    //             m_pImpTableModel->Update();
    //             ui->tabVImplants->horizontalHeader()->setVisible(false);
            }
            if(type == SeriesItem)
            {

                QModelIndex parentIndex = m_pAbutTreeModel->parent(idx);
                DbSeries * series = reinterpret_cast<DbSeries*>(m_pAbutTreeModel->data(idx, DataRole).value<void*>());
                if(!series)
                    return;
                DbProvider & provider = series->GetProvider();
                provider.RemoveSeries(*series);
                m_pAbutTreeModel->Update();
                if(parentIndex.isValid())
                {
                    ui->tVAbutments->setCurrentIndex(parentIndex);
                    ui->tVAbutments->expand(parentIndex);
                }
                m_pAbutTableModel->setSeries(NULL);
                m_pAbutTableModel->Update();
                ui->tabVAbutments->horizontalHeader()->setVisible(false);
                disableActions(ProviderItem, Implants);
            }
        }
    }
    else if(fWidget == ui->tabVImplants)
    {
        QItemSelectionModel * selectionModel  = ui->tabVImplants->selectionModel();
        QModelIndexList miList = selectionModel->selectedIndexes();
        DbSeries * series = reinterpret_cast<DbSeries*>(m_pImpTreeModel->data(ui->tVImplants->currentIndex(), DataRole).value<void*>());;
        if(!series)
            return;
        QList<DbImplant *> impList;
        foreach(QModelIndex idx, miList)
        {
            if(!idx.isValid())
                return;
            if(idx.column() > 0)
                continue;
            DbImplant * implant = reinterpret_cast<DbImplant*>(m_pImpTableModel->data(idx, DataRole).value<void*>());
            if(implant)
                impList << implant;
        }
        for(int i = 0; i < impList.count(); i++)
            series->RemoveImplant(*(impList[i]));
        m_pImpTableModel->Update();
        ui->tabVImplants->horizontalHeader()->setVisible(!(series->GetImplants().empty()));
        ui->tabVImplants->resizeColumnsToContents();
    }
    else if(fWidget == ui->tabVAbutments)
    {
        QItemSelectionModel * selectionModel  = ui->tabVAbutments->selectionModel();
        QModelIndexList miList = selectionModel->selectedIndexes();
        DbSeries * series = reinterpret_cast<DbSeries*>(m_pAbutTreeModel->data(ui->tVAbutments->currentIndex(), DataRole).value<void*>());;
        if(!series)
            return;
        QList<DbAbutment *> abutList;
        foreach(QModelIndex idx, miList)
        {
            if(!idx.isValid())
                return;
            if(idx.column() > 0)
                continue;
            DbAbutment * abutment = reinterpret_cast<DbAbutment*>(m_pAbutTableModel->data(idx, DataRole).value<void*>());
            if(abutment)
                abutList << abutment;
        }
        for(int i = 0; i < abutList.count(); i++)
            series->RemoveAbutment(*(abutList[i]));
        m_pAbutTableModel->Update();
        ui->tabVAbutments->horizontalHeader()->setVisible(!(series->GetAbutment().empty()));
        ui->tabVAbutments->resizeColumnsToContents();
    }
}

void MainWindow::on_actionLoad_triggered()
{
    QString dir = qApp->applicationDirPath();
    if(!fileName.isEmpty())
    {
        QFileInfo fi(fileName);
        dir = fi.absolutePath();
    }
    QString name = QFileDialog::getOpenFileName(this, tr("Load Data Base"), dir, tr("Data Base (*.xml)"));
    if(name.isEmpty() || name == fileName)
        return;
    if(!fileName.isEmpty() && QMessageBox::question(this, tr("Save data base"), tr("Do you want to save the current data base?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
        iadb.SaveXml_All(fileName.toLocal8Bit().data());
    fileName = name;
    iadb.LoadXml_All(fileName.toLocal8Bit().data());
    fillModels(Implants);
    fillModels(Abutments);
}

void MainWindow::on_pBOK_clicked()
{
    iadb.SaveXml_All(fileName.toLocal8Bit().data());
    close();
}
