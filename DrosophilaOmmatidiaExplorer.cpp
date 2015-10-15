#include <QObject>
#include <AJGraphToTissueDescriptor.h>
#include "ui_DrosophilaOmmatidiaExplorer.h"
#include "DrosophilaOmmatidiaExplorer.h"

#include "DetectVerticesDialog.h"

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkVectorText.h>
#include <vtkSmartPointer.h>
#include <vtkPropPicker.h>
#include <vtkWorldPointPicker.h>

#include <HessianImageToVertexnessImageFilter.h>
#include <QFileDialog>
#include <QMessageBox>


#include <vtkHedgeHog.h>
#include <vtkProperty.h>
#include <vtkCubeSource.h>


#include <itkImageToVTKImageFilter.h>
#include <vtkImageData.h>

#include "LocalMaximaGraphInitializer.h"
#include "HessianGraphInitializer.h"
#include "RefineVerticesToNearestVertexnessMaxima.h"
#include "PredictAJGraph.h"

#include "VertexMolecularDistributionDescriptor.h"
#include "EdgeMolecularDistributionDescriptor.h"
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkFloatArray.h>
#include <vtkPlot.h>

#include "vtkMultiBlockDataSet.h"
#include "vtkPCAStatistics.h"
#include "vtkTable.h"
#include <string>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <string>
#ifdef USE_FFMPEG
#include <vtkFFMPEGWriter.h>
#endif

#include <OmmatidiaTissue.h>

#include <AJTrackingFrame.h>
// Constructor
DrosophilaOmmatidiaExplorer::DrosophilaOmmatidiaExplorer()
{

    m_pUI = new Ui_DrosophilaOmmatidiaExplorer;
    m_pUI->setupUi(this);

    //Set up combo boxes


    this->m_pUI->showDeconvolutedModeComboBox->insertItem(0,"Volume");
    this->m_pUI->showDeconvolutedModeComboBox->insertItem(1,"Slice");
    this->m_pUI->showDeconvolutedModeComboBox->setCurrentIndex(0);

    this->m_pUI->showOriginalModeComboBox->insertItem(0,"Volume");
    this->m_pUI->showOriginalModeComboBox->insertItem(1,"Slice");
    this->m_pUI->showOriginalModeComboBox->setCurrentIndex(0);


    this->m_pUI->showPlatenessModeComboBox->insertItem(0,"Volume");
    this->m_pUI->showPlatenessModeComboBox->insertItem(1,"Slice");
    this->m_pUI->showPlatenessModeComboBox->setCurrentIndex(0);

    //Setup renderer

    m_Renderer=vtkSmartPointer<vtkRenderer>::New();


    m_RenderWindow =this->m_pUI->qvtkWidget->GetRenderWindow();
    m_RenderWindow->AddRenderer(this->m_Renderer);
    m_RenderWindow->SetAlphaBitPlanes(1); //enable usage of alpha channel


    this->m_RenderWindowInteractor=this->m_pUI->qvtkWidget->GetInteractor();
    this->m_RenderWindowInteractor->SetDesiredUpdateRate(1.0);


    m_DeconvolutedDrawer.SetRenderer(this->m_Renderer);
    m_OriginalDrawer.SetRenderer(this->m_Renderer);
    m_PlatenessDrawer.SetRenderer(this->m_Renderer);
    m_VertexnessDrawer.SetRenderer(this->m_Renderer);
    m_MolecularImageDrawer.SetRenderer(this->m_Renderer);
    m_MotionVolumeDrawer.SetRenderer(this->m_Renderer);
    m_VertexLocationsDrawer.SetRenderer(this->m_Renderer);
    m_VertexMotionsDrawer.SetRenderer(this->m_Renderer);
    m_EdgesDrawer.SetRenderer(this->m_Renderer);
    m_CellDrawer.SetRenderer(this->m_Renderer);

    //Setup connection forwarder
    this->m_QtToVTKConnections= vtkSmartPointer<vtkEventQtSlotConnect>::New();

    //Project controls
    connect(this->m_pUI->actionOpenFile, SIGNAL(triggered()), SLOT(slotOpenProject()));
    connect(this->m_pUI->actionExit, SIGNAL(triggered()), SLOT(slotExit()));


    //Algorithms
    connect(this->m_pUI->actionDetectVertices,SIGNAL(triggered()),SLOT(slotDoVertexLocation()));
    connect(this->m_pUI->actionTrackVertices,SIGNAL(triggered()),SLOT(slotDoVertexTracking()));
    connect(this->m_pUI->actionVertexMolecularDistribution,SIGNAL(triggered()),SLOT(slotDoVertexMolecularDistribution()));
    connect(this->m_pUI->actionEdgeMolecularDistribution,SIGNAL(triggered()),SLOT(slotDoEdgeMolecularDistribution()));
    connect(this->m_pUI->actionExportMovie,SIGNAL(triggered()),SLOT(slotDoExportMovie()));
    connect(this->m_pUI->actionRefineVertices,SIGNAL(triggered()),SLOT(slotDoRefineVertexLocation()));
    connect(this->m_pUI->actionCells,SIGNAL(triggered()),SLOT(slotDoCells()));

    connect(this->m_pUI->actionTrackOne,SIGNAL(triggered()),SLOT(slotDoTrackOne()));

    //Visualization

    connect(this->m_pUI->frameSlider,SIGNAL(valueChanged(int)),this,SLOT(slotFrameChanged(int)));

    connect(this->m_pUI->showOriginalGroupBox,SIGNAL(toggled(bool)),SLOT(slotShowOriginalChanged(bool)));
    connect(this->m_pUI->showOriginalModeComboBox,SIGNAL(currentIndexChanged(QString)),SLOT(slotShowOriginalModeChanged(QString)));
    connect(this->m_pUI->showOriginalOpacitySlider,SIGNAL(valueChanged(int)),SLOT(slotShowOriginalOpacityChanged(int)));
    connect(this->m_pUI->showOriginalSliceSpinBox,SIGNAL(valueChanged(int)),SLOT(slotShowOriginalSliceChanged(int)));

    connect(this->m_pUI->showDeconvolutedGroupBox,SIGNAL(toggled(bool)),SLOT(slotShowDeconvolutedChanged(bool)));
    connect(this->m_pUI->showDeconvolutedModeComboBox,SIGNAL(currentIndexChanged(QString)),SLOT(slotShowDeconvolutedModeChanged(QString)));
    connect(this->m_pUI->showDeconvolutedOpacitySlider,SIGNAL(valueChanged(int)),SLOT(slotShowDeconvolutedOpacityChanged(int)));
    connect(this->m_pUI->showDeconvolutedSliceSpinBox,SIGNAL(valueChanged(int)),SLOT(slotShowDeconvolutedSliceChanged(int)));

    connect(this->m_pUI->showMotionFieldGroupBox,SIGNAL(toggled(bool)),SLOT(slotShowMotionFieldChanged(bool)));

    connect(this->m_pUI->showVertexMotionCBox,SIGNAL(toggled(bool)),SLOT(slotShowVertexMotionChanged(bool)));

    connect(this->m_pUI->showAJVerticesCheckBox,SIGNAL(toggled(bool)),SLOT(slotShowVertexLocationsChanged(bool)));

    connect(this->m_pUI->showPlatenessGroupBox,SIGNAL(toggled(bool)),SLOT(slotShowPlatenessChanged(bool)));
    connect(this->m_pUI->showPlatenessModeComboBox,SIGNAL(currentIndexChanged(QString)),SLOT(slotShowPlatenessModeChanged(QString)));
    connect(this->m_pUI->showPlatenessOpacitySlider,SIGNAL(valueChanged(int)),SLOT(slotShowPlatenessOpacityChanged(int)));
    connect(this->m_pUI->showPlatenessSliceSpinBox,SIGNAL(valueChanged(int)),SLOT(slotShowPlatenessSliceChanged(int)));

    connect(this->m_pUI->showVertexnessGroupBox,SIGNAL(toggled(bool)),SLOT(slotShowVertexnessChanged(bool)));
    connect(this->m_pUI->showVertexnessOpacitySlider,SIGNAL(valueChanged(int)),SLOT(slotShowVertexnessOpacityChanged(int)));

    connect(this->m_pUI->showCellsCheckBox,SIGNAL(toggled(bool)),SLOT(slotShowCellsChanged(bool)));


    connect(this->m_pUI->showMolecularChannelGroupBox,SIGNAL(toggled(bool)),SLOT(slotShowMolecularChanged(bool)));
    connect(this->m_pUI->showMolecularChannelOpacitySlider,SIGNAL(valueChanged(int)),SLOT(slotShowMolecularOpacityChanged(int)));


    // Set up action signals and slots

    m_SelectedVertex=-1;
    m_PointWidget=vtkSmartPointer<vtkPointWidget>::New();
    m_PointWidget->AllOff();
    m_PointWidget->SetInteractor(this->m_RenderWindowInteractor);



    connect(this->m_pUI->actionSelectVertex,SIGNAL(toggled(bool)),SLOT(slotVertexSelectionToggled(bool)));
    connect(this->m_pUI->actionAddVertices,SIGNAL(toggled(bool)),SLOT(slotVertexAdditionToggled(bool)));
    connect(this->m_pUI->actionDeleteVertex,SIGNAL(triggered()),SLOT(slotDeleteVertex()));
    connect(this->m_pUI->actionMoveVertex,SIGNAL(toggled(bool)),SLOT(slotVertexMoveToggled(bool)));





    connect(this->m_pUI->actionAddEdge,SIGNAL(toggled(bool)),SLOT(slotEdgeAdditionToggled(bool)));
    connect(this->m_pUI->actionSelectEdge,SIGNAL(toggled(bool)),SLOT(slotEdgeSelectionToggled(bool)));

    connect(this->m_pUI->actionDeleteEdge,SIGNAL(triggered()),SLOT(slotDeleteEdge()));

    connect(this->m_pUI->actionSelectCell,SIGNAL(toggled(bool)),SLOT(slotCellSelectionToggled(bool)));

    connect(this->m_pUI->actionPlotSelectedEdgeLength,SIGNAL(triggered()),SLOT(slotPlotSelectedEdgeLength()));
    connect(this->m_pUI->actionPlotSelectedCellArea,SIGNAL(triggered()),SLOT(slotPlotSelectedCellArea()));

    //connect(this->m_pUI->actionShowEdges,SIGNAL(toggled(bool)),SLOT(slotShowEdges(bool)));

    this->m_IsSelectedEdge=false;

    m_pTemporaryLineStruct=NULL;


    vtkSmartPointer<vtkCubeSource> cube=vtkSmartPointer<vtkCubeSource>::New();

    cube->SetBounds(0,1,0,1,0,1);
    cube->Update();
    m_PointWidget->SetInputData(cube->GetOutput());
    m_PointWidget->PlaceWidget();

    m_pTissueDockWidget = new OmmatidiaTissueDockWidget(this);
    this->m_pTissueDockWidget->setHidden(true);
    this->addDockWidget(Qt::RightDockWidgetArea,this->m_pTissueDockWidget);
    connect(this->m_pUI->actionShowTissueTree,SIGNAL(toggled(bool)),this->m_pTissueDockWidget,SLOT(setVisible(bool)));


    m_pVertexListDockWidget=  new VertexListDockWidget(this);
    m_pVertexListDockWidget->setHidden(true);
    this->addDockWidget(Qt::RightDockWidgetArea,this->m_pVertexListDockWidget);
    connect(this->m_pUI->actionShowVertices,SIGNAL(toggled(bool)),this->m_pVertexListDockWidget,SLOT(setVisible(bool)));

    connect(this->m_pVertexListDockWidget,SIGNAL(visibilityChanged(bool)),this->m_pUI->actionShowVertices,SLOT(setChecked(bool)));
    connect(this->m_pVertexListDockWidget,SIGNAL(SelectedVertexChanged(AJGraph<AJVertex,AJEdge>::AJVertexHandler)),SLOT(slotVertexTableSelectionChanged(AJGraph<AJVertex,AJEdge>::AJVertexHandler)));


    m_pEdgeListDockWidget= new EdgeListDockWidget(this);
    m_pEdgeListDockWidget->setHidden(true);
    this->addDockWidget(Qt::RightDockWidgetArea,this->m_pEdgeListDockWidget);
    connect(this->m_pUI->actionShowEdgeList,SIGNAL(toggled(bool)),this->m_pEdgeListDockWidget,SLOT(setVisible(bool)));
    connect(this->m_pEdgeListDockWidget,SIGNAL(DrawEdgeRequested(AJGraph<AJVertex,AJEdge>::AJEdgeHandler)),SLOT(slotPlotEdgeLength(AJGraph<AJVertex,AJEdge>::AJEdgeHandler)));
    connect(this->m_pEdgeListDockWidget,SIGNAL(SelectedEdgeChanged(AJGraph<AJVertex,AJEdge>::AJEdgeHandler)),SLOT(slotEdgeTableSelectionChanged(AJGraph<AJVertex,AJEdge>::AJEdgeHandler)));
    connect(this->m_pEdgeListDockWidget,SIGNAL(visibilityChanged(bool)),this->m_pUI->actionShowEdgeList,SLOT(setChecked(bool)));


    m_pGraphPlotterDockWidget = new GraphPlotterDockWidget(this);
    m_pGraphPlotterDockWidget->setHidden(true);
    this->addDockWidget(Qt::RightDockWidgetArea,this->m_pGraphPlotterDockWidget);
    connect(this->m_pUI->actionShowCharts,SIGNAL(toggled(bool)),this->m_pGraphPlotterDockWidget,SLOT(setVisible(bool)));
    connect(this->m_pGraphPlotterDockWidget,SIGNAL(visibilityChanged(bool)),this->m_pUI->actionShowCharts,SLOT(setChecked(bool)));


    connect(this->m_pUI->actionPlotEdgeDistribution,SIGNAL(triggered()),this,SLOT(slotPlotSelectedEdgeMolecularDistribution()));
    connect(this->m_pUI->actionPlotVertexDistribution,SIGNAL(triggered()),this,SLOT(slotPlotSelectedVertexMolecularDistribution()));


    m_pCellListDockWidget= new CellListDockWidget(this);
    //m_pCellListDockWidget->setHidden(true);
    this->addDockWidget(Qt::RightDockWidgetArea,this->m_pCellListDockWidget);
    connect(this->m_pCellListDockWidget,SIGNAL(ModifiedCells()),SLOT(slotCellsShanged()));
    connect(this->m_pCellListDockWidget,SIGNAL(SelectedCellChanged(CellGraph<Cell>::CellVertexHandler)),SLOT(slotSelectedCellChanged(CellGraph<Cell>::CellVertexHandler)));

    connect(this->m_pUI->actionShowCells,SIGNAL(toggled(bool)),this->m_pCellListDockWidget,SLOT(setVisible(bool)));


    m_SelectedCell=-1;



    connect(this->m_pUI->nextButton,SIGNAL(clicked()),SLOT(slotNextFrame()));
    connect(this->m_pUI->previousButton,SIGNAL(clicked()),SLOT(slotPreviousFrame()));

    connect(this->m_pUI->actionValidateTracking,SIGNAL(triggered()),SLOT(slotDoValidateTracking()));
}


