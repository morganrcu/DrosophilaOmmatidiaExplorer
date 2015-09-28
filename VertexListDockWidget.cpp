#include "VertexListDockWidget.h"
#include "ui_VertexListDockWidget.h"

VertexListDockWidget::VertexListDockWidget(QWidget *parent) :
    QDockWidget(parent),
    m_pUI(new Ui::VertexListDockWidget)
{
    m_pUI->setupUi(this);
    this->m_pUI->verticesTableWidget->setColumnCount(3);

    QStringList header;
    header << "x" << "y" << "z";
    this->m_pUI->verticesTableWidget->setHorizontalHeaderLabels(header);

     connect(this->m_pUI->verticesTableWidget,SIGNAL(itemSelectionChanged()),SLOT(slotVertexTableSelectionChanged()));

}

void VertexListDockWidget::slotVertexTableSelectionChanged(){

    emit SelectedVertexChanged(static_cast<const VertexContainer::AJVertexHandler>(this->m_pUI->verticesTableWidget->selectionModel()->currentIndex().row()));
}

VertexListDockWidget::~VertexListDockWidget()
{
    delete m_pUI;
}

 VertexListDockWidget::VertexContainer::AJVertexHandler VertexListDockWidget::GetSelectedVertex() {

    return this->m_pUI->verticesTableWidget->selectionModel()->currentIndex().row();

}



void VertexListDockWidget::SetSelectedVertex(const VertexContainer::AJVertexHandler & vertex){

    this->m_pUI->verticesTableWidget->blockSignals(true);
    this->m_pUI->verticesTableWidget->selectRow(vertex);
    this->m_pUI->verticesTableWidget->blockSignals(false);

}
void VertexListDockWidget::ClearSelection(){
    this->m_pUI->verticesTableWidget->blockSignals(true);
    this->m_pUI->verticesTableWidget->clearSelection();
    this->m_pUI->verticesTableWidget->blockSignals(false);

}
void VertexListDockWidget::Draw(){
    this->m_pUI->verticesTableWidget->setRowCount(this->m_VertexContainer->GetNumVertices());




    int row=0;

    for(auto it = this->m_VertexContainer->VerticesBegin();it!=this->m_VertexContainer->VerticesEnd();++it){

        QTableWidgetItem * itemX =  new QTableWidgetItem((tr("%1").arg(this->m_VertexContainer->GetAJVertex(*it)->GetPosition()[0])));
        QTableWidgetItem * itemY =  new QTableWidgetItem((tr("%1").arg(this->m_VertexContainer->GetAJVertex(*it)->GetPosition()[1])));
        QTableWidgetItem * itemZ =  new QTableWidgetItem((tr("%1").arg(this->m_VertexContainer->GetAJVertex(*it)->GetPosition()[2])));

        this->m_pUI->verticesTableWidget->setItem(row,0,itemX);
        this->m_pUI->verticesTableWidget->setItem(row,1,itemY);
        this->m_pUI->verticesTableWidget->setItem(row,2,itemZ);


        row++;
    }

}
