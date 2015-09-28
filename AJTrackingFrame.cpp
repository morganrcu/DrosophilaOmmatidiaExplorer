#include "AJTrackingFrame.h"
#include "ui_AJTrackingFrame.h"
#include <vtkPropPicker.h>
//#include "MinCostMaxFlowAJAssociationCommand.h"

AJTrackingFrame::AJTrackingFrame(QWidget *parent, DrosophilaOmmatidiaJSONProject * pProject) :
    QDialog(parent),
	m_pUI(new Ui::AJTrackingFrame),
	m_pProject(pProject)
{
	m_pUI->setupUi(this);


	this->m_pUI->frameSlider->setMinimum(0);
	this->m_pUI->frameSlider->setMaximum(pProject->GetNumberOfFrames()-2);


	this->m_BeforeRenderer = vtkSmartPointer<vtkRenderer>::New();
	this->m_AfterRenderer = vtkSmartPointer<vtkRenderer>::New();

	this->m_BeforeRenderWindow = this->m_pUI->beforeQVTKWidget->GetRenderWindow();
	this->m_AfterRenderWindow = this->m_pUI->afterQVTKWidget->GetRenderWindow();

	this->m_BeforeRenderWindowInteractor = m_BeforeRenderWindow->GetInteractor();
	this->m_AfterRenderWindowInteractor = m_AfterRenderWindow->GetInteractor();

	this->m_BeforeRenderWindow->StereoRenderOff();
	this->m_AfterRenderWindow->StereoRenderOff();

	this->m_BeforeRenderWindow->AddRenderer(this->m_BeforeRenderer);
	this->m_AfterRenderWindow->AddRenderer(this->m_AfterRenderer);

	this->m_BeforeEdgesDrawer.SetRenderer(this->m_BeforeRenderer);
	this->m_BeforeVertexDrawer.SetRenderer(this->m_BeforeRenderer);
	this->m_BeforeCellsDrawer.SetRenderer(this->m_BeforeRenderer);

	this->m_AfterEdgesDrawer.SetRenderer(this->m_AfterRenderer);
	this->m_AfterVertexDrawer.SetRenderer(this->m_AfterRenderer);
	this->m_AfterCellsDrawer.SetRenderer(this->m_AfterRenderer);
	this->m_Camera= vtkSmartPointer<vtkCamera>::New();
	this->m_BeforeRenderer->SetActiveCamera(this->m_Camera);
	this->m_AfterRenderer->SetActiveCamera(this->m_Camera);

	this->m_pUI->frameSlider->setMaximum(m_pProject->GetNumberOfFrames());
	connect(this->m_pUI->frameSlider,SIGNAL(valueChanged(int)),SLOT(slotFrameChanged(int)));

	connect(this->m_pUI->actionSelectVertexBefore,SIGNAL(triggered()),SLOT(slotSelectVertexBefore()));
	connect(this->m_pUI->actionSelectVertexAfter,SIGNAL(triggered()),SLOT(slotSelectVertexAfter()));

	connect(this->m_pUI->actionSelectEdgeBefore,SIGNAL(triggered()),SLOT(slotSelectEdgeBefore()));
	connect(this->m_pUI->actionSelectEdgeAfter,SIGNAL(triggered()),SLOT(slotSelectEdgeAfter()));

	connect(this->m_pUI->actionSelectCellBefore,SIGNAL(triggered()),SLOT(slotSelectCellBefore()));
	connect(this->m_pUI->actionSelectCellAfter,SIGNAL(triggered()),SLOT(slotSelectCellAfter()));

	connect(this->m_pUI->actionAddCorrespondence,SIGNAL(triggered()),SLOT(slotAddCorrespondence()));

	connect(this->m_pUI->actionDeleteCorrespondence,SIGNAL(triggered()),SLOT(slotDeleteSelectedCorrespondence()));
#if 0

	connect(this->m_pUI->actionAddSuccesor,SIGNAL(triggered()),SLOT(slotAddSuccesor()));
	connect(this->m_pUI->actionAddLeaving,SIGNAL(triggered()),SLOT(slotAddLeaving()));
	connect(this->m_pUI->actionAddMerge,SIGNAL(triggered()),SLOT(slotAddMerge()));
	connect(this->m_pUI->actionAddSplit,SIGNAL(triggered()),SLOT(slotAddSplit()));
	connect(this->m_pUI->actionAddT1,SIGNAL(triggered()),SLOT(slotAddT1()));
	connect(this->m_pUI->actionAddT2,SIGNAL(triggered()),SLOT(slotAddT2()));

	connect(this->m_pUI->actionDeleteEntering,SIGNAL(triggered()),SLOT(slotDeleteEntering()));
	connect(this->m_pUI->actionDeleteSuccesor,SIGNAL(triggered()),SLOT(slotDeleteSuccesor()));
	connect(this->m_pUI->actionDeleteLeaving,SIGNAL(triggered()),SLOT(slotDeleteLeaving()));
	connect(this->m_pUI->actionDeleteMerge,SIGNAL(triggered()),SLOT(slotDeleteMerge()));
	connect(this->m_pUI->actionDeleteSplit,SIGNAL(triggered()),SLOT(slotDeleteSplit()));
	connect(this->m_pUI->actionDeleteT1,SIGNAL(triggered()),SLOT(slotDeleteT1()));
	connect(this->m_pUI->actionDeleteT2,SIGNAL(triggered()),SLOT(slotDeleteT2()));

	connect(this->m_pUI->actionComputeCorrespondence,SIGNAL(triggered()),SLOT(slotComputeCorrespondences()));
#endif
	this->slotFrameChanged(0);


	m_PointWidget=vtkSmartPointer<vtkPointWidget>::New();
	m_QtToVTKConnections=vtkSmartPointer<vtkEventQtSlotConnect>::New();

}

