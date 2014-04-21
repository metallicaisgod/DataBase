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
    //m_pImplantsTreeModel = new QStandardItemModel();
    //m_pAbutmentsTreeModel = new QStandardItemModel();
    fileName = "..\\DataBase\\implants_db.xml";
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
    iadb.LoadXml_All(fileName.toLocal8Bit().data());

    t_implantsEnumNFsp impl_enum = iadb.GetImplantsEnumerator(impl_length_comp<db::DbImplant>(), db::no_filter<db::DbImplant*>());
    DbImplant* pImplant = NULL;
//    QStandardItem * parentItem = new QStandardItem("Implants");
//    parentItem->setCheckable(true);
//    parentItem->setCheckState(Qt::Checked);
//    parentItem->setEditable(false);

    DbSeries * series;
    DbProvider * provider;
    providerList.clear();
    while (impl_enum->MoveNext())
    {
        pImplant = impl_enum->GetCurrent();        
        series = pImplant->GetSeries();
        provider = &(series->GetProvider());
        if(!providerList.contains(provider))
        {
            providerList << provider;
        }
    }
    m_pTreeModel = new TreeModel(providerList, Implants, this);
    //m_pImplantsTreeModel->appendRow(parentItem);
    delete impl_enum;
    ui->tVImplants->setModel(m_pTreeModel);
    ui->tVImplants->expand(m_pTreeModel->rootIndex());
    m_pTableModel = new TableModel(Implants, this);
    ui->tabVImplants->setModel(m_pTableModel);
    ui->tabVImplants->horizontalHeader()->setVisible(false);
    ui->tabVImplants->resizeColumnsToContents();

    connect(m_pTableModel, SIGNAL(clicked()), this, SLOT(tableImplantsClicked()));
    connect(m_pTreeModel, SIGNAL(stateChanged(QModelIndex)), SLOT(treeImplantStateChanged(QModelIndex)));
}

void MainWindow::on_tVImplants_clicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pTreeModel->data(index, SeriesRole).value<void *>());
    //if(series)
    //{
    m_pTableModel->setSeries(series);
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
        if(m_pTreeModel->rowCount(index) > 0)
        {
            for(int i = 0; i < m_pTreeModel->rowCount(index); i++)
            {
                QModelIndex childIndex = m_pTreeModel->index(i, 0, index);
                db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pTreeModel->data(childIndex, SeriesRole).value<void *>());
                if(series == m_pTableModel->series())
                {
                    m_pTableModel->setSeries(NULL);
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
            db::DbSeries * series = reinterpret_cast<db::DbSeries *>(m_pTreeModel->data(index, SeriesRole).value<void *>());
            if(series == m_pTableModel->series())
            {
                m_pTableModel->setSeries(NULL);
                ui->tabVImplants->horizontalHeader()->setVisible(false);
            }
        }
    }
}

void MainWindow::tableImplantsClicked()
{
    QModelIndex idx = ui->tVImplants->currentIndex();
    m_pTreeModel->Update();
    ui->tVImplants->setCurrentIndex(idx);
}

void MainWindow::treeImplantStateChanged(QModelIndex index)
{
    if(!index.isValid())
        return;
    m_pTreeModel->Update();
    on_tVImplants_clicked(index);
    ui->tVImplants->setCurrentIndex(index);
    ui->tVImplants->expand(index);
}