void DrosophilaOmmatidiaExplorer::slotVertexTableSelectionChanged( OmmatidiaTissue<3>::AJGraphType::AJVertexHandler  selectedVertex){
    this->SetSelectedVertex(selectedVertex);
    this->m_Renderer->GetRenderWindow()->Render();
}


void DrosophilaOmmatidiaExplorer::slotEdgeTableSelectionChanged( OmmatidiaTissue<3>::AJGraphType::AJEdgeHandler selectedEdge){
    this->SetSelectedEdge(selectedEdge);
    this->m_Renderer->GetRenderWindow()->Render();
}

void DrosophilaOmmatidiaExplorer::slotVertexAdditionToggled(bool toggled){

    if(toggled){
        this->m_pUI->actionAddEdge->setEnabled(false);
        this->m_pUI->actionSelectEdge->setEnabled(false);
        this->m_pUI->actionSelectVertex->setEnabled(false);
        this->m_pUI->actionDetectVertices->setEnabled(false);
        this->m_pUI->actionTrackVertices->setEnabled(false);

        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickVertexAddMode(vtkObject*,ulong,void*,void*,vtkCommand*)));
    }else{
        this->m_pUI->actionAddEdge->setEnabled(true);
        this->m_pUI->actionSelectEdge->setEnabled(true);
        this->m_pUI->actionSelectVertex->setEnabled(true);
        this->m_pUI->actionDetectVertices->setEnabled(true);
        this->m_pUI->actionTrackVertices->setEnabled(true);
        this->m_QtToVTKConnections->Disconnect();
    }

}
void DrosophilaOmmatidiaExplorer::slotRightClickVertexAddMode(vtkObject*,ulong,void*,void*,vtkCommand*){
	this->m_pUI->actionAddVertices->toggle();
}
void DrosophilaOmmatidiaExplorer::slotLeftClickVertexAddMode(vtkObject*,ulong,void*,void*,vtkCommand*){

    int* clickPos = this->m_RenderWindowInteractor->GetEventPosition();
    // Pick from this location.
    vtkSmartPointer<vtkWorldPointPicker> picker = vtkSmartPointer<vtkWorldPointPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, m_Renderer);


    double picked[3];
    picker->GetPickPosition(picked);

     DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::Pointer newVertex = DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::New();

     DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::PointType point;
    point[0]=picked[0];
    point[1]=picked[1];
    point[2]=picked[2];
    newVertex->SetPosition(point);

     DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler newVertexHandler= this->m_CurrentAJGraph->AddAJVertex(newVertex);

    vtkSmartPointer<vtkActor> drawn=this->m_VertexLocationsDrawer.DrawAJVertex(newVertexHandler);
    drawn->SetVisibility(true);

    this->m_Renderer->GetRenderWindow()->Render();
    this->m_Project.SetAJGraph(this->m_CurrentFrame,this->m_CurrentAJGraph);


}
void DrosophilaOmmatidiaExplorer::slotLeftClickCellSelectionMode(vtkObject*, unsigned long, void*,void*, vtkCommand*){
    int* clickPos = this->m_RenderWindowInteractor->GetEventPosition();
    // Pick from this location.
    vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, m_Renderer);

    vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

    if(pickedActor){
        auto clickedCell = this->m_CellDrawer.GetCellFromActor(pickedActor);
        std::cout << "Clicked" <<  clickedCell << std::endl;
        this->SetSelectedCell(clickedCell);
    }
}

void DrosophilaOmmatidiaExplorer::slotRightClickCellSelectionMode(vtkObject*, unsigned long, void*,void*, vtkCommand*){

}
void DrosophilaOmmatidiaExplorer::slotVertexMoveToggled(bool toggled){

    if(toggled){

         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::Pointer vertex = this->m_CurrentAJGraph->GetAJVertex(this->m_SelectedVertex);
         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::PointType position= vertex->GetPosition();

        m_PointWidget->SetPosition(position[0],position[1],position[2]);

        vtkSmartPointer<vtkMoveVertexCallback> callback = vtkSmartPointer<vtkMoveVertexCallback>::New();


        callback->m_AJVertex=vertex;
        callback->m_AJVertexHandler=this->m_SelectedVertex;
        callback->m_pDrawer=&this->m_VertexLocationsDrawer;
        this->m_QtToVTKConnections->Disconnect();
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickVertexMoveMode(vtkObject*,ulong,void*,void*,vtkCommand*)));
        this->m_PointWidget->AddObserver(vtkCommand::InteractionEvent,callback);

        m_PointWidget->On();
    }else{
        this->m_Project.SetAJGraph(this->m_CurrentFrame,this->m_CurrentAJGraph);
        m_PointWidget->Off();
        m_PointWidget->RemoveAllObservers();
        this->m_QtToVTKConnections->Disconnect();
        //Restore Selection mode
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickVertexSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickVertexSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));

        this->DrawAJVertices(this->m_CurrentFrame);
    }
}

void DrosophilaOmmatidiaExplorer::slotRightClickVertexMoveMode(vtkObject*,ulong,void*,void*,vtkCommand*){
	this->m_pUI->actionMoveVertex->toggle();
}



void DrosophilaOmmatidiaExplorer::slotCellSelectionToggled(bool toggled){
	if(toggled){
		this->m_CellDrawer.PickOn();
		this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickCellSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
		this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickCellSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
	}else{
		this->m_CellDrawer.PickOff();
		this->m_QtToVTKConnections->Disconnect();
	}

}
void DrosophilaOmmatidiaExplorer::slotVertexSelectionToggled(bool toggled){

    if(toggled){

        this->m_pUI->actionAddEdge->setEnabled(false);
        this->m_pUI->actionAddVertices->setEnabled(false);
        this->m_pUI->actionSelectEdge->setEnabled(false);
        this->m_pUI->actionDetectVertices->setEnabled(false);
        this->m_pUI->actionTrackVertices->setEnabled(false);

        this->m_pUI->actionVertexMolecularDistribution->setEnabled(false);
        this->m_pUI->actionEdgeMolecularDistribution->setEnabled(false);

        this->m_VertexLocationsDrawer.PickOn();
        this->m_pUI->actionAddVertices->setChecked(false);
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickVertexSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickVertexSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        std::cout << "VERTEX SELECTION ON" << std::endl;

    }else{
        this->m_pUI->actionAddEdge->setEnabled(true);
        this->m_pUI->actionAddVertices->setEnabled(true);
        this->m_pUI->actionSelectEdge->setEnabled(true);
        this->m_pUI->actionDetectVertices->setEnabled(true);
        this->m_pUI->actionTrackVertices->setEnabled(true);
        this->m_pUI->actionDeleteVertex->setEnabled(false);
        this->m_pUI->actionMoveVertex->setEnabled(false);

        this->m_pUI->actionVertexMolecularDistribution->setEnabled(true);
        this->m_pUI->actionEdgeMolecularDistribution->setEnabled(true);

        this->m_VertexLocationsDrawer.PickOff();
        this->m_QtToVTKConnections->Disconnect();

        std::cout << "VERTEX SELECTION OFF" << std::endl;
    }

}