AJTrackingFrame::~AJTrackingFrame()
{
    delete m_pUI;
}

template<class AJVertexSubsetType> QString to_qstring(const AJVertexSubsetType & subset){

	QString label;
	if(subset.GetOrder()==1){
		label =QString("%1").arg(*(subset.BeginVertices()));

	}else if(subset.GetOrder()==2){
		label = label.arg("vertex");
		auto vertexIt =subset.BeginVertices();
		auto vertexA = *vertexIt;
		++vertexIt;
		auto vertexB=*vertexIt;

		label =QString("(%1,%2)").arg(vertexA).arg(vertexB);
	}else if(subset.GetOrder()>2){
		label = QString("[");
		auto vertexIt =subset.BeginVertices();
		auto vertexA = *vertexIt;

		label = QString("[%1").arg(vertexA);
		++vertexIt;
		while(vertexIt!=subset.EndVertices()){
			auto vertexA = *vertexIt;
			label = QString("%1,%2").arg(label).arg(vertexA);
			++vertexIt;
		}
		label = QString("%1]").arg(label);
	}
	return label;

#if 0
		auto cell = this->m_AfterTissue->GetCellGraph()->GetCell(clickedCell);

		for (auto ajvertexIt = cell->BeginPerimeterAJVertices();ajvertexIt!=cell->EndPerimeterAJVertices();++ajvertexIt){
			label = QString("%1 %2").arg(label).arg(*ajvertexIt);
		}
		label = QString("%1]").arg(label);
		this->m_pUI->afterSelectLabel->setText(label);
#endif




}

