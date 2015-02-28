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

}

void GraphPlotterDockWidget::SetTemporalReference(const vtkSmartPointer<vtkDoubleArray> & tScale){
    m_Table->AddColumn(tScale);
}

void GraphPlotterDockWidget::AddPlot(const vtkSmartPointer<vtkDoubleArray> & series ){

    m_Table->AddColumn(series);

}

void GraphPlotterDockWidget::Draw(){
   //m_Chart->ClearPlots();
    for(int c=1;c<this->m_Table->GetNumberOfColumns();c++){
        vtkSmartPointer<vtkPlot> line = m_Chart->AddPlot(vtkChart::LINE);
        line->SetInputData(this->m_Table,0,c);
        line->SetColor(0,255.0,0,255.0);

    }
    m_Chart->SetAutoAxes(true);
    m_Chart->SetAutoSize(true);
    m_Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("t");

    m_View->GetRenderWindow()->Render();
}

GraphPlotterDockWidget::~GraphPlotterDockWidget()
{
    delete m_pUI;
}