void DrosophilaOmmatidiaExplorer::slotEdgeSelectionToggled(bool toggled){

    if(toggled){
        this->m_pUI->actionAddEdge->setEnabled(false);
        this->m_pUI->actionAddVertices->setEnabled(false);
        this->m_pUI->actionSelectVertex->setEnabled(false);
        this->m_pUI->actionDetectVertices->setEnabled(false);
        this->m_pUI->actionTrackVertices->setEnabled(false);
        this->m_pUI->actionVertexMolecularDistribution->setEnabled(false);
        this->m_pUI->actionEdgeMolecularDistribution->setEnabled(false);

        this->m_EdgesDrawer.PickOn();
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickEdgeSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickEdgeSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        std::cout << "EDGE SELECTION ON" << std::endl;
    }else{
        this->m_pUI->actionAddEdge->setEnabled(true);
        this->m_pUI->actionAddVertices->setEnabled(true);
        this->m_pUI->actionSelectVertex->setEnabled(true);
        this->m_pUI->actionDetectVertices->setEnabled(true);
        this->m_pUI->actionTrackVertices->setEnabled(true);
        this->m_pUI->actionVertexMolecularDistribution->setEnabled(true);
        this->m_pUI->actionEdgeMolecularDistribution->setEnabled(true);

        this->m_EdgesDrawer.PickOff();
        this->m_QtToVTKConnections->Disconnect();
        //this->m_QtToVTKConnections->Disconnect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        //this->m_QtToVTKConnections->Disconnect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        std::cout << "EDGE SELECTION OFF" << std::endl;

    }
}

void DrosophilaOmmatidiaExplorer::slotLeftClickVertexSelectionMode(vtkObject*, unsigned long, void*,void*, vtkCommand*){

    int* clickPos = this->m_RenderWindowInteractor->GetEventPosition();
    // Pick from this location.
    vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, m_Renderer);

    vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

    if(pickedActor){
         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler clickedVertex = this->m_VertexLocationsDrawer.GetVertexFromActor(pickedActor);
        std::cout << "Clicked" <<  clickedVertex << std::endl;
        this->SetSelectedVertex(clickedVertex);
    }
}



void DrosophilaOmmatidiaExplorer::slotLeftClickEdgeSelectionMode(vtkObject*, unsigned long, void*,void*, vtkCommand*){

    int* clickPos = this->m_RenderWindowInteractor->GetEventPosition();
    // Pick from this location.
    vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, m_Renderer);

    vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

    if(pickedActor){
         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJEdgeHandler clickedEdge = this->m_EdgesDrawer.GetEdgeFromActor(pickedActor);
        std::cout << "Clicked" <<  clickedEdge << std::endl;
        this->SetSelectedEdge(clickedEdge);
    }

}

void DrosophilaOmmatidiaExplorer::SetSelectedCell(const  DrosophilaOmmatidiaJSONProject::CellGraphType::CellVertexHandler & selectedCell){

	if(this->m_SelectedCell!=-1){
		 this->m_CellDrawer.DeemphasizeCell(this->m_SelectedCell);
	 }


	 this->m_CellDrawer.HighlightCell(selectedCell);

	 this->m_pCellListDockWidget->SetSelectedCell(selectedCell);

	 this->m_SelectedCell=selectedCell;
}

void DrosophilaOmmatidiaExplorer::SetSelectedVertex(const  DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler & selectedVertex){

    if(this->m_SelectedVertex!=-1){
        this->m_VertexLocationsDrawer.DeemphasizeAJVertex(this->m_SelectedVertex);
    }

    this->m_pUI->actionDeleteVertex->setEnabled(true);
    this->m_pUI->actionMoveVertex->setEnabled(true);
    this->m_pUI->actionPlotVertexDistribution->setEnabled(true);

    this->m_VertexLocationsDrawer.HighlightAJVertex(selectedVertex);

    this->m_pVertexListDockWidget->SetSelectedVertex(selectedVertex);

    this->m_SelectedVertex=selectedVertex;
}

void DrosophilaOmmatidiaExplorer::SetSelectedEdge(const  DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJEdgeHandler & selectedEdge){

    if(this->m_IsSelectedEdge){
        this->m_EdgesDrawer.DeemphasizeAJEdge(this->m_SelectedEdge);
    }
    this->m_IsSelectedEdge=true;
    this->m_SelectedEdge=selectedEdge;

    this->m_pUI->actionDeleteEdge->setEnabled(true);
    this->m_pUI->actionPlotEdgeDistribution->setEnabled(true);

    this->m_pUI->actionPlotSelectedEdgeLength->setEnabled(true);

    this->m_EdgesDrawer.HighlightEdge(selectedEdge);

    //this->m_pUI->edgesTableWidget->blockSignals(true);
    //this->m_pUI->edgesTableWidget->selectRow(selectedEdge);
    //this->m_pUI->edgesTableWidget->blockSignals(false);

}


void DrosophilaOmmatidiaExplorer::ClearSelectedVertex(){

    this->m_pUI->actionDeleteVertex->setEnabled(false);
    this->m_pUI->actionMoveVertex->setEnabled(false);
    this->m_pUI->actionPlotVertexDistribution->setEnabled(false);
    this->m_pVertexListDockWidget->ClearSelection();



    this->m_VertexLocationsDrawer.DeemphasizeAJVertex(this->m_SelectedVertex);
    this->m_SelectedVertex=-1;
}

void DrosophilaOmmatidiaExplorer::ClearSelectedEdge(){

    this->m_pUI->actionDeleteEdge->setEnabled(false);
    this->m_pUI->actionPlotEdgeDistribution->setEnabled(false);
    this->m_pUI->actionPlotSelectedEdgeLength->setEnabled(false);

    //this->m_pUI->verticesTableWidget->clearSelection();
    this->m_EdgesDrawer.DeemphasizeAJEdge(this->m_SelectedEdge);
    this->m_IsSelectedEdge=false;
}


void DrosophilaOmmatidiaExplorer::slotRightClickVertexSelectionMode(vtkObject*, unsigned long, void*,void*, vtkCommand*){
    std::cout<< "RightClick" << std::endl;

    if(this->m_SelectedVertex!=-1){
        this->ClearSelectedVertex();
    }
    this->m_pUI->actionSelectVertex->toggle();
}

void DrosophilaOmmatidiaExplorer::slotRightClickEdgeSelectionMode(vtkObject*, unsigned long, void*,void*, vtkCommand*){

    if(this->m_IsSelectedEdge){
        this->ClearSelectedEdge();
    }

    this->m_pUI->actionSelectEdge->toggle();

}

void DrosophilaOmmatidiaExplorer::slotDoVertexLocation(){
	for(int t=0;t<m_Project.GetNumberOfFrames();t++){


    typedef HessianImageToVertexnessImageFilter<DrosophilaOmmatidiaJSONProject::HessianImageType,DrosophilaOmmatidiaJSONProject::VertexnessImageType> HessianImageToVertexnessType;

     HessianImageToVertexnessType::Pointer hessianToVertexness = HessianImageToVertexnessType::New();

    hessianToVertexness->SetInput(m_Project.GetHessianImage(t));

    hessianToVertexness->Update();

     DrosophilaOmmatidiaJSONProject::VertexnessImageType::Pointer vertexnessImage=hessianToVertexness->GetOutput();

    m_Project.SetVertexnessImage(t,vertexnessImage);
	}
#if 0
    DetectVerticesDialog dialog(this);
    dialog.exec();

    if(dialog.result()==dialog.Accepted){
        int referenceFrame=dialog.GetReferenceFrame();
        double threshold = dialog.GetThreshold();



        typedef HessianImageToVertexnessImageFilter<DrosophilaOmmatidiaJSONProject::HessianImageType,DrosophilaOmmatidiaJSONProject::VertexnessImageType> HessianImageToVertexnessType;

         HessianImageToVertexnessType::Pointer hessianToVertexness = HessianImageToVertexnessType::New();

        hessianToVertexness->SetInput(m_Project.GetHessianImage(referenceFrame));

        hessianToVertexness->Update();

         DrosophilaOmmatidiaJSONProject::VertexnessImageType::Pointer vertexnessImage=hessianToVertexness->GetOutput();

        m_Project.SetVertexnessImage(referenceFrame,vertexnessImage);

#if 0
        typedef LocalMaximaGraphInitializer<DrosophilaOmmatidiaJSONProject::VertexnessImageType,DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> LocalMaxGraphInitializer;

        typename LocalMaxGraphInitializer::Pointer localMaxGraphInitializerFilter = LocalMaxGraphInitializer::New();



        typedef HessianGraphInitializer<DrosophilaOmmatidiaJSONProject::HessianImageType,DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> HessianGraphInitializer;
        typename HessianGraphInitializer::Pointer localMaxGraphInitializerFilter = HessianGraphInitializer::New();
        typename DrosophilaOmmatidiaJSONProject::HessianImageType::Pointer hessianImage=this->m_Project.GetHessianImage(referenceFrame);

        //localMaxGraphInitializerFilter->SetThreshold(threshold);
        localMaxGraphInitializerFilter->SetInputImage(hessianImage);
        localMaxGraphInitializerFilter->Compute();

        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer localMax = localMaxGraphInitializerFilter->GetOutputGraph();

        this->m_Project.SetAJGraph(referenceFrame,localMax);

        m_VertexLocationsDrawer.Reset();
        m_VertexLocationsDrawer.SetVertexLocations(localMax);
        m_VertexLocationsDrawer.Draw();
        m_VertexLocationsDrawer.SetVisibility(this->m_pUI->showAJVerticesGroupBox->isChecked());


#endif
    }
#endif
}

void DrosophilaOmmatidiaExplorer::slotDoValidateTracking(){

	AJTrackingFrame frame(this,m_Project);

	frame.exec();
}

void DrosophilaOmmatidiaExplorer::slotDoRefineVertexLocation(){
	typedef RefineVerticesToNearestVertexnessMaxima<DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType,DrosophilaOmmatidiaJSONProject::VertexnessImageType> RefineVerticesToNearestVertexnessMaximaType;

	 RefineVerticesToNearestVertexnessMaximaType::Pointer verticesRefiner = RefineVerticesToNearestVertexnessMaximaType::New();

	 DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer vertices = m_Project.GetAJGraph(m_CurrentFrame);
	 DrosophilaOmmatidiaJSONProject::VertexnessImageType::Pointer vertexness = m_Project.GetVertexnessImage(m_CurrentFrame);

	verticesRefiner->SetVertexnessImage(vertexness);
	verticesRefiner->SetVertices(vertices);
	verticesRefiner->Compute();
	m_Project.SetAJGraph(m_CurrentFrame,verticesRefiner->GetVertices());
}