void AJTrackingFrame::slotDeleteSelectedCorrespondences(){
	QList<QTableWidgetSelectionRange> selectedRanges =this->m_pUI->correspondencesTableWidget->selectedRanges();
	for( auto selectedRange : selectedRanges){
		for(int row = selectedRange.bottomRow();row<=selectedRange.topRow();++row){
			auto correspondence= m_PlottedCorrespondences[row];
			this->m_Correspondences.RemoveCorrespondence(correspondence.GetAntecessor(),correspondence.GetSuccessor());
		}
	}
	this->PopulateTable();
	this->m_pProject->SetCorrespondences(this->m_CurrentFrame,this->m_CurrentFrame+1,this->m_Correspondences);
//	m_Correspondences.DeleteCorrespondence();
}
void AJTrackingFrame::PopulateTable(){

	m_PlottedCorrespondences.resize(m_Correspondences.GetNumberOfCorrespondences());

	this->m_pUI->correspondencesTableWidget->setRowCount(m_Correspondences.GetNumberOfCorrespondences());
	this->m_pUI->correspondencesTableWidget->setColumnCount(2);
	int k=0;
	for(auto it= m_Correspondences.BeginCorrespondences();it!=m_Correspondences.EndCorrespondences();++it){
		QTableWidgetItem * antecessor = new QTableWidgetItem(to_qstring(it->GetAntecessor()));
		QTableWidgetItem * successor = new QTableWidgetItem(to_qstring(it->GetSuccessor()));

		this->m_pUI->correspondencesTableWidget->setItem(k,0,antecessor);
		this->m_pUI->correspondencesTableWidget->setItem(k,1,successor);
		this->m_PlottedCorrespondences[k]=*it;
		k++;
	}
}
void AJTrackingFrame::slotAddCorrespondence(){
	this->m_Correspondences.AddCorrespondence(this->m_BeforeSelection,this->m_AfterSelection,0);
	this->PopulateTable();
}
void AJTrackingFrame::slotLeftClickBeforeVertexSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*){

	int* clickPos = this->m_BeforeRenderWindowInteractor->GetEventPosition();
	    // Pick from this location.
	vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0, m_BeforeRenderer);

	vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

	if(pickedActor){
		auto clickedVertex = this->m_BeforeVertexDrawer.GetVertexFromActor(pickedActor);
		std::cout << "Picked " << clickedVertex << std::endl;
		m_BeforeVertexDrawer.PickOff();
		m_QtToVTKConnections->Disconnect();

		AJSubgraphType selection;
		selection.SetOrder(1);
		selection.AddVertex(clickedVertex);
		this->m_BeforeSelection=selection;

		this->m_BeforeVertexDrawer.HighlightAJVertex(clickedVertex);

		this->m_pUI->beforeSelectLabel->setText(QString("Selected vertex %1").arg(to_qstring(selection)));
	}
}
void AJTrackingFrame::slotLeftClickAfterVertexSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*){
	int* clickPos = this->m_AfterRenderWindowInteractor->GetEventPosition();
		    // Pick from this location.
	vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0, m_AfterRenderer);

	vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

	if(pickedActor){
		auto clickedVertex = this->m_AfterVertexDrawer.GetVertexFromActor(pickedActor);
		std::cout << "Picked " << clickedVertex << std::endl;
		m_AfterVertexDrawer.PickOff();
		this->m_AfterVertexDrawer.HighlightAJVertex(clickedVertex);
		m_QtToVTKConnections->Disconnect();

		AJSubgraphType selection;
		selection.SetOrder(1);
		selection.AddVertex(clickedVertex);
		this->m_AfterSelection=selection;


		this->m_pUI->afterSelectLabel->setText(QString("Selected vertex %1").arg(clickedVertex));

	}
}
void AJTrackingFrame::slotLeftClickBeforeEdgeSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*){
	int* clickPos = this->m_BeforeRenderWindowInteractor->GetEventPosition();
		    // Pick from this location.
	vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0, m_BeforeRenderer);

	vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

	if(pickedActor){

		auto clickedEdge = this->m_BeforeEdgesDrawer.GetEdgeFromActor(pickedActor);
		std::cout << "Picked " << clickedEdge << std::endl;
		m_BeforeEdgesDrawer.PickOff();
		m_QtToVTKConnections->Disconnect();
		this->m_BeforeEdgesDrawer.HighlightEdge(clickedEdge);


		AJSubgraphType selection;
		selection.SetOrder(2);
		selection.AddVertex(m_BeforeTissue->GetAJGraph()->GetAJEdgeSource(clickedEdge));
		selection.AddVertex(m_BeforeTissue->GetAJGraph()->GetAJEdgeTarget(clickedEdge));
		selection.AddEdge(clickedEdge);
		this->m_BeforeSelection=selection;

		this->m_pUI->beforeSelectLabel->setText(QString("Selected edge %1").arg(to_qstring(selection)));


	}
}

