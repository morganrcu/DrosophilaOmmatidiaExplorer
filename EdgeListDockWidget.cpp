#include "EdgeListDockWidget.h"
#include "ui_EdgeListDockWidget.h"
#include <QMenu>
EdgeListDockWidget::EdgeListDockWidget(QWidget *parent) :
    QDockWidget(parent),
    m_pUI(new Ui::EdgeListDockWidget)
{
    m_pUI->setupUi(this);

    this->m_pUI->edgesTableWidget->setColumnCount(2);;
    QStringList header;
    header << "source" << "target";
    this->m_pUI->edgesTableWidget->setHorizontalHeaderLabels(header);

      connect(this->m_pUI->edgesTableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(slotEdgeTableSelectionChanged()));

    m_pUI->edgesTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->m_pUI->edgesTableWidget,SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotDisplayMenu(QPoint)));
}

void EdgeListDockWidget::slotDisplayMenu(const QPoint &pos)
{
    QMenu menu(this);

    QAction *u = menu.addAction("plot"); // there can be more than one
    QAction *a = menu.exec(m_pUI->edgesTableWidget->viewport()->mapToGlobal(pos));
    if (a == u)
    {
        emit DrawEdgeRequested(this->GetSelectedEdge());
        // do what you want or call another function
    }
}

void EdgeListDockWidget::slotEdgeTableSelectionChanged(){

    emit SelectedEdgeChanged(this->m_RowToEdge[this->m_pUI->edgesTableWidget->selectionModel()->currentIndex().row()]);
}


typename EdgeListDockWidget::EdgeContainer::AJEdgeHandler EdgeListDockWidget::GetSelectedEdge() {

    return this->m_RowToEdge[this->m_pUI->edgesTableWidget->selectionModel()->currentIndex().row()];

}

EdgeListDockWidget::~EdgeListDockWidget()
{
    delete m_pUI;
}

void EdgeListDockWidget::Draw(){

    this->m_pUI->edgesTableWidget->setRowCount(m_EdgesContainer->GetNumEdges());

       int row=0;




    for(auto it = this->m_EdgesContainer->EdgesBegin();it!=this->m_EdgesContainer->EdgesEnd();it++){
        QTableWidgetItem * source =  new QTableWidgetItem((tr("%1").arg(this->m_EdgesContainer->GetAJEdgeSource(*it))));
        QTableWidgetItem * target =  new QTableWidgetItem((tr("%1").arg(this->m_EdgesContainer->GetAJEdgeTarget(*it))));


        //this->m_EdgeToRow[*it]=row;
        this->m_RowToEdge[row]=*it;
        this->m_pUI->edgesTableWidget->setItem(row,0,source);
        this->m_pUI->edgesTableWidget->setItem(row,1,target);
        row++;
    }

}
