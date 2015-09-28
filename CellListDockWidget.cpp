#include "CellListDockWidget.h"
#include "ui_CellListDockWidget.h"
#include <QComboBox>
CellListDockWidget::CellListDockWidget(QWidget *parent) :
    QDockWidget(parent),
    m_pUI(new Ui::CellListDockWidget)
{
    m_pUI->setupUi(this);
    connect(this->m_pUI->cellTableWidget,SIGNAL(currentCellChanged(int,int,int,int)),SLOT(slotCellTableSelectionChanged(int,int,int,int)));
}

void CellListDockWidget::SetSelectedCell(const CellsContainer::CellVertexHandler & cell){
    this->m_pUI->cellTableWidget->blockSignals(true);
    this->m_pUI->cellTableWidget->selectRow(cell);
    this->m_pUI->cellTableWidget->blockSignals(false);
}

void CellListDockWidget::ClearSelection(){
    this->m_pUI->cellTableWidget->blockSignals(true);
    this->m_pUI->cellTableWidget->clearSelection();
    this->m_pUI->cellTableWidget->blockSignals(false);
}
void CellListDockWidget::Draw(){
    this->m_pUI->cellTableWidget->setRowCount(this->m_Cells->GetNumberOfCells());
    this->m_pUI->cellTableWidget->setColumnCount(6);

    int row=0;

    m_SignalMapper = new QSignalMapper(this);



    for(auto it = this->m_Cells->CellsBegin();it!=this->m_Cells->CellsEnd();++it){
    	auto cell = this->m_Cells->GetCell(*it);
        QComboBox * comboBox = new QComboBox;
        comboBox->insertItem(0,"UNKNOWN");
        comboBox->insertItem(1,"Anterior Cone");
        comboBox->insertItem(2,"Posterior Cone");
        comboBox->insertItem(3,"Polar Cone");
        comboBox->insertItem(4,"Equatorial Cone");
        comboBox->insertItem(5,"Primary");
        comboBox->insertItem(6,"Long Secundary");
        comboBox->insertItem(7,"Short Secundary");
        comboBox->insertItem(8,"Tertiary");
        comboBox->insertItem(9,"Bristle");
        comboBox->insertItem(10,"Surviving");
        comboBox->insertItem(11,"Apoptotic");

        comboBox->setCurrentIndex(cell->GetCellType());


        this->m_pUI->cellTableWidget->setCellWidget(row,0,comboBox);
        //comboBox->setProperty("cell",*it);

        connect(comboBox, SIGNAL(currentIndexChanged(int)), m_SignalMapper, SLOT(map()));
        m_SignalMapper->setMapping(comboBox, QString("%1-%2").arg(row).arg(*it));

        //connect(comboBox,SIGNAL(currentIndexChanged(int)),SLOT(slotCellModified(*it)));

        QTableWidgetItem * areaWidgetItem =new QTableWidgetItem;
        areaWidgetItem->setData(0,cell->GetArea());

        this->m_pUI->cellTableWidget->setItem(row,1,areaWidgetItem);

        QTableWidgetItem * perimeterWidgetItem =new QTableWidgetItem;
        perimeterWidgetItem->setData(0,cell->GetPerimeterLength());

        this->m_pUI->cellTableWidget->setItem(row,2,perimeterWidgetItem);

        QTableWidgetItem * xxWidgetItem =new QTableWidgetItem;
        xxWidgetItem->setData(0,cell->GetXX());

        this->m_pUI->cellTableWidget->setItem(row,3,xxWidgetItem);

        QTableWidgetItem * xyWidgetItem =new QTableWidgetItem;
        xyWidgetItem->setData(0,cell->GetXY());

        this->m_pUI->cellTableWidget->setItem(row,4,xyWidgetItem);

        QTableWidgetItem * yyWidgetItem =new QTableWidgetItem;
        yyWidgetItem->setData(0,cell->GetXY());
        this->m_pUI->cellTableWidget->setItem(row,5,yyWidgetItem);




        row++;
    }

    connect(m_SignalMapper, SIGNAL(mapped(const QString &)),this, SLOT(slotCellModified(const QString &)));

}

void CellListDockWidget::slotCellModified(const QString & position){

	QStringList coordinates = position.split("-");
	 int row = coordinates[0].toInt();
	 int col = coordinates[1].toInt();


	QComboBox * changed = static_cast<QComboBox*>(this->m_pUI->cellTableWidget->cellWidget(row,0));

	this->m_Cells->GetCell(col)->SetCellType(changed->currentIndex());
	std::cout << "Setting type to " <<this->m_Cells->GetCell(col)->GetCellType() << std::endl;
	emit ModifiedCells();
}
void CellListDockWidget::slotCellTableSelectionChanged(int row, int column,int,int){

	this->m_pUI->cellTableWidget->cellWidget(row,0)->setEnabled(true);
	emit SelectedCellChanged(static_cast<CellsContainer::CellVertexHandler>(this->m_pUI->cellTableWidget->currentRow()));
}

CellListDockWidget::~CellListDockWidget()
{
    delete m_pUI;
}