void AJTrackingFrame::slotLeftClickAfterEdgeSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*){
	int* clickPos = this->m_AfterRenderWindowInteractor->GetEventPosition();
		    // Pick from this location.
	vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0, m_AfterRenderer);

	vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

	if(pickedActor){
		auto clickedEdge = this->m_AfterEdgesDrawer.GetEdgeFromActor(pickedActor);
		m_AfterEdgesDrawer.PickOff();
		m_QtToVTKConnections->Disconnect();
		this->m_AfterEdgesDrawer.HighlightEdge(clickedEdge);


		AJSubgraphType selection;
		selection.SetOrder(2);
		selection.AddVertex(m_AfterTissue->GetAJGraph()->GetAJEdgeSource(clickedEdge));
		selection.AddVertex(m_AfterTissue->GetAJGraph()->GetAJEdgeTarget(clickedEdge));
		selection.AddEdge(clickedEdge);
		this->m_AfterSelection=selection;


		this->m_pUI->afterSelectLabel->setText(QString("Selected edge %1").arg(to_qstring(selection)));
	}
}

void AJTrackingFrame::slotLeftClickBeforeCellSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*){
	int* clickPos = this->m_BeforeRenderWindowInteractor->GetEventPosition();
		    // Pick from this location.
	vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0, m_BeforeRenderer);

	vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

	if(pickedActor){
		auto clickedCell = this->m_BeforeCellsDrawer.GetCellFromActor(pickedActor);
		m_BeforeCellsDrawer.PickOff();
		m_QtToVTKConnections->Disconnect();
		this->m_BeforeCellsDrawer.HighlightCell(clickedCell);

		auto cell = this->m_BeforeTissue->GetCellGraph()->GetCell(clickedCell);

		AJSubgraphType selection;
		selection.SetOrder(3);

		for(auto itVertices = cell->BeginPerimeterAJVertices();itVertices!=cell->EndPerimeterAJVertices();++itVertices){
			selection.AddVertex(*itVertices);
		}

		for(auto itEdges = cell->BeginPerimeterAJEdges();itEdges!=cell->EndPerimeterAJEdges();++itEdges){
			selection.AddEdge(*itEdges);
		}

		this->m_BeforeSelection=selection;

		QString label=QString("Selected cell %1").arg(to_qstring(selection));

		this->m_pUI->beforeSelectLabel->setText(label);
	}
}

void AJTrackingFrame::slotLeftClickAfterCellSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*){
	int* clickPos = this->m_AfterRenderWindowInteractor->GetEventPosition();
		    // Pick from this location.
	vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0, m_AfterRenderer);

	vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

	if(pickedActor){
		auto clickedCell = this->m_AfterCellsDrawer.GetCellFromActor(pickedActor);
		this->m_AfterCellsDrawer.HighlightCell(clickedCell);
		m_QtToVTKConnections->Disconnect();
		m_AfterCellsDrawer.PickOff();

		auto cell = this->m_BeforeTissue->GetCellGraph()->GetCell(clickedCell);

		AJSubgraphType selection;
		selection.SetOrder(3);

		for(auto itVertices = cell->BeginPerimeterAJVertices();itVertices!=cell->EndPerimeterAJVertices();++itVertices){
			selection.AddVertex(*itVertices);
		}

		for(auto itEdges = cell->BeginPerimeterAJEdges();itEdges!=cell->EndPerimeterAJEdges();++itEdges){
			selection.AddEdge(*itEdges);
		}

		this->m_AfterSelection=selection;
		QString label=QString("Selected cell %1").arg(to_qstring(selection));

		this->m_pUI->afterSelectLabel->setText(label);

	}
}

