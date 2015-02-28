
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

#include "PredictAJGraph.h"

#include "VertexMolecularDistributionDescriptor.h"
#include "EdgeMolecularDistributionDescriptor.h"

#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkFloatArray.h>
#include <vtkPlot.h>


// Constructor
DrosophilaOmmatidiaExplorer::DrosophilaOmmatidiaExplorer()
{

  this->m_pUI = new Ui_DrosophilaOmmatidiaExplorer;
  this->m_pUI->setupUi(this);

  // Place the table view in the designer form
  //this->ui->tableFrame->layout()->addWidget(this->TableView->GetWidget());
    this->m_Renderer=vtkSmartPointer<vtkRenderer>::New();
    // VTK/Qt wedded
    this->m_pUI->qvtkWidget->GetRenderWindow()->AddRenderer(this->m_Renderer);

    this->m_QtToVTKConnections= vtkSmartPointer<vtkEventQtSlotConnect>::New();

    this->m_RenderWindowInteractor=this->m_pUI->qvtkWidget->GetInteractor();
    //this->m_DefaultInteractorStyle=this->m_RenderWindowInteractor->GetInteractorStyle();

    //this->m_VertexSelectionInteractorStyle=vtkSmartPointer<AJGraphSelectionInteractorStyle<VertexLocationsDrawer<typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> >  >::New();
    //this->m_VertexSelectionInteractorStyle=vtkSmartPointer<AJGraphSelectionInteractorStyle >::New();


  //Set up combo boxes

  this->m_pUI->showDeconvolutedModeComboBox->insertItem(0,"Volume");
  this->m_pUI->showDeconvolutedModeComboBox->setCurrentIndex(0);

  this->m_pUI->showOriginalModeComboBox->insertItem(0,"Volume");
  this->m_pUI->showOriginalModeComboBox->setCurrentIndex(0);


  // Set up action signals and slots
  connect(this->m_pUI->actionOpenFile, SIGNAL(triggered()), this, SLOT(slotOpenProject()));
  connect(this->m_pUI->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
  connect(this->m_pUI->actionDetectVertices,SIGNAL(triggered()),this,SLOT(slotDoVertexLocation()));
  connect(this->m_pUI->actionTrackVertices,SIGNAL(triggered()),this,SLOT(slotDoVertexTracking()));
  connect(this->m_pUI->frameSlider,SIGNAL(valueChanged(int)),this,SLOT(slotFrameChanged(int)));

  connect(this->m_pUI->showOriginalGroupBox,SIGNAL(toggled(bool)),this,SLOT(slotShowOriginalChanged(bool)));
  connect(this->m_pUI->showDeconvolutedGroupBox,SIGNAL(toggled(bool)),this,SLOT(slotShowDeconvolutedChanged(bool)));
  connect(this->m_pUI->showMotionFieldGroupBox,SIGNAL(toggled(bool)),this,SLOT(slotShowMotionFieldChanged(bool)));
  connect(this->m_pUI->showPlatenessGroupBox,SIGNAL(toggled(bool)),this,SLOT(slotShowPlatenessChanged(bool)));
  connect(this->m_pUI->showAJVerticesGroupBox,SIGNAL(toggled(bool)),this,SLOT(slotShowVertexLocationsChanged(bool)));
  connect(this->m_pUI->showMolecularChannelGroupBox,SIGNAL(toggled(bool)),SLOT(slotShowMolecularChanged(bool)));

  connect(this->m_pUI->showOriginalModeComboBox,SIGNAL(currentIndexChanged(QString)),SLOT(slotShowOriginalModeChanged(QString)));
  connect(this->m_pUI->showDeconvolutedModeComboBox,SIGNAL(currentIndexChanged(QString)),SLOT(slotShowDeconvolutedModeChanged(QString)));

  connect(this->m_pUI->showDeconvolutedOpacitySlider,SIGNAL(valueChanged(int)),SLOT(slotShowDeconvolutedOpacityChanged(int)));
  connect(this->m_pUI->showOriginalOpacitySlider,SIGNAL(valueChanged(int)),SLOT(slotShowOriginalOpacityChanged(int)));
  connect(this->m_pUI->showPlatenessOpacitySlider,SIGNAL(valueChanged(int)),SLOT(slotShowPlatenessOpacityChanged(int)));
  connect(this->m_pUI->showMolecularChannelOpacitySlider,SIGNAL(valueChanged(int)),SLOT(slotShowMolecularOpacityChanged(int)));


  connect(this->m_pUI->actionSelectVertex,SIGNAL(toggled(bool)),SLOT(slotVertexSelectionToggled(bool)));
  connect(this->m_pUI->actionAddVertices,SIGNAL(toggled(bool)),SLOT(slotVertexAdditionToggled(bool)));
  connect(this->m_pUI->actionDeleteVertex,SIGNAL(triggered()),SLOT(slotDeleteVertex()));

  connect(this->m_pUI->actionAddEdge,SIGNAL(toggled(bool)),SLOT(slotEdgeAdditionToggled(bool)));
  connect(this->m_pUI->actionSelectEdge,SIGNAL(toggled(bool)),SLOT(slotEdgeSelectionToggled(bool)));
  connect(this->m_pUI->actionDeleteEdge,SIGNAL(triggered()),SLOT(slotDeleteEdge()));



  connect(this->m_pUI->actionMoveVertex,SIGNAL(toggled(bool)),SLOT(slotVertexMoveToggled(bool)));


  connect(this->m_pUI->actionPlotEdge,SIGNAL(triggered()),SLOT(slotPlotEdgeLength()));

  //connect(this->m_pUI->actionShowEdges,SIGNAL(toggled(bool)),SLOT(slotShowEdges(bool)));



  m_DeconvolutedDrawer.SetRenderer(this->m_Renderer);
  m_OriginalDrawer.SetRenderer(this->m_Renderer);
  m_PlatenessDrawer.SetRenderer(this->m_Renderer);
  m_VertexnessDrawer.SetRenderer(this->m_Renderer);
  m_MolecularImageDrawer.SetRenderer(this->m_Renderer);

  m_MotionVolumeDrawer.SetRenderer(this->m_Renderer);

  m_VertexLocationsDrawer.SetRenderer(this->m_Renderer);
  m_EdgesDrawer.SetRenderer(this->m_Renderer);

  m_SelectedVertex=-1;
    this->m_IsSelectedEdge=false;


  m_pTemporaryLineStruct=NULL;

    m_PointWidget=vtkSmartPointer<vtkPointWidget>::New();
  m_PointWidget->AllOff();
  m_PointWidget->SetInteractor(this->m_RenderWindowInteractor);

  vtkSmartPointer<vtkCubeSource> cube=vtkSmartPointer<vtkCubeSource>::New();

  cube->SetBounds(0,1,0,1,0,1);
  cube->Update();
  m_PointWidget->SetInputData(cube->GetOutput());
  m_PointWidget->PlaceWidget();



  m_pVertexListDockWidget=  new VertexListDockWidget(this);
  this->addDockWidget(Qt::RightDockWidgetArea,this->m_pVertexListDockWidget);

  connect(this->m_pVertexListDockWidget,SIGNAL(SelectedVertexChanged(AJGraph<AJVertex>::AJVertexHandler)),SLOT(slotVertexTableSelectionChanged(AJGraph<AJVertex>::AJVertexHandler)));

    connect(this->m_pVertexListDockWidget,SIGNAL(visibilityChanged(bool)),this->m_pUI->actionShowVertices,SLOT(setChecked(bool)));

    m_pGraphPlotterDockWidget = new GraphPlotterDockWidget(this);
    this->addDockWidget(Qt::RightDockWidgetArea,this->m_pGraphPlotterDockWidget);


    m_pEdgeListDockWidget= new EdgeListDockWidget(this);
    m_pEdgeListDockWidget->setHidden(true);
    connect(this->m_pEdgeListDockWidget,SIGNAL(SelectedEdgeChanged(AJGraph<AJVertex>::AJEdgeHandler)),SLOT(slotEdgeTableSelectionChanged(AJGraph<AJVertex>::AJEdgeHandler)));
    connect(this->m_pEdgeListDockWidget,SIGNAL(visibilityChanged(bool)),this->m_pUI->actionShowEdges,SLOT(setChecked(bool)));
    connect(this->m_pUI->actionShowEdges,SIGNAL(toggled(bool)),this->m_pEdgeListDockWidget,SLOT(setVisible(bool)));



    connect(this->m_pUI->actionVertexMolecularDistribution,SIGNAL(triggered()),this->m_pEdgeListDockWidget,SLOT(slotDoVertexMolecularDistribution()));
    connect(this->m_pUI->actionEdgeMolecularDistribution,SIGNAL(triggered()),this->m_pEdgeListDockWidget,SLOT(slotDoVertexMolecularDistribution()));


 //   connect(this->m_pEdgeListDockWidget,SIGNAL(DrawEdgeRequested(AJGraph<AJVertex>::AJEdgeHandler)),SLOT(slotPlotEdgeLength(AJGraph<AJVertex>::AJEdgeHandler)));
}


void DrosophilaOmmatidiaExplorer::slotVertexTableSelectionChanged(AJGraph<AJVertex>::AJVertexHandler  selectedVertex){
    this->SetSelectedVertex(selectedVertex);
    this->m_Renderer->GetRenderWindow()->Render();
}


void DrosophilaOmmatidiaExplorer::slotEdgeTableSelectionChanged(AJGraph<AJVertex>::AJEdgeHandler selectedEdge){
    this->SetSelectedEdge(selectedEdge);
    this->m_Renderer->GetRenderWindow()->Render();
}

void DrosophilaOmmatidiaExplorer::slotVertexAdditionToggled(bool toggled){

    if(toggled){
        this->m_pUI->actionAddEdge->setEnabled(false);
        this->m_pUI->actionSelectEdge->setEnabled(false);
        this->m_pUI->actionSelectVertex->setEnabled(false);

        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickAddMode(vtkObject*,ulong,void*,void*,vtkCommand*)));
    }else{
        this->m_pUI->actionAddEdge->setEnabled(true);
        this->m_pUI->actionSelectEdge->setEnabled(true);
        this->m_pUI->actionSelectVertex->setEnabled(true);

        this->m_QtToVTKConnections->Disconnect();
    }

}

