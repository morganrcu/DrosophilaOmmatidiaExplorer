#include "GraphPlotterDockWidget.h"
#include "ui_GraphPlotterDockWidget.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkContextScene.h>
#include <vtkTable.h>
#include <vtkIntArray.h>
#include <vtkDoubleArray.h>
#include <vtkPlot.h>
#include <vtkAxis.h>
#include <vtkPen.h>
#include <vtkDelimitedTextWriter.h>
#include <QFileDialog>
GraphPlotterDockWidget::GraphPlotterDockWidget(QWidget *parent) :
    QDockWidget(parent),
    m_pUI(new Ui::GraphPlotterDockWidget)
{
    m_pUI->setupUi(this);


    // Set up a 2D scene, add an XY chart to it
    m_View =vtkSmartPointer<vtkContextView>::New();


    // VTK/Qt wedded
    m_View->SetRenderWindow(this->m_pUI->graphsQVTKWidget->GetRenderWindow());

    m_View->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

    m_Chart =vtkSmartPointer<vtkChartXY>::New();

    m_View->GetScene()->AddItem(m_Chart);

    // Create a table with some points in it...
    m_Table =vtkSmartPointer<vtkTable>::New();
    connect(this->m_pUI->clearButton,SIGNAL(clicked()),SLOT(slotClear()));
    connect(this->m_pUI->exportDataButton,SIGNAL(clicked()),SLOT(slotExportData()));
}
#if 0
void GraphPlotterDockWidget::SetTemporalReference(const vtkSmartPointer<vtkDoubleArray> & tScale){
    m_Table->AddColumn(tScale);
}
#endif
void GraphPlotterDockWidget::slotClear(){
	for(int c=m_Table->GetNumberOfColumns()-1;c>0;c--){
		m_Table->RemoveColumn(c);
	}
	m_Colors.clear();
	m_Dashed.clear();
	m_Chart->ClearPlots();
}
void GraphPlotterDockWidget::SetLength(unsigned int samples){
	vtkSmartPointer<vtkDoubleArray> arrT =vtkSmartPointer<vtkDoubleArray>::New();
	arrT->SetName("t");
	arrT->SetNumberOfTuples(samples);

	for(int t=0;t<samples;t++){
		arrT->SetTuple1(t,t);
	}

	assert(m_Table->GetNumberOfColumns()==0);

	m_Table->AddColumn(arrT);

}

void GraphPlotterDockWidget::AddPlot(const vtkSmartPointer<vtkDoubleArray> & series ,itk::FixedArray<double,3>  & color,bool dashed){
	std::cout<< m_Table->GetNumberOfColumns() << std::endl;
    m_Table->AddColumn(series);
    std::cout<< m_Table->GetNumberOfColumns() << std::endl;
    m_Colors.push_back(color);
    m_Dashed.push_back(dashed);

}

void GraphPlotterDockWidget::Draw(){
   //m_Chart->ClearPlots();
	m_Chart->ClearPlots();
    for(int c=1;c<this->m_Table->GetNumberOfColumns();c++){
        vtkSmartPointer<vtkPlot> line = m_Chart->AddPlot(vtkChart::LINE);
        line->SetInputData(this->m_Table,0,c);
        line->SetColor(m_Colors[c-1][0],m_Colors[c-1][1],m_Colors[c-1][2]);
        if(m_Dashed[c-1]){
        	line->GetPen()->SetLineType(vtkPen::DASH_LINE);
        }
    }
    m_Chart->SetAutoAxes(true);
    m_Chart->SetAutoSize(true);
    m_Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("t");

    m_View->GetRenderWindow()->Render();
}

void GraphPlotterDockWidget::slotExportData(){

	vtkSmartPointer<vtkDelimitedTextWriter> writer = vtkSmartPointer<vtkDelimitedTextWriter>::New();

	QFileDialog dialog;

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save data"),"/home/jana/untitled.png",tr("Comma Separated Files (*.csv)"));
	writer->SetFileName(fileName.toStdString().c_str());

	writer->SetInputData(this->m_Table);

	writer->Write();

}
GraphPlotterDockWidget::~GraphPlotterDockWidget()
{
    delete m_pUI;
}