void AJTrackingFrame::slotSelectVertexBefore(){

	this->m_BeforeVertexDrawer.PickOn();
	this->m_BeforeEdgesDrawer.PickOff();
	this->m_BeforeCellsDrawer.PickOff();


	this->m_QtToVTKConnections->Connect(this->m_BeforeRenderWindowInteractor,vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickBeforeVertexSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
}
void AJTrackingFrame::slotSelectVertexAfter(){
	this->m_AfterVertexDrawer.PickOn();
	this->m_AfterEdgesDrawer.PickOff();
	this->m_AfterCellsDrawer.PickOff();

	this->m_QtToVTKConnections->Connect(this->m_AfterRenderWindowInteractor,vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickAfterVertexSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
}
void AJTrackingFrame::slotSelectEdgeBefore(){

	this->m_BeforeVertexDrawer.PickOff();
	this->m_BeforeEdgesDrawer.PickOn();
	this->m_BeforeCellsDrawer.PickOff();

	this->m_QtToVTKConnections->Connect(this->m_BeforeRenderWindowInteractor,vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickBeforeEdgeSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
}
void AJTrackingFrame::slotSelectEdgeAfter(){
	this->m_AfterVertexDrawer.PickOff();
	this->m_AfterEdgesDrawer.PickOn();
	this->m_AfterCellsDrawer.PickOff();
	this->m_QtToVTKConnections->Connect(this->m_AfterRenderWindowInteractor,vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickAfterEdgeSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
}
void AJTrackingFrame::slotSelectCellBefore(){
	this->m_BeforeVertexDrawer.PickOff();
	this->m_BeforeEdgesDrawer.PickOff();
	this->m_BeforeCellsDrawer.PickOn();

	this->m_QtToVTKConnections->Connect(this->m_BeforeRenderWindowInteractor,vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickBeforeCellSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
}
void AJTrackingFrame::slotSelectCellAfter(){
	this->m_AfterVertexDrawer.PickOff();
	this->m_AfterEdgesDrawer.PickOff();
	this->m_AfterCellsDrawer.PickOn();
	this->m_QtToVTKConnections->Connect(this->m_AfterRenderWindowInteractor,vtkCommand::LeftButtonPressEvent,this,SLOT(slotLeftClickAfterCellSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*)));
}
void AJTrackingFrame::slotFrameChanged(int frame){


	this->m_CurrentFrame=frame;
	m_Correspondences = m_pProject->GetCorrespondences(m_CurrentFrame,m_CurrentFrame+1);
	this->m_BeforeTissue=this->m_pProject->GetTissueDescriptor(frame);
	this->m_AfterTissue=this->m_pProject->GetTissueDescriptor(frame+1);

	this->m_BeforeEdgesDrawer.SetEdgesContainer(this->m_BeforeTissue->GetAJGraph());
	this->m_BeforeEdgesDrawer.Draw();
	this->m_BeforeEdgesDrawer.SetVisibility(true);

	this->m_AfterEdgesDrawer.SetEdgesContainer(this->m_AfterTissue->GetAJGraph());
	this->m_AfterEdgesDrawer.Draw();
	this->m_AfterEdgesDrawer.SetVisibility(true);

	this->m_BeforeVertexDrawer.SetVertexLocations(this->m_BeforeTissue->GetAJGraph());
	this->m_BeforeVertexDrawer.Draw();
	this->m_BeforeVertexDrawer.SetVisibility(true);

	this->m_AfterVertexDrawer.SetVertexLocations(this->m_AfterTissue->GetAJGraph());
	this->m_AfterVertexDrawer.Draw();
	this->m_AfterVertexDrawer.SetVisibility(true);

	this->m_BeforeCellsDrawer.SetAJVertices(this->m_BeforeTissue->GetAJGraph());
	this->m_BeforeCellsDrawer.SetCells(this->m_BeforeTissue->GetCellGraph());
	this->m_BeforeCellsDrawer.Draw();
	this->m_BeforeCellsDrawer.SetVisibility(true);

	this->m_AfterCellsDrawer.SetAJVertices(this->m_AfterTissue->GetAJGraph());
	this->m_AfterCellsDrawer.SetCells(this->m_AfterTissue->GetCellGraph());
	this->m_AfterCellsDrawer.Draw();
	this->m_AfterCellsDrawer.SetVisibility(true);
	this->PopulateTable();
#if 0
#if 0
	for( auto succesorsIt = this->m_Correspondences->BeginSuccesors();succesorsIt!=this->m_Correspondences->EndSuccesors();++succesorsIt){
		m_BeforeVertexDrawer.SetVertexColor(succesorsIt->GetV0(),0.0,0.0,1.0);
		m_AfterVertexDrawer.SetVertexColor(succesorsIt->GetV1(),0.0,0.0,1.0);
	}

	for( auto splitIt = this->m_Correspondences->BeginSplits();splitIt!=this->m_Correspondences->EndSplits();++splitIt){
		m_BeforeVertexDrawer.SetVertexColor(splitIt->GetV0(),1.0,0.0,0.0);
		m_AfterVertexDrawer.SetVertexColor(splitIt->GetV1A(),1.0,0.0,0.0);
		m_AfterVertexDrawer.SetVertexColor(splitIt->GetV1B(),1.0,0.0,0.0);
	}
#endif
	for( auto mergeIt = this->m_Correspondences->BeginMerges();mergeIt!=this->m_Correspondences->EndMerges();++mergeIt){
		m_BeforeVertexDrawer.SetVertexColor(mergeIt->GetV0A(),0.0,1.0,0.0);
		m_BeforeVertexDrawer.SetVertexColor(mergeIt->GetV0B(),0.0,1.0,0.0);
		m_AfterVertexDrawer.SetVertexColor(mergeIt->GetV1(),0.0,1.0,0.0);
	}
#if 1
	for( auto t1It = this->m_Correspondences->BeginT1s();t1It!=this->m_Correspondences->EndT1s();++t1It){
		m_BeforeVertexDrawer.SetVertexColor(t1It->GetV0A(),1.0,1.0,0.0);
		m_BeforeVertexDrawer.SetVertexColor(t1It->GetV0B(),1.0,1.0,0.0);
		m_AfterVertexDrawer.SetVertexColor(t1It->GetV1A(),1.0,1.0,0.0);
		m_AfterVertexDrawer.SetVertexColor(t1It->GetV1B(),1.0,1.0,0.0);
	}
	for( auto enteringIt = this->m_Correspondences->BeginEnterings();enteringIt!=this->m_Correspondences->EndEnterings();++enteringIt){
		m_AfterVertexDrawer.SetVertexColor(enteringIt->GetV1(),1.0,1.0,1.0);
	}

	for( auto leavingIt = this->m_Correspondences->BeginLeavings();leavingIt!=this->m_Correspondences->EndLeavings();++leavingIt){
		m_BeforeVertexDrawer.SetVertexColor(leavingIt->GetV0(),1.0,1.0,1.0);
	}

	for( auto t2It = this->m_Correspondences->BeginT2s();t2It!=this->m_Correspondences->EndT2s();++t2It){
		for(auto itSrc = t2It->BeginV0();itSrc!= t2It->EndV0();++itSrc){
			m_BeforeVertexDrawer.SetVertexColor(*itSrc,0.0,1.0,1.0);
		}
		for(auto itSrc = t2It->BeginV1();itSrc!= t2It->EndV1();++itSrc){
			m_AfterVertexDrawer.SetVertexColor(*itSrc,0.0,1.0,1.0);
		}
	}
#endif
	this->m_BeforeRenderWindow->Render();
	this->m_AfterRenderWindow->Render();
	this->PopulateTables();
#endif
}




void AJTrackingFrame::PopulateTables(){
#if 0
	this->m_pUI->succesorsTableWidget->clear();

	this->m_pUI->succesorsTableWidget->setColumnCount(2);
	this->m_pUI->succesorsTableWidget->setRowCount(m_Correspondences->GetNumberOfSuccesors());

	int k=0;
	for( auto succesorsIt = this->m_Correspondences->BeginSuccesors();succesorsIt!=this->m_Correspondences->EndSuccesors();++succesorsIt){
		{
		 QTableWidgetItem * v0Widget =new QTableWidgetItem;
		 v0Widget->setData(0,(unsigned long long) succesorsIt->GetV0());
		 this->m_pUI->succesorsTableWidget->setItem(k,0,v0Widget);
		}
		{
		 QTableWidgetItem * v1Widget =new QTableWidgetItem;
		 v1Widget->setData(0,(unsigned long long) succesorsIt->GetV1());
		 this->m_pUI->succesorsTableWidget->setItem(k,1,v1Widget);
		}
		k++;
	}
	k=0;

	this->m_pUI->mergesTableWidget->setRowCount(m_Correspondences->GetNumberOfMerges());
	this->m_pUI->mergesTableWidget->setColumnCount(3);
	for( auto mergesIt = this->m_Correspondences->BeginMerges();mergesIt!=this->m_Correspondences->EndMerges();mergesIt++){
		{
		QTableWidgetItem * v0aWidget =new QTableWidgetItem;
		v0aWidget->setData(0,(unsigned long long) mergesIt->GetV0A());
		this->m_pUI->mergesTableWidget->setItem(k,0,v0aWidget);
		}
		{

		QTableWidgetItem * v0bWidget =new QTableWidgetItem;
		v0bWidget->setData(0,(unsigned long long) mergesIt->GetV0B());
		this->m_pUI->mergesTableWidget->setItem(k,1,v0bWidget);
		}
		{
		QTableWidgetItem * v1Widget =new QTableWidgetItem;
		v1Widget->setData(0,(unsigned long long) mergesIt->GetV1());
		this->m_pUI->mergesTableWidget->setItem(k,2,v1Widget);
		}
		k++;

	}

	k=0;
	this->m_pUI->splitTableWidget->setRowCount(m_Correspondences->GetNumberOfSplits());
	this->m_pUI->splitTableWidget->setColumnCount(3);
	for( auto splitsIt = this->m_Correspondences->BeginSplits();splitsIt!=this->m_Correspondences->EndSplits();splitsIt++){

		{
			QTableWidgetItem * v0Widget =new QTableWidgetItem;
			v0Widget->setData(0,(unsigned long long) splitsIt->GetV0());
			this->m_pUI->splitTableWidget->setItem(k,0,v0Widget);
		}
		{

			QTableWidgetItem * v1aWidget =new QTableWidgetItem;
			v1aWidget->setData(0,(unsigned long long) splitsIt->GetV1A());
			this->m_pUI->splitTableWidget->setItem(k,1,v1aWidget);
		}
		{
			QTableWidgetItem * v1bWidget =new QTableWidgetItem;
			v1bWidget->setData(0,(unsigned long long) splitsIt->GetV1B());
			this->m_pUI->splitTableWidget->setItem(k,2,v1bWidget);
		}


		k++;
	}

	k=0;
	this->m_pUI->t1TableWidget->setColumnCount(4);
	this->m_pUI->t1TableWidget->setRowCount(m_Correspondences->GetNumberOfT1s());
	for( auto t1It = this->m_Correspondences->BeginT1s();t1It!=this->m_Correspondences->EndT1s();t1It++){

		{
			QTableWidgetItem * v0aWidget =new QTableWidgetItem;
			v0aWidget->setData(0,(unsigned long long) t1It->GetV0A());
			this->m_pUI->t1TableWidget->setItem(k,0,v0aWidget);
		}
		{
			QTableWidgetItem * v0bWidget =new QTableWidgetItem;
			v0bWidget->setData(0,(unsigned long long) t1It->GetV0B());
			this->m_pUI->t1TableWidget->setItem(k,1,v0bWidget);
		}
		{

			QTableWidgetItem * v1aWidget =new QTableWidgetItem;
			v1aWidget->setData(0,(unsigned long long) t1It->GetV1A());
			this->m_pUI->t1TableWidget->setItem(k,2,v1aWidget);
		}
		{
			QTableWidgetItem * v1bWidget =new QTableWidgetItem;
			v1bWidget->setData(0,(unsigned long long) t1It->GetV1B());
			this->m_pUI->t1TableWidget->setItem(k,3,v1bWidget);
		}


		k++;
	}

	k=0;
	this->m_pUI->t2TableWidget->setRowCount(m_Correspondences->GetNumberOfT2s());
	this->m_pUI->t2TableWidget->setColumnCount(4);


	for( auto t2It = this->m_Correspondences->BeginT2s();t2It!=this->m_Correspondences->EndT2s();t2It++){

#if 0
		this->m_pUI->t1TableWidget->item(k,0)->setText(QString("%1").arg((unsigned long)t2I->GetV0A()));
		this->m_pUI->t1TableWidget->item(k,1)->setText(QString("%1").arg((unsigned long)t1It->GetV0B()));
		this->m_pUI->t1TableWidget->item(k,2)->setText(QString("%1").arg((unsigned long)t1It->GetV1A()));
		this->m_pUI->t1TableWidget->item(k,3)->setText(QString("%1").arg((unsigned long)t1It->GetV1B()));
		k++;
#endif
	}

	this->m_pUI->enteringTableWidget->setRowCount(0);
	this->m_pUI->leavingTableWidget->setRowCount(0);
#endif
}

void AJTrackingFrame::slotComputeCorrespondences(){
#if 0
	ttt::MinCostMaxFlowAJAssociationCommand<DrosophilaOmmatidiaJSONProject::TissueType> associator;

	associator.SetTissue0(this->m_BeforeTissue);
	associator.SetTissue1(this->m_AfterTissue);

	this->PopulateTables();
#endif
}