void DrosophilaOmmatidiaExplorer::slotLeftClickAddMode(vtkObject*,ulong,void*,void*,vtkCommand*){

    int* clickPos = this->m_RenderWindowInteractor->GetEventPosition();
    // Pick from this location.
    vtkSmartPointer<vtkWorldPointPicker> picker = vtkSmartPointer<vtkWorldPointPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, m_Renderer);


    double picked[3];
    picker->GetPickPosition(picked);

    typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::Pointer newVertex = DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::New();

    typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::PointType point;
    point[0]=picked[0];
    point[1]=picked[1];
    point[2]=picked[2];
    newVertex->SetPosition(point);

    typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler newVertexHandler= this->m_CurrentAJGraph->AddAJVertex(newVertex);

    vtkSmartPointer<vtkActor> drawn=this->m_VertexLocationsDrawer.DrawAJVertex(newVertexHandler);
    drawn->SetVisibility(true);

    this->m_Renderer->GetRenderWindow()->Render();
    this->m_Project.SetAJGraph(this->m_CurrentFrame,this->m_CurrentAJGraph);


}

void DrosophilaOmmatidiaExplorer::slotVertexMoveToggled(bool toggled){

    if(toggled){

        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::Pointer vertex = this->m_CurrentAJGraph->GetAJVertex(this->m_SelectedVertex);
        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::PointType position= vertex->GetPosition();

        m_PointWidget->SetPosition(position[0],position[1],position[2]);

        vtkSmartPointer<vtkMoveVertexCallback> callback = vtkSmartPointer<vtkMoveVertexCallback>::New();


        callback->m_AJVertex=vertex;
        callback->m_AJVertexHandler=this->m_SelectedVertex;
        callback->m_pDrawer=&this->m_VertexLocationsDrawer;

        this->m_PointWidget->AddObserver(vtkCommand::InteractionEvent,callback);

        m_PointWidget->On();



    }else{
        this->m_Project.SetAJGraph(this->m_CurrentFrame,this->m_CurrentAJGraph);
        m_PointWidget->Off();
    }
}