void DrosophilaOmmatidiaExplorer::slotDeleteVertex(){

    assert(this->m_SelectedVertex!=-1);
    this->m_CurrentAJGraph->DeleteAJVertex(this->m_SelectedVertex);
    this->m_Project.SetAJGraph(this->m_CurrentFrame,this->m_CurrentAJGraph);

    this->m_VertexLocationsDrawer.Draw();
    this->m_VertexLocationsDrawer.SetVisibility(this->m_pUI->showAJVerticesCheckBox->isChecked());
    this->m_VertexLocationsDrawer.PickOn();

    this->m_EdgesDrawer.Draw();
    this->m_EdgesDrawer.SetVisibility(this->m_pUI->showAJVerticesCheckBox->isChecked());
    this->m_EdgesDrawer.PickOff();

    this->m_SelectedVertex=-1;
    this->m_pUI->actionDeleteVertex->setEnabled(false);
    this->m_pUI->actionMoveVertex->setEnabled(false);

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();
}



void DrosophilaOmmatidiaExplorer::slotDeleteEdge(){

    assert(this->m_IsSelectedEdge);

    this->m_CurrentAJGraph->DeleteAJEdge(this->m_SelectedEdge);

    this->m_Project.SetAJGraph(this->m_CurrentFrame,this->m_CurrentAJGraph);

    this->m_VertexLocationsDrawer.Draw();
    this->m_VertexLocationsDrawer.SetVisibility(this->m_pUI->showAJVerticesCheckBox->isChecked());
    this->m_VertexLocationsDrawer.PickOff();

    this->m_EdgesDrawer.Draw();
    this->m_EdgesDrawer.SetVisibility(this->m_pUI->showAJVerticesCheckBox->isChecked());
    this->m_EdgesDrawer.PickOn();

    this->m_IsSelectedEdge=false;
    this->m_pUI->actionDeleteEdge->setEnabled(false);

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();
}



void DrosophilaOmmatidiaExplorer::slotEdgeAdditionToggled(bool toggled){

    if(toggled){


        this->m_pUI->actionAddVertices->setEnabled(false);
        this->m_pUI->actionSelectEdge->setEnabled(false);
        this->m_pUI->actionSelectVertex->setEnabled(false);

        this->m_VertexLocationsDrawer.PickOn();
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotFirstLeftClickAddEdgeMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickAddEdgeMode(vtkObject*,ulong,void*,void*,vtkCommand*)));
        //this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        std::cout << "SELECTION ON" << std::endl;
    }else{

        this->m_pUI->actionAddVertices->setEnabled(true);
        this->m_pUI->actionSelectEdge->setEnabled(true);
        this->m_pUI->actionSelectVertex->setEnabled(true);

        this->m_QtToVTKConnections->Disconnect();
        this->m_VertexLocationsDrawer.PickOff();
        if(this->m_pTemporaryLineStruct){
            this->m_Renderer->RemoveActor(this->m_pTemporaryLineStruct->lineActor);
            this->m_Renderer->GetRenderWindow()->Render();
            delete m_pTemporaryLineStruct;
            this->m_pTemporaryLineStruct=NULL;
        }


    }

}

void DrosophilaOmmatidiaExplorer::slotRightClickAddEdgeMode(vtkObject *, unsigned long, void *, void *, vtkCommand *){
    this->m_pUI->actionAddEdge->toggle();
}

void DrosophilaOmmatidiaExplorer::slotFirstLeftClickAddEdgeMode(vtkObject*,unsigned long, void*,void*,vtkCommand* command){

    int* clickPos = this->m_RenderWindowInteractor->GetEventPosition();
    // Pick from this location.
    vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, m_Renderer);

    vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

    if(pickedActor){
         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler clickedVertex = this->m_VertexLocationsDrawer.GetVertexFromActor(pickedActor);
        std::cout << "Clicked" <<  clickedVertex << std::endl;

        m_AddingEdgeSource= clickedVertex;

         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::Pointer sourceVertex=this->m_CurrentAJGraph->GetAJVertex(clickedVertex);

        this->m_pTemporaryLineStruct = new TemporaryRenderedLineStruct();
        this->m_pTemporaryLineStruct->lineSource = vtkSmartPointer<vtkLineSource>::New();
        this->m_pTemporaryLineStruct->lineSource->SetPoint1(sourceVertex->GetPosition()[0],sourceVertex->GetPosition()[1],sourceVertex->GetPosition()[2]);
        this->m_pTemporaryLineStruct->lineSource->SetPoint2(sourceVertex->GetPosition()[0],sourceVertex->GetPosition()[1],sourceVertex->GetPosition()[2]);
        this->m_pTemporaryLineStruct->lineSource->Update();


        this->m_pTemporaryLineStruct->lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        this->m_pTemporaryLineStruct->lineMapper->SetInputData(this->m_pTemporaryLineStruct->lineSource->GetOutput());


        this->m_pTemporaryLineStruct->lineActor=vtkSmartPointer<vtkActor>::New();
        this->m_pTemporaryLineStruct->lineActor->SetMapper(this->m_pTemporaryLineStruct->lineMapper);
        this->m_pTemporaryLineStruct->lineActor->GetProperty()->SetColor(1.0,1.0,0.0);
        this->m_pTemporaryLineStruct->lineActor->VisibilityOn();
        this->m_pTemporaryLineStruct->lineActor->PickableOff();

        this->m_Renderer->AddActor(this->m_pTemporaryLineStruct->lineActor);
        this->m_Renderer->GetRenderWindow()->Render();


        //this->SetSelectedVertex(clickedVertex);
        this->m_QtToVTKConnections->Disconnect();
        //this->m_QtToVTKConnections->Disconnect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotFirstLeftClickAddEdgeMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotSecondLeftClickAddEdgeMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::MouseMoveEvent,this,SLOT(slotMouseMoveAddEdgeMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickAddEdgeMode(vtkObject*,ulong,void*,void*,vtkCommand*)));
        //command->AbortFlagOn();
        std::cout << "First click commited" << std::endl;
    }
}

void DrosophilaOmmatidiaExplorer::slotMouseMoveAddEdgeMode(vtkObject*,unsigned long, void* client_data,void*,vtkCommand*){



    int* clickPos = this->m_RenderWindowInteractor->GetEventPosition();
    // Pick from this location.
    vtkSmartPointer<vtkWorldPointPicker>  picker =  	vtkSmartPointer<vtkWorldPointPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, m_Renderer);

    double picked[3];
    picker->GetPickPosition(picked);

    this->m_pTemporaryLineStruct->lineSource->SetPoint2(picked[0],picked[1],picked[2]);

    this->m_pTemporaryLineStruct->lineSource->Update();
    this->m_Renderer->GetRenderWindow()->Render();
    //Update NEW POSITION

}


void DrosophilaOmmatidiaExplorer::slotSecondLeftClickAddEdgeMode(vtkObject*,unsigned long, void* client_data,void*,vtkCommand*){

    std::cout << "Second click started" << std::endl;

    int* clickPos = this->m_RenderWindowInteractor->GetEventPosition();
    // Pick from this location.
    vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, m_Renderer);

    vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

    if(pickedActor){


        //this->m_Renderer->RemoveActor(this->m_pTemporaryLineStruct->lineActor);
        //renderingStructure->lineActor->Delete();
        //renderingStructure->lineMapper->Delete();
        //renderingStructure->lineSource->Delete();

        //delete this->m_pTemporaryLineStruct;

         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler clickedVertex = this->m_VertexLocationsDrawer.GetVertexFromActor(pickedActor);



        std::cout << "Linking " << this->m_AddingEdgeSource << " TO " << clickedVertex << std::endl;

         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJEdgeHandler newEdge=this->m_CurrentAJGraph->AddAJEdge(m_AddingEdgeSource,clickedVertex);

        this->m_Project.SetAJGraph(this->m_CurrentFrame,this->m_CurrentAJGraph);

        vtkSmartPointer<vtkActor> newActor = this->m_EdgesDrawer.DrawAJEdge(newEdge);
        newActor->SetVisibility(true);


        //this->m_QtToVTKConnections->Disconnect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotFirstLeftClickAddEdgeMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        //this->m_QtToVTKConnections->Disconnect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotMouseMoveAddEdgeMode(vtkObject*,ulong,void*,void*,vtkCommand*)));
        this->m_QtToVTKConnections->Disconnect();

        this->m_pUI->actionAddEdge->toggle();
        std::cout << "Second click commited" << std::endl;
    }
}


void DrosophilaOmmatidiaExplorer::slotDoTrackOne(){
#if 0
	typedef PredictAJGraph<DrosophilaOmmatidiaJSONProject::TissueType,DrosophilaOmmatidiaJSONProject::MotionImageType,DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> PredictAJGraphType;

    PredictAJGraphType::Pointer predict= PredictAJGraphType::New();
    predict->SetMotionImage(this->m_Project.GetMotionImage(this->m_CurrentFrame));
    predict->SetInputGraph(this->m_Project.GetAJGraph(this->m_CurrentFrame));
    predict->Compute();
	     //   m_Project.SetAJGraph(t+1,predict->GetOutputGraph());


  	typedef RefineVerticesToNearestVertexnessMaxima<DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType,DrosophilaOmmatidiaJSONProject::VertexnessImageType> RefineVerticesToNearestVertexnessMaximaType;

  	RefineVerticesToNearestVertexnessMaximaType::Pointer verticesRefiner = RefineVerticesToNearestVertexnessMaximaType::New();


	DrosophilaOmmatidiaJSONProject::VertexnessImageType::Pointer vertexness = m_Project.GetVertexnessImage(this->m_CurrentFrame+1);

	verticesRefiner->SetVertexnessImage(vertexness);
	verticesRefiner->SetVertices(predict->GetOutputGraph());
	verticesRefiner->Compute();
	verticesRefiner->GetVertices();

	m_Project.SetAJGraph(this->m_CurrentFrame+1,verticesRefiner->GetVertices());

	this->slotFrameChanged(this->m_CurrentFrame+1);
#endif
}