void DrosophilaOmmatidiaExplorer::slotVertexSelectionToggled(bool toggled){

    if(toggled){

        this->m_pUI->actionAddEdge->setEnabled(false);
        this->m_pUI->actionAddVertices->setEnabled(false);
        this->m_pUI->actionSelectEdge->setEnabled(false);

        this->m_VertexLocationsDrawer.PickOn();
        this->m_pUI->actionAddVertices->setChecked(false);
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickVertexSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickVertexSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        std::cout << "VERTEX SELECTION ON" << std::endl;

    }else{
        this->m_pUI->actionAddEdge->setEnabled(true);
        this->m_pUI->actionAddVertices->setEnabled(true);
        this->m_pUI->actionSelectEdge->setEnabled(true);

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
        this->m_EdgesDrawer.PickOn();
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickEdgeSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        this->m_QtToVTKConnections->Connect(this->m_pUI->qvtkWidget->GetRenderWindow()->GetInteractor(),vtkCommand::RightButtonPressEvent,this,SLOT(slotRightClickEdgeSelectionMode(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
        std::cout << "EDGE SELECTION ON" << std::endl;
    }else{
        this->m_pUI->actionAddEdge->setEnabled(true);
        this->m_pUI->actionAddVertices->setEnabled(true);
        this->m_pUI->actionSelectVertex->setEnabled(true);
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
        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler clickedVertex = this->m_VertexLocationsDrawer.GetVertexFromActor(pickedActor);
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
        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJEdgeHandler clickedEdge = this->m_EdgesDrawer.GetEdgeFromActor(pickedActor);
        std::cout << "Clicked" <<  clickedEdge << std::endl;
        this->SetSelectedEdge(clickedEdge);
    }

}


void DrosophilaOmmatidiaExplorer::SetSelectedVertex(const typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler & selectedVertex){

    if(this->m_SelectedVertex!=-1){
        this->m_VertexLocationsDrawer.DeemphasizeAJVertex(this->m_SelectedVertex);
    }

    this->m_pUI->actionDeleteVertex->setEnabled(true);
    this->m_pUI->actionMoveVertex->setEnabled(true);

    this->m_VertexLocationsDrawer.HighlightVertex(selectedVertex);

    this->m_pVertexListDockWidget->SetSelectedVertex(selectedVertex);

    this->m_SelectedVertex=selectedVertex;
}

void DrosophilaOmmatidiaExplorer::SetSelectedEdge(const typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJEdgeHandler & selectedEdge){

    if(this->m_IsSelectedEdge){
        this->m_EdgesDrawer.DeemphasizeAJEdge(this->m_SelectedEdge);
    }
    this->m_IsSelectedEdge=true;
    this->m_SelectedEdge=selectedEdge;

    this->m_pUI->actionDeleteEdge->setEnabled(true);

    this->m_EdgesDrawer.HighlightEdge(selectedEdge);

    //this->m_pUI->edgesTableWidget->blockSignals(true);
    //this->m_pUI->edgesTableWidget->selectRow(selectedEdge);
    //this->m_pUI->edgesTableWidget->blockSignals(false);

}


void DrosophilaOmmatidiaExplorer::ClearSelectedVertex(){

    this->m_pUI->actionDeleteVertex->setEnabled(false);
    this->m_pUI->actionMoveVertex->setEnabled(false);

    this->m_pVertexListDockWidget->ClearSelection();



    this->m_VertexLocationsDrawer.DeemphasizeAJVertex(this->m_SelectedVertex);
    this->m_SelectedVertex=-1;
}

void DrosophilaOmmatidiaExplorer::ClearSelectedEdge(){

    this->m_pUI->actionDeleteEdge->setEnabled(false);

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

    DetectVerticesDialog dialog(this);
    dialog.exec();

    if(dialog.result()==dialog.Accepted){
        int referenceFrame=dialog.GetReferenceFrame();
        double threshold = dialog.GetThreshold();

#if 0

        typedef HessianImageToVertexnessImageFilter<DrosophilaOmmatidiaJSONProject::HessianImageType,DrosophilaOmmatidiaJSONProject::VertexnessImageType> HessianImageToVertexnessType;

        typename HessianImageToVertexnessType::Pointer hessianToVertexness = HessianImageToVertexnessType::New();

        hessianToVertexness->SetInput(;

        hessianToVertexness->Update();

        typename DrosophilaOmmatidiaJSONProject::VertexnessImageType::Pointer vertexnessImage=hessianToVertexness->GetOutput();

        m_Project.SetVertexnessImage(referenceFrame,vertexnessImage);


        typedef LocalMaximaGraphInitializer<DrosophilaOmmatidiaJSONProject::VertexnessImageType,DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> LocalMaxGraphInitializer;

        typename LocalMaxGraphInitializer::Pointer localMaxGraphInitializerFilter = LocalMaxGraphInitializer::New();

#endif

        typedef HessianGraphInitializer<DrosophilaOmmatidiaJSONProject::HessianImageType,DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> HessianGraphInitializer;
        typename HessianGraphInitializer::Pointer localMaxGraphInitializerFilter = HessianGraphInitializer::New();
        typename DrosophilaOmmatidiaJSONProject::HessianImageType::Pointer hessianImage=this->m_Project.GetHessianImage(referenceFrame);

        //localMaxGraphInitializerFilter->SetThreshold(1e-6);
        localMaxGraphInitializerFilter->SetInputImage(hessianImage);
        localMaxGraphInitializerFilter->Compute();

        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer localMax = localMaxGraphInitializerFilter->GetOutputGraph();

        this->m_Project.SetAJGraph(referenceFrame,localMax);

        m_VertexLocationsDrawer.Reset();
        m_VertexLocationsDrawer.SetVertexLocations(localMax);
        m_VertexLocationsDrawer.Draw();
        m_VertexLocationsDrawer.SetVisibility(this->m_pUI->showAJVerticesGroupBox->isChecked());



    }

}

void DrosophilaOmmatidiaExplorer::slotDeleteVertex(){

    assert(this->m_SelectedVertex!=-1);
    this->m_CurrentAJGraph->DeleteAJVertex(this->m_SelectedVertex);
    this->m_Project.SetAJGraph(this->m_CurrentFrame,this->m_CurrentAJGraph);

    this->m_VertexLocationsDrawer.Draw();
    this->m_VertexLocationsDrawer.SetVisibility(this->m_pUI->showAJVerticesGroupBox->isChecked());
    this->m_VertexLocationsDrawer.PickOn();

    this->m_EdgesDrawer.Draw();
    this->m_EdgesDrawer.SetVisibility(this->m_pUI->showAJVerticesGroupBox->isChecked());
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
    this->m_VertexLocationsDrawer.SetVisibility(this->m_pUI->showAJVerticesGroupBox->isChecked());
    this->m_VertexLocationsDrawer.PickOff();

    this->m_EdgesDrawer.Draw();
    this->m_EdgesDrawer.SetVisibility(this->m_pUI->showAJVerticesGroupBox->isChecked());
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
        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler clickedVertex = this->m_VertexLocationsDrawer.GetVertexFromActor(pickedActor);
        std::cout << "Clicked" <<  clickedVertex << std::endl;

        m_AddingEdgeSource= clickedVertex;

        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::Pointer sourceVertex=this->m_CurrentAJGraph->GetAJVertex(clickedVertex);

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

        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler clickedVertex = this->m_VertexLocationsDrawer.GetVertexFromActor(pickedActor);



        std::cout << "Linking " << this->m_AddingEdgeSource << " TO " << clickedVertex << std::endl;

        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJEdgeHandler newEdge=this->m_CurrentAJGraph->AddAJEdge(m_AddingEdgeSource,clickedVertex);

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




void DrosophilaOmmatidiaExplorer::slotDoVertexTracking(){

    for(int t=0;t<this->m_Project.GetNumFrames()-1;t++){
        typedef PredictAJGraph<DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType,DrosophilaOmmatidiaJSONProject::MotionImageType,DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> PredictAJGraphType;

        typename PredictAJGraphType::Pointer predict= PredictAJGraphType::New();
        predict->SetMotionImage(this->m_Project.GetMotionImage(t));
        predict->SetInputGraph(this->m_Project.GetAJGraph(t));
        predict->Compute();

        m_Project.SetAJGraph(t+1,predict->GetOutputGraph());
    }
}


void DrosophilaOmmatidiaExplorer::slotShowOriginalModeChanged(const QString & mode){

    if(mode=="Volume"){
        this->m_ShowOriginalVolumeMode=VOLUME;
    }
    //TODO Update signal

}

void DrosophilaOmmatidiaExplorer::slotShowDeconvolutedModeChanged(const QString & mode){

    if(mode=="Volume"){
        this->m_ShowDeconvolutedVolumeMode=VOLUME;
    }
    //TODO Update signal

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


void DrosophilaOmmatidiaExplorer::slotShowPlatenessChanged(bool value){

    m_PlatenessDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();

}

DrosophilaOmmatidiaExplorer::~DrosophilaOmmatidiaExplorer()
{


}
void DrosophilaOmmatidiaExplorer::slotFrameChanged(int frame){

    QString baseText("t=%1");
    this->m_pUI->currentFrameLabel->setText(baseText.arg(frame));

    this->m_CurrentFrame=frame;

    if(this->m_CurrentFrame==m_Project.GetNumFrames()-1){
        this->m_pUI->showMotionFieldGroupBox->setEnabled(false);
    }else{\
        this->m_pUI->showMotionFieldGroupBox->setEnabled(true);
    }

    this->DrawFrame(this->m_CurrentFrame);

}

void DrosophilaOmmatidiaExplorer::DrawFrame(int frame ){

    this->m_Renderer->RemoveAllViewProps();
    m_OriginalDrawer.SetImage(m_Project.GetOriginalImage(frame));
    m_OriginalDrawer.SetOpacity(double(this->m_pUI->showOriginalOpacitySlider->value())/1000000.0);
    m_OriginalDrawer.Draw();
    m_OriginalDrawer.SetVisibility(this->m_pUI->showOriginalGroupBox->isChecked());


    m_DeconvolutedDrawer.SetImage(m_Project.GetDeconvolutedImage(frame));
    m_DeconvolutedDrawer.SetOpacity(double(this->m_pUI->showDeconvolutedOpacitySlider->value())/1000000.0);
    m_DeconvolutedDrawer.Draw();
    m_DeconvolutedDrawer.SetVisibility(this->m_pUI->showDeconvolutedGroupBox->isChecked());


    m_PlatenessDrawer.SetImage(m_Project.GetPlatenessImage(frame));
    m_PlatenessDrawer.SetOpacity(double(this->m_pUI->showPlatenessOpacitySlider->value())/1000000.0);
    m_PlatenessDrawer.Draw();
    m_PlatenessDrawer.SetVisibility(this->m_pUI->showPlatenessGroupBox->isChecked());

    if(frame!=this->m_Project.GetNumFrames()-1){
        typename DrosophilaOmmatidiaJSONProject::MotionImageType::Pointer motionImage = m_Project.GetMotionImage(frame);
        m_MotionVolumeDrawer.SetMotionImage(motionImage);
        m_MotionVolumeDrawer.Draw();
        m_MotionVolumeDrawer.SetVisibility(this->m_pUI->showMotionFieldGroupBox->isChecked());

    }else{
        m_MotionVolumeDrawer.Reset();
    }

    if(this->m_Project.IsAJVertices(frame)){

        this->m_CurrentAJGraph=this->m_Project.GetAJGraph(frame);

        this->m_VertexLocationsDrawer.SetVertexLocations(this->m_CurrentAJGraph);
        this->m_VertexLocationsDrawer.Draw();
        this->m_VertexLocationsDrawer.SetVisibility(this->m_pUI->showAJVerticesGroupBox->isChecked());

        this->m_pVertexListDockWidget->SetVertexContainer(this->m_CurrentAJGraph);
        this->m_pVertexListDockWidget->Draw();


        this->m_EdgesDrawer.SetEdgesContainer(this->m_CurrentAJGraph);
        this->m_EdgesDrawer.Draw();
        this->m_EdgesDrawer.SetVisibility(this->m_pUI->showAJVerticesGroupBox->isChecked());

        this->m_pEdgeListDockWidget->SetEdgesContainer(this->m_CurrentAJGraph);
        this->m_pEdgeListDockWidget->Draw();

    }else{
        this->m_VertexLocationsDrawer.Reset();
        this->m_EdgesDrawer.Reset();
    }

    if(this->m_Project.IsMolecularImage(frame)){

        m_MolecularImageDrawer.SetImage(m_Project.GetMolecularImage(frame));
        m_MolecularImageDrawer.SetOpacity(double(this->m_pUI->showMolecularChannelOpacitySlider->value())/1000000.0);
        m_MolecularImageDrawer.Draw();
        m_MolecularImageDrawer.SetVisibility(this->m_pUI->showMolecularChannelGroupBox->isChecked());

    }else{
        this->m_MolecularImageDrawer.Reset();
    }


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
            this->m_pUI->frameSlider->setMaximum(this->m_Project.GetNumFrames()-1);
            this->m_pUI->frameSlider->setValue(0);
            this->m_pUI->frameSlider->setTickInterval(1);
            this->m_pUI->frameSlider->blockSignals(false);

            this->m_pUI->actionDetectVertices->setEnabled(true);
            this->m_pUI->actionSelectVertex->setEnabled(true);
            this->m_pUI->actionAddVertices->setEnabled(true);

            this->m_pUI->actionSelectEdge->setEnabled(true);
            this->m_pUI->actionAddEdge->setEnabled(true);

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
    for(unsigned int t=0;t<m_Project.GetNumFrames();t++){

        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer ajgraph= m_Project.GetAJGraph(t);

        typedef VertexMolecularDistributionDescriptor<typename  DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType,typename DrosophilaOmmatidiaJSONProject::MolecularImageType> VertexMolecularDistributionDescriptorType;

        typename VertexMolecularDistributionDescriptorType::Pointer vertexDescriptor = VertexMolecularDistributionDescriptorType::New();

        vertexDescriptor->SetMolecularImage(m_Project.GetMolecularImage(t));
        vertexDescriptor->SetRadius(3.0); //TODO
        vertexDescriptor->SetAJGraph(ajgraph);
        vertexDescriptor->Compute();

        typename DrosophilaOmmatidiaJSONProject::VertexMolecularFeatureMapType features=vertexDescriptor->GetComputedFeatures();
        m_Project.SetVertexMolecularDistribution(t,features);
    }
}

void DrosophilaOmmatidiaExplorer::slotDoEdgeMolecularDistribution(){
    for(unsigned int t=0;t<m_Project.GetNumFrames();t++){
        typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer ajgraph= m_Project.GetAJGraph(t);

        typedef EdgeMolecularDistributionDescriptor<typename  DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType,typename DrosophilaOmmatidiaJSONProject::MolecularImageType> EdgeMolecularDistributionDescriptorType;

        typename EdgeMolecularDistributionDescriptorType::Pointer edgeDescriptor = EdgeMolecularDistributionDescriptorType::New();

        edgeDescriptor->SetMolecularImage(m_Project.GetMolecularImage(t));
        edgeDescriptor->SetRadius(3.0); //TODO

        edgeDescriptor->SetAJGraph(ajgraph);
        edgeDescriptor->Compute();

        typename DrosophilaOmmatidiaJSONProject::EdgeMolecularFeatureMapType features=edgeDescriptor->GetComputedFeatures();
        m_Project.SetEdgeMolecularDistribution(t,features,ajgraph);
    }
}


void DrosophilaOmmatidiaExplorer::slotPlotEdgeLength(const AJGraph<AJVertex>::AJEdgeHandler & edge){


    vtkSmartPointer<vtkDoubleArray> arrT =vtkSmartPointer<vtkDoubleArray>::New();
    arrT->SetName("t");
    arrT->SetNumberOfTuples(this->m_Project.GetNumFrames());

    vtkSmartPointer<vtkDoubleArray> arrEdge =vtkSmartPointer<vtkDoubleArray>::New();
    arrEdge->SetName("edge");
    arrEdge->SetNumberOfTuples(this->m_Project.GetNumFrames());

    for(int t=0;t<this->m_Project.GetNumFrames();t++){
        auto graph = m_Project.GetAJGraph(t);

        auto sourceHandler=graph->GetAJEdgeSource(edge);
        auto targetHandler=graph->GetAJEdgeTarget(edge);

        auto sourceLocation = graph->GetAJVertex(sourceHandler)->GetPosition();
        auto targetLocation = graph->GetAJVertex(targetHandler)->GetPosition();

        auto diff =  sourceLocation - targetLocation;


        double dist = diff.GetNorm();

        arrEdge->SetTuple1(t,dist);
        arrT->SetTuple1(t,t);
    }

    this->m_pGraphPlotterDockWidget->SetTemporalReference(arrT);
    this->m_pGraphPlotterDockWidget->AddPlot(arrEdge);
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
void DrosophilaOmmatidiaExplorer::slotVertexMD(){

    typedef VertexMolecularDistributionDescriptor<typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType, DrosophilaOmmatidiaJSONProject::PlatenessImageType> VertexMolecularDistributionDescriptorType;

    typename VertexMolecularDistributionDescriptorType::Pointer vertexMDD=VertexMolecularDistributionDescriptorType::New();
}

void DrosophilaOmmatidiaExplorer::slotExit() {

  qApp->exit();

}