void DrosophilaOmmatidiaExplorer::slotDoVertexTracking(){
#if 0
    for(int t=0;t<this->m_Project.GetNumberOfFrames()-1;t++){
        typedef PredictAJGraph<DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType,DrosophilaOmmatidiaJSONProject::MotionImageType,DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> PredictAJGraphType;


        PredictAJGraphType::Pointer predict= PredictAJGraphType::New();
        predict->SetMotionImage(this->m_Project.GetMotionImage(t));
        predict->SetInputGraph(this->m_Project.GetAJGraph(t));
        predict->Compute();

     //   m_Project.SetAJGraph(t+1,predict->GetOutputGraph());


    	typedef RefineVerticesToNearestVertexnessMaxima<DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType,DrosophilaOmmatidiaJSONProject::VertexnessImageType> RefineVerticesToNearestVertexnessMaximaType;

    	RefineVerticesToNearestVertexnessMaximaType::Pointer verticesRefiner = RefineVerticesToNearestVertexnessMaximaType::New();

    	DrosophilaOmmatidiaJSONProject::VertexnessImageType::Pointer vertexness = m_Project.GetVertexnessImage(t+1);

        verticesRefiner->SetVertexnessImage(vertexness);
       	verticesRefiner->SetVertices(predict->GetOutputGraph());
       	verticesRefiner->Compute();

        m_Project.SetAJGraph(t+1,verticesRefiner->GetVertices());


    }
    {
        auto pastGraph=m_Project.GetAJGraph(0);
        auto currentGraph=m_Project.GetAJGraph(1);

        for(auto it= currentGraph->VerticesBegin();it!=currentGraph->VerticesEnd();it++){
            auto vertexHandler = *it;
            auto velocity = (currentGraph->GetAJVertex(vertexHandler)->GetPosition()-pastGraph->GetAJVertex(vertexHandler)->GetPosition());
            pastGraph->GetAJVertex(vertexHandler)->SetVelocity(velocity);
        }

        m_Project.SetAJGraph(0,pastGraph);
    }

    for(int t=1;t<this->m_Project.GetNumberOfFrames()-1;t++){
        auto pastGraph=m_Project.GetAJGraph(t-1);
        auto currentGraph=m_Project.GetAJGraph(t);
        auto nextGraph=m_Project.GetAJGraph(t+1);

        for(auto it= currentGraph->VerticesBegin();it!=currentGraph->VerticesEnd();it++){
            auto vertexHandler = *it;
            auto velocity = (nextGraph->GetAJVertex(vertexHandler)->GetPosition()-pastGraph->GetAJVertex(vertexHandler)->GetPosition())/2;
            currentGraph->GetAJVertex(vertexHandler)->SetVelocity(velocity);
        }

        m_Project.SetAJGraph(t,currentGraph);
    }
    {
        auto currentGraph=m_Project.GetAJGraph(this->m_Project.GetNumberOfFrames()-2);
        auto nextGraph=m_Project.GetAJGraph(this->m_Project.GetNumberOfFrames()-1);
        for(auto it= nextGraph->VerticesBegin();it!=nextGraph->VerticesEnd();it++){
            auto vertexHandler = *it;
            auto velocity = (nextGraph->GetAJVertex(vertexHandler)->GetPosition()-currentGraph->GetAJVertex(vertexHandler)->GetPosition());
            nextGraph->GetAJVertex(vertexHandler)->SetVelocity(velocity);
        }
        m_Project.SetAJGraph(this->m_Project.GetNumberOfFrames()-1,nextGraph);

    }
#endif
}

void DrosophilaOmmatidiaExplorer::slotDoCells(){

#if 0
	typedef AJGraphToTissueDescriptor<typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType,
					   typename DrosophilaOmmatidiaJSONProject::TissueType> CellFilterType;

	typename CellFilterType::Pointer cellFilter = CellFilterType::New();

	cellFilter->SetAJGraph(m_Project.GetAJGraph(m_CurrentFrame));
	cellFilter->Compute();

	//m_Project.SetCellGraph(m_CurrentFrame,cellFilter->GetCellGraph());
#endif

}

void DrosophilaOmmatidiaExplorer::slotShowOriginalModeChanged(const QString & mode){

    if(mode=="Volume"){

        this->m_OriginalDrawer.SetRenderingMode(OriginalDrawerType::VOLUME);

    }else if(mode=="Slice"){

        this->m_OriginalDrawer.SetRenderingMode(OriginalDrawerType::SLICE);
    }
    m_OriginalDrawer.Draw();
    this->m_OriginalDrawer.SetVisibility(this->m_pUI->showOriginalGroupBox->isChecked());

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

void DrosophilaOmmatidiaExplorer::slotShowDeconvolutedModeChanged(const QString & mode){

    if(mode=="Volume"){

        this->m_DeconvolutedDrawer.SetRenderingMode(DeconvolutedDrawerType::VOLUME);
    }else if(mode=="Slice"){

        this->m_DeconvolutedDrawer.SetRenderingMode(DeconvolutedDrawerType::SLICE);
    }
    m_DeconvolutedDrawer.Draw();
    this->m_DeconvolutedDrawer.SetVisibility(this->m_pUI->showDeconvolutedGroupBox->isChecked());
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();
}

void DrosophilaOmmatidiaExplorer::slotShowPlatenessModeChanged(const QString & mode){

    if(mode=="Volume"){

        this->m_PlatenessDrawer.SetRenderingMode(PlatenessDrawerType::VOLUME);
    }else if(mode=="Slice"){

        this->m_PlatenessDrawer.SetRenderingMode(PlatenessDrawerType::SLICE);
    }
    m_PlatenessDrawer.Draw();
    this->m_PlatenessDrawer.SetVisibility(this->m_pUI->showPlatenessGroupBox->isChecked());
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

void DrosophilaOmmatidiaExplorer::slotShowDeconvolutedOpacityChanged(int opacity){

    double value=double(opacity)/1000000.0;

    this->m_DeconvolutedDrawer.SetOpacity(value);

    this->m_DeconvolutedDrawer.Draw();
    this->m_DeconvolutedDrawer.SetVisibility(this->m_pUI->showDeconvolutedGroupBox->isChecked());

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}
void DrosophilaOmmatidiaExplorer::slotShowOriginalOpacityChanged(int opacity){

    double value = double(opacity)/1000000;
    this->m_OriginalDrawer.SetOpacity(value);
    this->m_OriginalDrawer.Draw();
    this->m_OriginalDrawer.SetVisibility(this->m_pUI->showOriginalGroupBox->isChecked());

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

void DrosophilaOmmatidiaExplorer::slotShowOriginalSliceChanged(int value){

    m_OriginalDrawer.SetSlice(value);
    this->m_OriginalDrawer.Draw();
    this->m_OriginalDrawer.SetVisibility(this->m_pUI->showOriginalGroupBox->isChecked());

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

void DrosophilaOmmatidiaExplorer::slotShowDeconvolutedSliceChanged(int value){

    m_DeconvolutedDrawer.SetSlice(value);
    this->m_DeconvolutedDrawer.Draw();
    this->m_DeconvolutedDrawer.SetVisibility(this->m_pUI->showDeconvolutedGroupBox->isChecked());

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

void DrosophilaOmmatidiaExplorer::slotShowPlatenessOpacityChanged(int opacity){

    double value = double(opacity)/1000000.0;
    this->m_PlatenessDrawer.SetOpacity(value);
    this->m_PlatenessDrawer.Draw();
    this->m_PlatenessDrawer.SetVisibility(this->m_pUI->showPlatenessGroupBox->isChecked());

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();


}

void DrosophilaOmmatidiaExplorer::slotShowVertexLocationsChanged(bool value){

    m_VertexLocationsDrawer.SetVisibility(value);
    m_EdgesDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

void DrosophilaOmmatidiaExplorer::slotShowCellsChanged(bool value){

	m_CellDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

void DrosophilaOmmatidiaExplorer::slotShowDeconvolutedChanged(bool value){

    m_DeconvolutedDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

void DrosophilaOmmatidiaExplorer::slotShowOriginalChanged(bool value){

    m_OriginalDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

void DrosophilaOmmatidiaExplorer::slotShowMolecularChanged(bool value){

    this->m_MolecularImageDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();
}

void DrosophilaOmmatidiaExplorer::slotShowMolecularOpacityChanged(int opacity){
    double value = double(opacity)/1000000.0;
    this->m_MolecularImageDrawer.SetOpacity(value);
    this->m_MolecularImageDrawer.Draw();
    this->m_MolecularImageDrawer.SetVisibility(this->m_pUI->showMolecularChannelGroupBox->isChecked());

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}


void DrosophilaOmmatidiaExplorer::slotShowMotionFieldChanged(bool value){

    m_MotionVolumeDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

void DrosophilaOmmatidiaExplorer::slotShowVertexMotionChanged(bool value){

    m_VertexMotionsDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}


void DrosophilaOmmatidiaExplorer::slotShowPlatenessChanged(bool value){

    m_PlatenessDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}


void DrosophilaOmmatidiaExplorer::slotShowPlatenessSliceChanged(int value){

    m_PlatenessDrawer.SetSlice(value);
    this->m_PlatenessDrawer.Draw();
    this->m_PlatenessDrawer.SetVisibility(this->m_pUI->showPlatenessGroupBox->isChecked());

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}
void DrosophilaOmmatidiaExplorer::slotShowVertexnessChanged(bool value){

    m_VertexnessDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}
void DrosophilaOmmatidiaExplorer::slotShowVertexnessOpacityChanged(int opacity){
    double value = double(opacity)/1000000.0;
    this->m_VertexnessDrawer.SetOpacity(value);
    this->m_VertexnessDrawer.Draw();
    this->m_VertexnessDrawer.SetVisibility(this->m_pUI->showVertexnessGroupBox->isChecked());

    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}


DrosophilaOmmatidiaExplorer::~DrosophilaOmmatidiaExplorer()
{


}


void DrosophilaOmmatidiaExplorer::slotFrameChanged(int frame){

    QString baseText("t=%1");
    this->m_pUI->currentFrameLabel->setText(baseText.arg(frame));

    this->m_CurrentFrame=frame;

    if(this->m_CurrentFrame==m_Project.GetNumberOfFrames()-1){
        this->m_pUI->showMotionFieldGroupBox->setEnabled(false);
    }else{\
        this->m_pUI->showMotionFieldGroupBox->setEnabled(true);
    }


	this->m_pUI->frameSlider->blockSignals(true);
	this->m_pUI->frameSlider->setValue(this->m_CurrentFrame);
    this->m_pUI->frameSlider->blockSignals(false);

    this->DrawFrame(this->m_CurrentFrame);

}

void DrosophilaOmmatidiaExplorer::slotNextFrame(){
	this->m_CurrentFrame++;
	this->slotFrameChanged(this->m_CurrentFrame);

}

void DrosophilaOmmatidiaExplorer::slotPreviousFrame(){
	this->m_CurrentFrame--;
	this->slotFrameChanged(this->m_CurrentFrame);
}


void DrosophilaOmmatidiaExplorer::DrawCells(int frame){
    if(this->m_Project.IsTissueDescriptor(frame)){
           	this->m_CurrentTissue = m_Project.GetTissueDescriptor(frame);
           	this->m_CellDrawer.SetAJVertices(m_CurrentTissue->GetAJGraph());
           	this->m_CellDrawer.SetCells(m_CurrentTissue->GetCellGraph());
           	this->m_CellDrawer.Draw();
           	this->m_CellDrawer.SetVisibility(this->m_pUI->showCellsCheckBox->isChecked());
           	this->m_pTissueDockWidget->SetTissue(m_CurrentTissue);
           	this->m_pTissueDockWidget->Init();
           	//this->m_pCellListDockWidget->SetCells(m_CurrentTissue->GetCellGraph());
           	//this->m_pCellListDockWidget->Draw();
    }else{
    	this->m_CellDrawer.Reset();
    }

}

void DrosophilaOmmatidiaExplorer::DrawAJVertices(int frame){

    if(this->m_Project.IsAJVertices(frame)){

        this->m_CurrentAJGraph=this->m_Project.GetAJGraph(frame);

        this->m_VertexLocationsDrawer.SetVertexLocations(this->m_CurrentAJGraph);
        this->m_VertexLocationsDrawer.Draw();
        this->m_VertexLocationsDrawer.SetVisibility(this->m_pUI->showAJVerticesCheckBox->isChecked());

        this->m_pVertexListDockWidget->SetVertexContainer(this->m_CurrentAJGraph);
        this->m_pVertexListDockWidget->Draw();


        this->m_EdgesDrawer.SetEdgesContainer(this->m_CurrentAJGraph);
        this->m_EdgesDrawer.Draw();
        this->m_EdgesDrawer.SetVisibility(this->m_pUI->showAJVerticesCheckBox->isChecked());

        this->m_pEdgeListDockWidget->SetEdgesContainer(this->m_CurrentAJGraph);
        this->m_pEdgeListDockWidget->Draw();

        m_VertexMotionsDrawer.SetVertexLocations(m_CurrentAJGraph);
        m_VertexMotionsDrawer.Draw();
        m_VertexMotionsDrawer.SetVisibility(this->m_pUI->showVertexMotionCBox->isChecked());

    }else{
        this->m_VertexLocationsDrawer.Reset();
        this->m_EdgesDrawer.Reset();
        this->m_VertexMotionsDrawer.Reset();
    }
}
void DrosophilaOmmatidiaExplorer::DrawFrame(int frame ){
    this->m_pUI->frameSlider->blockSignals(true);
    this->m_pUI->frameSlider->setValue(frame);
    this->m_pUI->frameSlider->blockSignals(false);

    this->m_Renderer->RemoveAllViewProps();
#if 1
    m_OriginalDrawer.SetImage(m_Project.GetOriginalImage(frame));
    m_OriginalDrawer.SetOpacity(double(this->m_pUI->showOriginalOpacitySlider->value())/1000000.0);
    m_OriginalDrawer.Draw();
    m_OriginalDrawer.SetVisibility(this->m_pUI->showOriginalGroupBox->isChecked());
#endif


    m_DeconvolutedDrawer.SetImage(m_Project.GetDeconvolutedImage(frame));
    m_DeconvolutedDrawer.SetOpacity(double(this->m_pUI->showDeconvolutedOpacitySlider->value())/1000000.0);
    m_DeconvolutedDrawer.Draw();
    m_DeconvolutedDrawer.SetVisibility(this->m_pUI->showDeconvolutedGroupBox->isChecked());


    m_PlatenessDrawer.SetImage(m_Project.GetPlatenessImage(frame));
    m_PlatenessDrawer.SetOpacity(double(this->m_pUI->showPlatenessOpacitySlider->value())/1000000.0);
    m_PlatenessDrawer.Draw();
    m_PlatenessDrawer.SetVisibility(this->m_pUI->showPlatenessGroupBox->isChecked());

    if(this->m_Project.IsVertexnessImage(frame)){
    m_VertexnessDrawer.SetImage(m_Project.GetVertexnessImage(frame));
    m_VertexnessDrawer.SetOpacity(double(this->m_pUI->showVertexnessOpacitySlider->value())/1000000.0);
    //m_VertexnessDrawer.SetOpacity(1.0);
    m_VertexnessDrawer.Draw();
    m_VertexnessDrawer.SetVisibility(this->m_pUI->showVertexnessGroupBox->isChecked());

    }else{
    	m_VertexnessDrawer.Reset();
    }

    if(frame!=this->m_Project.GetNumberOfFrames()-1){
         DrosophilaOmmatidiaJSONProject::MotionImageType::Pointer motionImage = m_Project.GetMotionImage(frame);
        m_MotionVolumeDrawer.SetMotionImage(motionImage);
        m_MotionVolumeDrawer.Draw();
        m_MotionVolumeDrawer.SetVisibility(this->m_pUI->showMotionFieldGroupBox->isChecked());

    }else{
        m_MotionVolumeDrawer.Reset();
    }



    if(this->m_Project.IsMolecularImage(frame)){

        m_MolecularImageDrawer.SetImage(m_Project.GetMolecularImage(frame));
        m_MolecularImageDrawer.SetOpacity(double(this->m_pUI->showMolecularChannelOpacitySlider->value())/1000000.0);
        m_MolecularImageDrawer.Draw();
        m_MolecularImageDrawer.SetVisibility(this->m_pUI->showMolecularChannelGroupBox->isChecked());

    }else{
        this->m_MolecularImageDrawer.Reset();
    }

    this->DrawAJVertices(frame);
    this->DrawCells(frame);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();


}




// Action to be taken upon file open
void DrosophilaOmmatidiaExplorer::slotOpenProject(){

    QFileDialog fileDialog(this);

    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setOption(QFileDialog::ShowDirsOnly);

    if(fileDialog.exec()){
        this->m_ProjectPath=fileDialog.selectedFiles().first();

        std::cout << this->m_ProjectPath.toStdString() << std::endl;

        if(this->m_Project.Open(this->m_ProjectPath.toStdString())){
            this->m_CurrentFrame=0;
            this->DrawFrame(0);
            this->m_pUI->actionOpenFile->setEnabled(false);
            this->m_pUI->visualizationDock->setEnabled(true);

            this->m_pUI->frameSlider->blockSignals(true);
            this->m_pUI->frameSlider->setEnabled(true);
            this->m_pUI->frameSlider->setMinimum(0);
            this->m_pUI->frameSlider->setMaximum(this->m_Project.GetNumberOfFrames()-1);
            this->m_pUI->frameSlider->setValue(0);
            this->m_pUI->frameSlider->setTickInterval(1);
            this->m_pUI->frameSlider->blockSignals(false);

            this->m_pUI->actionDetectVertices->setEnabled(true);

            this->m_pUI->actionSelectVertex->setEnabled(true);
            this->m_pUI->actionAddVertices->setEnabled(true);

            this->m_pUI->actionSelectEdge->setEnabled(true);
            this->m_pUI->actionAddEdge->setEnabled(true);

            this->m_pUI->actionTrackVertices->setEnabled(true);
            this->m_pUI->actionVertexMolecularDistribution->setEnabled(true);
            this->m_pUI->actionEdgeMolecularDistribution->setEnabled(true);
            this->m_pUI->statusbar->showMessage(tr("Ready"));


            this->m_pGraphPlotterDockWidget->SetLength(m_Project.GetNumberOfFrames());
        }else{


            std::cout << "Error: " << std::endl;
            QMessageBox errorMessage(this);
            errorMessage.setText("Unable to Open Project");
            errorMessage.setInformativeText("Please check the selected directory contains a valid project and try again");
            errorMessage.setIcon(QMessageBox::Critical);
            errorMessage.exec();

        }
        //C

        //
    }
}

void DrosophilaOmmatidiaExplorer::slotDoVertexMolecularDistribution(){
    for(unsigned int t=0;t<m_Project.GetNumberOfFrames();t++){

         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer ajgraph= m_Project.GetAJGraph(t);

        typedef VertexMolecularDistributionDescriptor<typename  DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType,typename DrosophilaOmmatidiaJSONProject::MolecularImageType> VertexMolecularDistributionDescriptorType;

         VertexMolecularDistributionDescriptorType::Pointer vertexDescriptor = VertexMolecularDistributionDescriptorType::New();


         DrosophilaOmmatidiaJSONProject::MolecularImageType::Pointer molecular =m_Project.GetMolecularImage(t);
        vertexDescriptor->SetMolecularImage(molecular);
        vertexDescriptor->SetRadius(10*molecular->GetSpacing()[0]); //TODO
        vertexDescriptor->SetAJGraph(ajgraph);
        vertexDescriptor->Compute();

        m_Project.SetAJGraph(t,vertexDescriptor->GetAJGraph());

    }
}


void DrosophilaOmmatidiaExplorer::slotDoEdgeMolecularDistribution(){
    for(unsigned int t=0;t<m_Project.GetNumberOfFrames();t++){
         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer ajgraph= m_Project.GetAJGraph(t);

        typedef EdgeMolecularDistributionDescriptor<typename  DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType,typename DrosophilaOmmatidiaJSONProject::MolecularImageType> EdgeMolecularDistributionDescriptorType;

         EdgeMolecularDistributionDescriptorType::Pointer edgeDescriptor = EdgeMolecularDistributionDescriptorType::New();

         DrosophilaOmmatidiaJSONProject::MolecularImageType::Pointer molecularImage=m_Project.GetMolecularImage(t);
        edgeDescriptor->SetMolecularImage(molecularImage);
        edgeDescriptor->SetRadius(2.0*molecularImage->GetSpacing()[0]); //TODO
        edgeDescriptor->SetNumberOfSegments(4);

        edgeDescriptor->SetAJGraph(ajgraph);
        edgeDescriptor->Compute();


        m_Project.SetAJGraph(t,edgeDescriptor->GetAJGraph());
    }
}
void DrosophilaOmmatidiaExplorer::slotPlotSelectedCellArea(){
	return slotPlotCellArea(this->m_SelectedCell);
}
void DrosophilaOmmatidiaExplorer::slotPlotSelectedEdgeLength(){
	return slotPlotEdgeLength(this->m_SelectedEdge);
}
#if 0
void DrosophilaOmmatidiaExplorer::slotPlotCellArea(const DrosophilaOmmatidiaExplorer::CellGraphType::CellVertexHandler & cell){

    vtkSmartPointer<vtkDoubleArray> arrCell =vtkSmartPointer<vtkDoubleArray>::New();
    std::string arrayName("cellArea-" + std::to_string(cell));
    arrCell->SetName(arrayName.c_str());
    arrCell->SetNumberOfTuples(this->m_Project.GetNumberOfFrames());

    for(int t=0;t<this->m_Project.GetNumberOfFrames();t++){
        auto tissue = m_Project.GetTissueDescriptor(t);

        arrCell->SetTuple1(t,tissue->GetCellGraph()->GetCell(cell)->GetArea());

    }

    double sum=0;
    double sum2=0;
    for(int t=0;t< arrCell->GetNumberOfTuples();t++){
    	sum+=arrCell->GetTuple1(t);
    	sum2+=arrCell->GetTuple1(t)*arrCell->GetTuple1(t);
    }
    double mean = sum / arrCell->GetNumberOfTuples();

    double std = sum2/arrCell->GetNumberOfTuples() - mean*mean;
    std= sqrt(std);
    for(int t=0;t< arrCell->GetNumberOfTuples();t++){
    	arrCell->SetTuple1(t,(arrCell->GetTuple1(t)-mean)/std);
    }

    //auto color = this->m_EdgesDrawer.GetEdgeColor(edge);
    itk::FixedArray<double,3> color;
    color[0]=1.0;
    color[1]=1.0;
    color[2]=1.0;
    std::cout << color << std::endl;

    this->m_pGraphPlotterDockWidget->AddPlot(arrCell,color,false);
    this->m_pGraphPlotterDockWidget->Draw();

}
#endif
template<class TTissueDescriptor,class TAJSubgraph>  typename TTissueDescriptor::CellGraphType::CellVertexHandler vertexSubsetToCellHandler(const typename TTissueDescriptor::Pointer & tissue, const TAJSubgraph & subgraph){

	auto result = std::find_if(tissue->GetCellGraph()->CellsBegin(),tissue->GetCellGraph()->CellsEnd(),
							  [&tissue,&subgraph](const typename TTissueDescriptor::CellGraphType::CellVertexHandler & cellVertexHandler){
									auto cell = tissue->GetCellGraph()->GetCell(cellVertexHandler);
									std::vector<typename TTissueDescriptor::AJGraphType::AJVertexHandler> missing;

									for(auto perimeterVertexIt =cell->BeginPerimeterAJVertices();perimeterVertexIt!=cell->BeginPerimeterAJVertices();++perimeterVertexIt ){
										if(std::find(subgraph.BeginVertices(),subgraph.EndVertices(),*perimeterVertexIt)==subgraph.EndVertices()){
											return false;
										}
									}

									for(auto perimeterVertexIt =subgraph.BeginVertices();perimeterVertexIt!=subgraph.EndVertices();++perimeterVertexIt ){
										if(std::find(cell->BeginPerimeterAJVertices(),cell->EndPerimeterAJVertices(),*perimeterVertexIt)==cell->EndPerimeterAJVertices()){
											return false;
										}
									}
									return true;
							}
					);
	assert(result!=tissue->GetCellGraph()->CellsEnd());
	return *result;
}
void DrosophilaOmmatidiaExplorer::slotPlotCellArea(const OmmatidiaTissue<3>::CellGraphType::CellVertexHandler & cellHandler){

    vtkSmartPointer<vtkDoubleArray> arrCell =vtkSmartPointer<vtkDoubleArray>::New();
    std::string arrayName("cellArea-" + std::to_string(cellHandler));
    arrCell->SetName(arrayName.c_str());
    arrCell->SetNumberOfTuples(this->m_Project.GetNumberOfFrames());
    arrCell->FillComponent(0,0.0);

    DrosophilaOmmatidiaJSONProject::AJCorrespondenceType::AJSubgraphType cellSubgraph;

    auto cell = m_CurrentTissue->GetCellGraph()->GetCell(cellHandler);

    for(auto it= cell->BeginPerimeterAJVertices();it!=cell->EndPerimeterAJVertices();it++){
    	cellSubgraph.AddVertex(*it);
    }

    arrCell->SetTuple1(m_CurrentFrame,cell->GetArea());


    for(int t=m_CurrentFrame+1;t<this->m_Project.GetNumberOfFrames();t++){
        auto tissue = m_Project.GetTissueDescriptor(t);
        auto correspondences = m_Project.GetCorrespondences(t-1,t);

        auto resultSet = correspondences.FindByAntecessor(cellSubgraph);
        if(resultSet.first!=resultSet.second){
			auto correspondence = resultSet.first;

			cellSubgraph = correspondence->GetSuccessor();
			auto cellHandler =vertexSubsetToCellHandler<OmmatidiaTissue<3>,decltype(cellSubgraph)>(tissue,cellSubgraph);
			auto cell = tissue->GetCellGraph()->GetCell(cellHandler);
			arrCell->SetTuple1(t,cell->GetArea());
			std::cout << "A: " << cell->GetArea() << std::endl;
        }else{
        	break;
        }

    }

    for(int t=m_CurrentFrame;t>0;t--){
    	auto tissue = m_Project.GetTissueDescriptor(t-1);
        auto correspondences = m_Project.GetCorrespondences(t-1,t);

        auto resultSet = correspondences.FindBySuccessor(cellSubgraph);

        if(resultSet.first!=resultSet.second){
        	auto correspondence=resultSet.first;

			cellSubgraph = correspondence->GetAntecessor();
			auto cellHandler =vertexSubsetToCellHandler<OmmatidiaTissue<3>,decltype(cellSubgraph)>(tissue,cellSubgraph);
			auto cell = tissue->GetCellGraph()->GetCell(cellHandler);
			arrCell->SetTuple1(t,cell->GetArea());

        }else{
        	break;
        }
    }

    double sum=0;
    double sum2=0;
    for(int t=0;t< arrCell->GetNumberOfTuples();t++){
    	sum+=arrCell->GetTuple1(t);
    	sum2+=arrCell->GetTuple1(t)*arrCell->GetTuple1(t);
    }
    double mean = sum / arrCell->GetNumberOfTuples();


    double std = sum2/arrCell->GetNumberOfTuples() - mean*mean;
    std= sqrt(std);
    for(int t=0;t< arrCell->GetNumberOfTuples();t++){
    	arrCell->SetTuple1(t,(arrCell->GetTuple1(t)-mean)/std);
    }

    //auto color = this->m_EdgesDrawer.GetEdgeColor(edge);
    itk::FixedArray<double,3> color;
    color[0]=0.0;
    color[1]=1.0;
    color[2]=1.0;
    std::cout << color << std::endl;

    this->m_pGraphPlotterDockWidget->AddPlot(arrCell,color,false);
    this->m_pGraphPlotterDockWidget->Draw();
}

void DrosophilaOmmatidiaExplorer::slotPlotEdgeLength(const OmmatidiaTissue<3>::AJGraphType::AJEdgeHandler & edge){

    vtkSmartPointer<vtkDoubleArray> arrEdge =vtkSmartPointer<vtkDoubleArray>::New();
    std::string arrayName("edgeLength-" + std::to_string(this->m_CurrentAJGraph->GetAJEdgeSource(edge)) + "-" + std::to_string(this->m_CurrentAJGraph->GetAJEdgeTarget(edge)));
    arrEdge->SetName(arrayName.c_str());
    arrEdge->SetNumberOfTuples(this->m_Project.GetNumberOfFrames());

    arrEdge->FillComponent(0,0.0);
    auto sourceHandler=m_CurrentTissue->GetAJGraph()->GetAJEdgeSource(edge);
    auto targetHandler=m_CurrentTissue->GetAJGraph()->GetAJEdgeTarget(edge);

    DrosophilaOmmatidiaJSONProject::AJCorrespondenceType::AJSubgraphType edgeSubgraph;



    edgeSubgraph.AddEdge(edge);
    edgeSubgraph.AddVertex(sourceHandler);
    edgeSubgraph.AddVertex(targetHandler);


    auto sourceLocation = m_CurrentTissue->GetAJGraph()->GetAJVertex(sourceHandler)->GetPosition();
    auto targetLocation = m_CurrentTissue->GetAJGraph()->GetAJVertex(targetHandler)->GetPosition();
    auto diff =  sourceLocation - targetLocation;

    double dist = diff.GetNorm();

    arrEdge->SetTuple1(m_CurrentFrame,dist);

    auto currentSubgraph=edgeSubgraph;
    for(int t=m_CurrentFrame+1;t<this->m_Project.GetNumberOfFrames();t++){
        auto tissue = m_Project.GetTissueDescriptor(t);
        auto correspondences = m_Project.GetCorrespondences(t-1,t);

        auto resultSet = correspondences.FindByAntecessor(currentSubgraph);
        if(resultSet.first!=resultSet.second){
			auto correspondence = resultSet.first;

			currentSubgraph = correspondence->GetSuccessor();

			auto vertexIt=currentSubgraph.BeginVertices();
			auto source = *vertexIt;
			++vertexIt;
			auto target = *vertexIt;

			auto sourceLocation = tissue->GetAJGraph()->GetAJVertex(source)->GetPosition();
			auto targetLocation = tissue->GetAJGraph()->GetAJVertex(target)->GetPosition();
			auto diff =  sourceLocation - targetLocation;

			double dist = diff.GetNorm();
			arrEdge->SetTuple1(t,dist);
        }else{
        	break;
        }

    }
    currentSubgraph=edgeSubgraph;

    for(int t=m_CurrentFrame;t>0;t--){
    	auto tissue = m_Project.GetTissueDescriptor(t-1);
        auto correspondences = m_Project.GetCorrespondences(t-1,t);

        auto resultSet = correspondences.FindBySuccessor(currentSubgraph);

        if(resultSet.first!=resultSet.second){
			auto correspondence = resultSet.first;


			currentSubgraph = correspondence->GetAntecessor();


			auto vertexIt=edgeSubgraph.BeginVertices();
			auto source = *vertexIt;
			++vertexIt;
			auto target = *vertexIt;

			auto sourceLocation = tissue->GetAJGraph()->GetAJVertex(source)->GetPosition();
			auto targetLocation = tissue->GetAJGraph()->GetAJVertex(target)->GetPosition();
			auto diff =  sourceLocation - targetLocation;

			double dist = diff.GetNorm();
			arrEdge->SetTuple1(t,dist);
        }else{
        	break;
        }
    }

    double sum=0;
    double sum2=0;
    for(int t=0;t< arrEdge->GetNumberOfTuples();t++){
    	sum+=arrEdge->GetTuple1(t);
    	sum2+=arrEdge->GetTuple1(t)*arrEdge->GetTuple1(t);
    }
    double mean = sum / arrEdge->GetNumberOfTuples();

    double std = sum2/arrEdge->GetNumberOfTuples() - mean*mean;
    std= sqrt(std);
    for(int t=0;t< arrEdge->GetNumberOfTuples();t++){
    	arrEdge->SetTuple1(t,(arrEdge->GetTuple1(t)-mean)/std);
    }

    auto color = this->m_EdgesDrawer.GetEdgeColor(edge);
    std::cout << color << std::endl;

    this->m_pGraphPlotterDockWidget->AddPlot(arrEdge,color,false);
    this->m_pGraphPlotterDockWidget->Draw();

}

#if 0
void DrosophilaOmmatidiaExplorer::slotShowEdges(bool state){

    if(state){
        m_pEdgeListDockWidget->setHidden(false);
        this->addDockWidget(Qt::RightDockWidgetArea,this->m_pEdgeListDockWidget);
    }


}
#endif

void DrosophilaOmmatidiaExplorer::PlotDescriptor(const std::vector<itk::Array<double> > & descriptors,itk::FixedArray<double,3> color,const std::string & name){

	int T = descriptors.size();
    int nDescriptors = descriptors[0].size();


    std::vector< vtkSmartPointer<vtkDoubleArray> > arrays(nDescriptors);
    vtkSmartPointer<vtkTable> datasetTable = vtkSmartPointer<vtkTable>::New();

    vtkSmartPointer<vtkPCAStatistics> pcaStatistics = vtkSmartPointer<vtkPCAStatistics>::New();
    pcaStatistics->SetInputData(vtkStatisticsAlgorithm::INPUT_DATA,datasetTable);


    for(int n=0;n<nDescriptors;n++){
        arrays[n]=vtkSmartPointer<vtkDoubleArray>::New();
        std::string tmpName = name + std::to_string(n);
        arrays[n]->SetName(tmpName.c_str());
        arrays[n]->SetNumberOfTuples(T);
        datasetTable->AddColumn(arrays[n]);
        pcaStatistics->SetColumnStatus(tmpName.c_str(), 1 );
    }

    vtkSmartPointer<vtkDoubleArray> variation=vtkSmartPointer<vtkDoubleArray>::New();
    variation->SetName(name.c_str());
    variation->SetNumberOfTuples(T);


    itk::Array<double > means(nDescriptors);
    means.Fill(0);


    for (int t=0;t<T;t++){
        auto descriptor = descriptors[t];

        for(int c=0;c<descriptor.size();c++){
            datasetTable->SetValue(t,c,descriptor[c]);
            means[c]+=descriptor[c];

        }
    }

    for(int i=0;i<means.Size();i++){
        means[i]/=T;
    }

    pcaStatistics->RequestSelectedColumns();
    pcaStatistics->SetDeriveOption(true);
    pcaStatistics->Update();

    vtkSmartPointer<vtkDoubleArray> eigenvectors =
            vtkSmartPointer<vtkDoubleArray>::New();

    pcaStatistics->GetEigenvectors(eigenvectors);

    vtkSmartPointer<vtkDoubleArray> evec1 = vtkSmartPointer<vtkDoubleArray>::New();
    pcaStatistics->GetEigenvector(0, evec1);

    for(int t=0;t<T;t++){
        double val=0;
        for(int c=0;c<nDescriptors;c++){
            val+=(arrays[c]->GetValue(t)-means[c])*evec1->GetValue(c);
        }
        std::cout << val << std::endl;
        variation->SetTuple1(t,val);

    }
    this->m_pGraphPlotterDockWidget->AddPlot(variation,color,true);
    this->m_pGraphPlotterDockWidget->Draw();

}
void DrosophilaOmmatidiaExplorer::slotPlotSelectedEdgeMolecularDistribution(){
	this->slotPlotEdgeMolecularDistribution(m_SelectedEdge);
}
void DrosophilaOmmatidiaExplorer::slotPlotEdgeMolecularDistribution(const OmmatidiaTissue<3>::AJGraphType::AJEdgeHandler & edgeHandler){
    std::vector<itk::Array<double> > descriptorSeries;
    int T = m_Project.GetNumberOfFrames();

    auto sourceHandler=m_CurrentAJGraph->GetAJEdgeSource(edgeHandler);
    auto targetHandler=m_CurrentAJGraph->GetAJEdgeTarget(edgeHandler);

    DrosophilaOmmatidiaJSONProject::AJCorrespondenceType::AJSubgraphType edgeSubgraph;



    edgeSubgraph.AddEdge(edgeHandler);
    edgeSubgraph.AddVertex(sourceHandler);
    edgeSubgraph.AddVertex(targetHandler);


	auto edge = m_CurrentAJGraph->GetAJEdge(edgeHandler);
	descriptorSeries.push_back(edge->GetDescriptor());

    auto currentSubgraph=edgeSubgraph;
    for(int t=m_CurrentFrame+1;t<this->m_Project.GetNumberOfFrames();t++){
        auto tissue = m_Project.GetTissueDescriptor(t);
        auto correspondences = m_Project.GetCorrespondences(t-1,t);

        auto resultSet = correspondences.FindByAntecessor(currentSubgraph);
        if(resultSet.first!=resultSet.second){
			auto correspondence = resultSet.first;

			currentSubgraph = correspondence->GetSuccessor();

			auto vertexIt=currentSubgraph.BeginVertices();
			auto source = *vertexIt;
			++vertexIt;
			auto target = *vertexIt;

			auto edgeHandler = m_CurrentAJGraph->GetAJEdgeHandler(source,target);
			auto edge = m_CurrentAJGraph->GetAJEdge(edgeHandler);
			descriptorSeries.push_back(edge->GetDescriptor());

        }else{
        	break;
        }

    }
    auto color = this->m_EdgesDrawer.GetEdgeColor(edgeHandler);

    std::string arrayName("edgeDistribution-" + std::to_string(this->m_CurrentAJGraph->GetAJEdgeSource(edgeHandler)) + "-" + std::to_string(this->m_CurrentAJGraph->GetAJEdgeTarget(edgeHandler)));

    this->PlotDescriptor(descriptorSeries,color,arrayName);
#if 0

     DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler source= m_CurrentAJGraph->GetAJEdgeSource(edge);
     DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler target= m_CurrentAJGraph->GetAJEdgeTarget(edge);


    for(int t=0;t<T;t++){
         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer ajGraph =m_Project.GetAJGraph(t);

        auto distribution = ajGraph->GetAJEdge(ajGraph->GetAJEdgeHandler(source,target))->GetDescriptor();
        std::cout << distribution << std::endl;
        descriptorSeries.push_back(distribution);
    }
    auto color = this->m_EdgesDrawer.GetEdgeColor(edge);

    std::string arrayName("edgeDistribution-" + std::to_string(this->m_CurrentAJGraph->GetAJEdgeSource(edge)) + "-" + std::to_string(this->m_CurrentAJGraph->GetAJEdgeTarget(edge)));

    this->PlotDescriptor(descriptorSeries,color,arrayName);
#endif
}

void DrosophilaOmmatidiaExplorer::slotDoExportMovie(){

	this->m_Renderer->GetRenderWindow()->SetSize(1280,720);
#if 0
	vtkSmartPointer<vtkFFMPEGWriter> writer = vtkSmartPointer<vtkFFMPEGWriter>::New();

	writer->SetFileName("test.avi");

	vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =  vtkSmartPointer<vtkWindowToImageFilter>::New();
	  //this->m_Renderer->GetRenderWindow()->set
	windowToImageFilter->SetMagnification(1); //set the resolution of the output image (3 times the current resolution of vtk render window)
	windowToImageFilter->SetInputBufferTypeToRGB(); //also record the alpha (transparency) channel
	  //windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
	windowToImageFilter->SetInput(this->m_Renderer->GetRenderWindow());

	//writer->SetBitRate(this->m_Renderer->GetRenderWindow()->GetSize()[0]*this->m_Renderer->GetRenderWindow()->GetSize()[1]*this->m_Renderer->GetRenderWindow()->GetSize()[2]);
	writer->SetRate(15);
	writer->SetQuality(2);

	//writer->SetCompression(false);
	writer->SetInputConnection(windowToImageFilter->GetOutputPort());
	writer->Start();


	  for(int t=0;t<m_Project.GetNumberOfFrames();t++){

		  this->DrawFrame(t);
		  this->m_Renderer->GetRenderWindow()->Render();

		  windowToImageFilter->Update();
		  writer->Write();
	  }
	  writer->End();
#endif
#if 1



    for(int t=0;t<m_Project.GetNumberOfFrames();t++){
        this->DrawFrame(t);
        // Screenshot
        vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
                vtkSmartPointer<vtkWindowToImageFilter>::New();
        windowToImageFilter->SetInput(this->m_Renderer->GetRenderWindow());

        //windowToImageFilter->SetMagnification(3); //set the resolution of the output image (3 times the current resolution of vtk render window)
        windowToImageFilter->SetInputBufferTypeToRGB(); //also record the alpha (transparency) channel
        windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
        windowToImageFilter->Update();

        vtkSmartPointer<vtkPNGWriter> writer =
                vtkSmartPointer<vtkPNGWriter>::New();
        std::string frameName = std::string("frame") + std::to_string(t) + std::string(".png");
        writer->SetFileName(frameName.c_str());
        writer->SetInputConnection(windowToImageFilter->GetOutputPort());
        writer->Write();
    }
#endif
    this->DrawFrame(m_CurrentFrame);

}
void DrosophilaOmmatidiaExplorer::slotPlotSelectedVertexMolecularDistribution(){
	this->slotPlotVertexMolecularDistribution(m_SelectedVertex);
}
void DrosophilaOmmatidiaExplorer::slotPlotVertexMolecularDistribution(const OmmatidiaTissue<3>::AJGraphType::AJVertexHandler & vertex){

    std::vector<itk::Array<double> > descriptors;
    int T = m_Project.GetNumberOfFrames();
    for(int t=0;t<T;t++){

        auto graph  =m_Project.GetAJGraph(t);
        itk::Array<double> descriptor = graph->GetAJVertex(vertex)->GetDescriptor();
        descriptors.push_back(descriptor);
    }
    itk::FixedArray<double,3> color;
    color.Fill(0);
    std::string name("vertexDistribution-" + std::to_string(vertex));

    this->PlotDescriptor(descriptors,color,name);

#if 0
    ///////// Eigenvalues ////////////
    vtkSmartPointer<vtkDoubleArray> eigenvalues =
            vtkSmartPointer<vtkDoubleArray>::New();

    pcaStatistics->GetEigenvalues(eigenvalues);
    //  double eigenvaluesGroundTruth[3] = {.5, .166667, 0};
    for(vtkIdType i = 0; i < eigenvalues->GetNumberOfTuples(); i++)
    {
        std::cout << "Eigenvalue " << i << " = " << eigenvalues->GetValue(i) << std::endl;
    }

    ///////// Eigenvectors ////////////
    vtkSmartPointer<vtkDoubleArray> eigenvectors =
            vtkSmartPointer<vtkDoubleArray>::New();

    pcaStatistics->GetEigenvectors(eigenvectors);
    for(vtkIdType i = 0; i < eigenvectors->GetNumberOfTuples(); i++)
    {
        std::cout << "Eigenvector " << i << " : ";
        double* evec = new double[eigenvectors->GetNumberOfComponents()];
        eigenvectors->GetTuple(i, evec);
        for(vtkIdType j = 0; j < eigenvectors->GetNumberOfComponents(); j++)
        {
            std::cout << evec[j] << " ";
            vtkSmartPointer<vtkDoubleArray> eigenvectorSingle =
                    vtkSmartPointer<vtkDoubleArray>::New();
            pcaStatistics->GetEigenvector(i, eigenvectorSingle);
        }
        delete evec;
        std::cout << std::endl;
    }
#endif

}


void DrosophilaOmmatidiaExplorer::slotExit() {

    qApp->exit();

}

void DrosophilaOmmatidiaExplorer::slotCellsShanged(){
#if 0
	this->m_Project.SetCellGraph(m_CurrentFrame,this->m_pCellListDockWidget->GetCells());
	this->m_CellDrawer.SetCells(m_pCellListDockWidget->GetCells());
	this->m_CellDrawer.SetVisibility(true);
	this->m_CellDrawer.Draw();
	this->m_RenderWindow->Render();
#endif
}
void DrosophilaOmmatidiaExplorer::slotSelectedCellChanged(DrosophilaOmmatidiaExplorer::CellGraphType::CellVertexHandler  cell){
	this->SetSelectedCell(cell);
}
