#include "AJTrackingFrame.h"
#include "ui_AJTrackingFrame.h"
#include <vtkPropPicker.h>
#include <vtkColor.h>
#include "NearestNeighborCellAssociationCommand.h"
#include "NearestNeighborEdgeAssociationCommand.h"
//#include "MinCostMaxFlowAJAssociationCommand.h"

class RenderSyncCommand : public vtkCommand{
public:
	std::list<vtkSmartPointer<vtkRenderWindow> > m_Renderers;

	static RenderSyncCommand * New(){
		return new RenderSyncCommand;
	}
	virtual void Execute (vtkObject *caller, unsigned long eventId, void *callData){
		for(vtkSmartPointer<vtkRenderWindow> render : m_Renderers){
			render->Render();
		}
	}
};

AJTrackingFrame::AJTrackingFrame(QWidget *parent, DrosophilaOmmatidiaJSONProject & project) :
    QDialog(parent),
	m_pUI(new Ui::AJTrackingFrame),
	m_Project(project)
{
	m_pUI->setupUi(this);


	this->m_pUI->frameSlider->setMinimum(0);
	this->m_pUI->frameSlider->setMaximum(m_Project.GetNumberOfFrames()-2);


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

	vtkSmartPointer<RenderSyncCommand> renderSyncCommand = vtkSmartPointer<RenderSyncCommand>::New();
	renderSyncCommand->m_Renderers.push_back(m_BeforeRenderWindow);
	renderSyncCommand->m_Renderers.push_back(m_AfterRenderWindow);
	this->m_Camera->AddObserver(vtkCommand::ModifiedEvent,renderSyncCommand);

	this->m_pUI->frameSlider->setMaximum(m_Project.GetNumberOfFrames());
	connect(this->m_pUI->frameSlider,SIGNAL(valueChanged(int)),SLOT(slotFrameChanged(int)));

	connect(this->m_pUI->actionSelectVertexBefore,SIGNAL(triggered()),SLOT(slotSelectVertexBefore()));
	connect(this->m_pUI->actionSelectVertexAfter,SIGNAL(triggered()),SLOT(slotSelectVertexAfter()));

	connect(this->m_pUI->actionSelectEdgeBefore,SIGNAL(triggered()),SLOT(slotSelectEdgeBefore()));
	connect(this->m_pUI->actionSelectEdgeAfter,SIGNAL(triggered()),SLOT(slotSelectEdgeAfter()));

	//this->m_pUI->actionSelectCellBefore->setShortcutContext(Qt::ApplicationShortcut);
	//this->m_pUI->actionSelectCellAfter->setShortcutContext(Qt::ApplicationShortcut);

	connect(this->m_pUI->actionSelectCellBefore,SIGNAL(triggered()),SLOT(slotSelectCellBefore()));
	connect(this->m_pUI->actionSelectCellAfter,SIGNAL(triggered()),SLOT(slotSelectCellAfter()));

	connect(this->m_pUI->actionFrameForward,SIGNAL(triggered()),SLOT(slotFrameForward()));
	connect(this->m_pUI->actionFrameBackward,SIGNAL(triggered()),SLOT(slotFrameBackward()));

	connect(this->m_pUI->actionAddCorrespondence,SIGNAL(triggered()),SLOT(slotAddCorrespondence()));

	connect(this->m_pUI->actionDeleteCorrespondence,SIGNAL(triggered()),SLOT(slotDeleteSelectedCorrespondences()));

	connect(this->m_pUI->correspondencesTableWidget,SIGNAL(itemSelectionChanged()),SLOT(slotCorrespondenceSelectionChanged()));


	connect(this->m_pUI->actionGuessCorrespondence,SIGNAL(triggered()),SLOT(slotGuessCorrespondence()));
	this->slotFrameChanged(0);

	this->addAction(this->m_pUI->actionSelectVertexAfter);
	this->addAction(this->m_pUI->actionSelectVertexBefore);
	this->addAction(this->m_pUI->actionSelectEdgeBefore);
	this->addAction(this->m_pUI->actionSelectEdgeAfter);
	this->addAction(this->m_pUI->actionSelectCellAfter);
	this->addAction(this->m_pUI->actionSelectCellBefore);
	this->addAction(this->m_pUI->actionAddCorrespondence);
	this->addAction(this->m_pUI->actionDeleteCorrespondence);
	this->addAction(this->m_pUI->actionFrameForward);
	this->addAction(this->m_pUI->actionFrameBackward);
	this->addAction(this->m_pUI->actionGuessCorrespondence);
	m_PointWidget=vtkSmartPointer<vtkPointWidget>::New();
	m_QtToVTKConnections=vtkSmartPointer<vtkEventQtSlotConnect>::New();

	this->m_IsVertexSelectedBefore=false;
	this->m_IsVertexSelectedAfter=false;
	this->m_IsEdgeSelectedBefore=false;
	this->m_IsEdgeSelectedAfter=false;
	this->m_IsCellSelectedBefore=false;
	this->m_IsCellSelectedAfter=false;

}


AJTrackingFrame::~AJTrackingFrame()
{
    delete m_pUI;
}


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

void AJTrackingFrame::slotGuessCorrespondence(){

	if(this->m_IsCellSelectedBefore){
		typedef NearestNeighborCellAssociationCommand<OmmatidiaTissue<3>> CellAssociator;

		CellAssociator associator;

		associator.SetCell0(this->m_CellSelectedBefore);
		associator.SetTissue0(this->m_BeforeTissue);
		associator.SetTissue1(this->m_AfterTissue);
		associator.Do();

		auto cell1=associator.GetCell1();

		this->setSelectedCellAfter(cell1);
		this->m_AfterRenderWindow->Render();
	}
	if(this->m_IsEdgeSelectedBefore){
		typedef NearestNeighborEdgeAssociationCommand<OmmatidiaTissue<3>> EdgeAssociator;

		EdgeAssociator associator;

		associator.SetEdge0(this->m_EdgeSelectedBefore);
		associator.SetTissue0(this->m_BeforeTissue);
		associator.SetTissue1(this->m_AfterTissue);
		associator.Do();

		auto edge1=associator.GetEdge1();

		this->setSelectedEdgeAfter(edge1);
		this->m_AfterRenderWindow->Render();
	}
	if(this->m_IsVertexSelectedBefore){

		auto vertexPosition = m_BeforeTissue->GetAJGraph()->GetAJVertex(m_VertexSelectedBefore)->GetPosition();

		double minDist = std::numeric_limits<double>::max();
		VertexType vertex1;
		for (auto vertexIt = m_AfterTissue->GetAJGraph()->VerticesBegin();vertexIt!=m_AfterTissue->GetAJGraph()->VerticesEnd();++vertexIt){
			auto pos1= m_AfterTissue->GetAJGraph()->GetAJVertex(*vertexIt)->GetPosition();
			double dist = (vertexPosition-pos1).GetNorm();
			if(dist<minDist){
				minDist=dist;
				vertex1=*vertexIt;
			}
		}

		this->setSelectedVertexAfter(vertex1);
		this->m_AfterRenderWindow->Render();
	}
}

void AJTrackingFrame::slotFrameForward(){
	this->setFrame(m_CurrentFrame+1);
}
void AJTrackingFrame::slotFrameBackward(){
	this->setFrame(m_CurrentFrame-1);
}
void AJTrackingFrame::setFrame(int frame){
	this->slotFrameChanged(frame);
	this->m_pUI->frameSlider->blockSignals(true);
	this->m_pUI->frameSlider->setValue(frame);
	this->m_pUI->frameSlider->blockSignals(false);
}
void AJTrackingFrame::slotCorrespondenceSelectionChanged(){

if(this->m_pUI->correspondencesTableWidget->selectedItems().size()>0){
	int selectedCorrespondence = this->m_pUI->correspondencesTableWidget->selectedItems()[0]->row();

	auto correspondence = m_PlottedCorrespondences[selectedCorrespondence];

	auto antecessor = correspondence.GetAntecessor();
	auto successor = correspondence.GetSuccessor();

	this->clearBeforeSelection();
	switch(antecessor.GetOrder()){
	case 3: //cell
	{
		auto cell =vertexSubsetToCellHandler<OmmatidiaTissue<3>,AJSubgraphType>(m_BeforeTissue,antecessor);
		this->setSelectedCellBefore(cell);
		break;
	}
	case 2:
	{
		auto vertexIt = antecessor.BeginVertices();

		auto source = *vertexIt;
		++vertexIt;

		auto target = *vertexIt;
		auto edge=m_BeforeTissue->GetAJGraph()->GetAJEdgeHandler(source,target);

		this->setSelectedEdgeBefore(edge);

		break;
	}

	case 1:
		auto vertex = *(antecessor.BeginVertices());
		this->setSelectedVertexBefore(vertex);



	}
	this->clearAfterSelection();
	switch(successor.GetOrder()){
		case 3: //cell
		{
			auto cell =vertexSubsetToCellHandler<OmmatidiaTissue<3>,AJSubgraphType>(m_AfterTissue,successor);

			this->setSelectedCellAfter(cell);

			break;
		}
		case 2:
		{
			auto vertexIt = successor.BeginVertices();

			auto source = *vertexIt;
			++vertexIt;

			auto target = *vertexIt;
			auto edge=m_AfterTissue->GetAJGraph()->GetAJEdgeHandler(source,target);
			this->setSelectedEdgeAfter(edge);

			break;
		}

		case 1:
		{
			auto vertex = *(successor.BeginVertices());
			this->setSelectedVertexAfter(vertex);
			break;
		}
	}
	this->m_BeforeRenderWindow->Render();
	this->m_AfterRenderWindow->Render();
}
}

template<class AJVertexSubsetType> QString to_qstring(const AJVertexSubsetType & subset){

	QString label;
	if(subset.GetOrder()==1){
		label =QString("%1").arg(*(subset.BeginVertices()));

	}else if(subset.GetOrder()==2){

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
	this->m_Project.SetCorrespondences(this->m_CurrentFrame,this->m_CurrentFrame+1,this->m_Correspondences);
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
	this->m_Project.SetCorrespondences(this->m_CurrentFrame,this->m_CurrentFrame+1,this->m_Correspondences);
	this->PopulateTable();
}
void AJTrackingFrame::clearBeforeSelection(){
	if(this->m_IsVertexSelectedBefore){
		this->m_BeforeVertexDrawer.DeemphasizeAJVertex(m_VertexSelectedBefore);
		this->m_IsVertexSelectedBefore=false;
	}
	if(this->m_IsEdgeSelectedBefore){
		this->m_BeforeEdgesDrawer.DeemphasizeAJEdge(m_EdgeSelectedBefore);
		this->m_IsEdgeSelectedBefore=false;
	}

	if(this->m_IsCellSelectedBefore){
		this->m_BeforeCellsDrawer.DeemphasizeCell(m_CellSelectedBefore);
		this->m_IsCellSelectedBefore=false;
	}
}
void AJTrackingFrame::clearAfterSelection(){
	if(this->m_IsVertexSelectedAfter){
		this->m_AfterVertexDrawer.DeemphasizeAJVertex(m_VertexSelectedAfter);
		this->m_IsVertexSelectedAfter=false;
	}
	if(this->m_IsEdgeSelectedAfter){
		this->m_AfterEdgesDrawer.DeemphasizeAJEdge(m_EdgeSelectedAfter);
		this->m_IsEdgeSelectedAfter=false;
	}

	if(this->m_IsCellSelectedAfter){
		this->m_AfterCellsDrawer.DeemphasizeCell(m_CellSelectedAfter);
		this->m_IsCellSelectedAfter=false;
	}
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

		this->clearBeforeSelection();

		this->setSelectedVertexBefore(clickedVertex);
		m_BeforeVertexDrawer.PickOff();
		m_QtToVTKConnections->Disconnect();
	}
}
void AJTrackingFrame::setSelectedVertexBefore(const AJTrackingFrame::VertexType & selectedVertex ){

	AJSubgraphType selection;
	selection.SetOrder(1);
	selection.AddVertex(selectedVertex);
	this->clearBeforeSelection();

	this->m_BeforeSelection=selection;

	m_VertexSelectedBefore=selectedVertex;
	this->m_IsVertexSelectedBefore=true;

	this->m_BeforeVertexDrawer.HighlightAJVertex(selectedVertex);

	this->m_pUI->beforeSelectLabel->setText(QString("Selected vertex %1").arg(selectedVertex));
}

void AJTrackingFrame::setSelectedVertexAfter(const AJTrackingFrame::VertexType & selectedVertex ){

	AJSubgraphType selection;
	selection.SetOrder(1);
	selection.AddVertex(selectedVertex);
	this->clearAfterSelection();

	this->m_AfterSelection=selection;

	m_VertexSelectedAfter=selectedVertex;
	this->m_IsVertexSelectedAfter=true;

	this->m_AfterVertexDrawer.HighlightAJVertex(selectedVertex);

	this->m_pUI->afterSelectLabel->setText(QString("Selected vertex %1").arg(selectedVertex));
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

		this->clearAfterSelection();

		this->setSelectedVertexAfter(clickedVertex);
		m_AfterVertexDrawer.PickOff();
		m_QtToVTKConnections->Disconnect();



	}
}
void AJTrackingFrame::setSelectedEdgeBefore(const AJTrackingFrame::EdgeType & selectedEdge ){
	this->m_BeforeEdgesDrawer.HighlightEdge(selectedEdge);

	AJSubgraphType selection;
	selection.SetOrder(2);
	selection.AddVertex(m_BeforeTissue->GetAJGraph()->GetAJEdgeSource(selectedEdge));
	selection.AddVertex(m_BeforeTissue->GetAJGraph()->GetAJEdgeTarget(selectedEdge));
	selection.AddEdge(selectedEdge);
	this->m_BeforeSelection=selection;
	m_EdgeSelectedBefore=selectedEdge;
	this->m_IsEdgeSelectedBefore=true;
	this->m_pUI->beforeSelectLabel->setText(QString("Selected edge %1").arg(to_qstring(selection)));


}
void AJTrackingFrame::setSelectedEdgeAfter(const AJTrackingFrame::EdgeType & selectedEdge ){
	this->m_AfterEdgesDrawer.HighlightEdge(selectedEdge);

	AJSubgraphType selection;
	selection.SetOrder(2);
	selection.AddVertex(m_AfterTissue->GetAJGraph()->GetAJEdgeSource(selectedEdge));
	selection.AddVertex(m_AfterTissue->GetAJGraph()->GetAJEdgeTarget(selectedEdge));
	selection.AddEdge(selectedEdge);
	this->m_AfterSelection=selection;
	m_EdgeSelectedAfter=selectedEdge;
	this->m_IsEdgeSelectedAfter=true;
	this->m_pUI->afterSelectLabel->setText(QString("Selected edge %1").arg(to_qstring(selection)));


}
void AJTrackingFrame::slotLeftClickBeforeEdgeSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*){
	int* clickPos = this->m_BeforeRenderWindowInteractor->GetEventPosition();
		    // Pick from this location.
	std::cout << "Click at " << clickPos[0] << " "<< clickPos[1] << " "<< std::endl;
	vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0, m_BeforeRenderer);

	vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

	if(pickedActor){

		auto clickedEdge = this->m_BeforeEdgesDrawer.GetEdgeFromActor(pickedActor);

		std::cout << "Picked " << clickedEdge << std::endl;

		this->clearBeforeSelection();
		this->setSelectedEdgeBefore(clickedEdge);
		m_BeforeEdgesDrawer.PickOff();
		m_QtToVTKConnections->Disconnect();

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

		std::cout << "Picked " << clickedEdge << std::endl;

		this->clearAfterSelection();
		this->setSelectedEdgeAfter(clickedEdge);
		m_AfterEdgesDrawer.PickOff();
		m_QtToVTKConnections->Disconnect();
	}
}



void AJTrackingFrame::setSelectedCellAfter(const AJTrackingFrame::CellType & selectedCell ){

	auto cell = this->m_AfterTissue->GetCellGraph()->GetCell(selectedCell);

	AJSubgraphType selection;
	selection.SetOrder(3);

	for(auto itVertices = cell->BeginPerimeterAJVertices();itVertices!=cell->EndPerimeterAJVertices();++itVertices){
		selection.AddVertex(*itVertices);
	}
	for(auto itEdges = cell->BeginPerimeterAJEdges();itEdges!=cell->EndPerimeterAJEdges();++itEdges){
		selection.AddEdge(*itEdges);
	}

	this->m_AfterSelection=selection;
	this->m_AfterCellsDrawer.HighlightCell(selectedCell);
	m_CellSelectedAfter=selectedCell;

	this->m_IsCellSelectedAfter=true;

	QString label=QString("Selected cell %1").arg(to_qstring(selection));

	this->m_pUI->afterSelectLabel->setText(label);

}
void AJTrackingFrame::setSelectedCellBefore(const AJTrackingFrame::CellType & selectedCell ){

	auto cell = this->m_BeforeTissue->GetCellGraph()->GetCell(selectedCell);

	AJSubgraphType selection;
	selection.SetOrder(3);

	for(auto itVertices = cell->BeginPerimeterAJVertices();itVertices!=cell->EndPerimeterAJVertices();++itVertices){
		selection.AddVertex(*itVertices);
	}
	for(auto itEdges = cell->BeginPerimeterAJEdges();itEdges!=cell->EndPerimeterAJEdges();++itEdges){
		selection.AddEdge(*itEdges);
	}

	this->m_BeforeSelection=selection;
	this->m_BeforeCellsDrawer.HighlightCell(selectedCell);
	m_CellSelectedBefore=selectedCell;

	this->m_IsCellSelectedBefore=true;

	QString label=QString("Selected cell %1").arg(to_qstring(selection));

	this->m_pUI->beforeSelectLabel->setText(label);

}

void AJTrackingFrame::slotLeftClickBeforeCellSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*){
	int* clickPos = this->m_BeforeRenderWindowInteractor->GetEventPosition();
		    // Pick from this location.
	vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0, m_BeforeRenderer);

	vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

	if(pickedActor){

		this->clearBeforeSelection();

		auto clickedCell = this->m_BeforeCellsDrawer.GetCellFromActor(pickedActor);


		this->setSelectedCellBefore(clickedCell);
		m_BeforeCellsDrawer.PickOff();
		m_QtToVTKConnections->Disconnect();
	}
}
void AJTrackingFrame::slotLeftClickAfterCellSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*){
	int* clickPos = this->m_AfterRenderWindowInteractor->GetEventPosition();
		    // Pick from this location.
	vtkSmartPointer<vtkPropPicker>  picker =  	vtkSmartPointer<vtkPropPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0, m_AfterRenderer);

	vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

	if(pickedActor){


		this->clearAfterSelection();

		auto clickedCell = this->m_AfterCellsDrawer.GetCellFromActor(pickedActor);

		this->setSelectedCellAfter(clickedCell);
		m_AfterCellsDrawer.PickOff();
		m_QtToVTKConnections->Disconnect();
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

	if(frame==m_CurrentFrame+1){
		m_Forward=true;
		m_Backward=false;
		m_Colors=m_AfterColors;
	}else if(frame==m_CurrentFrame-1){
		m_Forward=false;
		m_Backward=true;
		m_Colors=m_BeforeColors;
	}else{
		m_Forward=false;
		m_Backward=false;
		m_Colors.clear();
	}

	this->m_CurrentFrame=frame;

	QString beforeLabel = QString("t=%1").arg(m_CurrentFrame);
	this->m_pUI->beforeGroupBox->setTitle(beforeLabel);
	QString afterLabel = QString("t=%1").arg(m_CurrentFrame+1);
	this->m_pUI->afterGroupBox->setTitle(afterLabel);
	m_Correspondences = m_Project.GetCorrespondences(m_CurrentFrame,m_CurrentFrame+1);
	this->m_BeforeTissue=this->m_Project.GetTissueDescriptor(frame);
	this->m_AfterTissue=this->m_Project.GetTissueDescriptor(frame+1);

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


#if 1
	if(frame>0){
		auto previousCorrespondences = m_Project.GetCorrespondences(m_CurrentFrame-1,m_CurrentFrame);

		for(auto correspondence = previousCorrespondences.BeginCorrespondences();correspondence!=previousCorrespondences.EndCorrespondences();++correspondence){

			auto successor = correspondence->GetSuccessor();
			switch(successor.GetOrder()){
				case 3: //cell
				{
					vtkColor3d color;
					color[0]=0.0;
					color[1]=0.0;
					color[2]=0.0;
					auto cell =vertexSubsetToCellHandler<OmmatidiaTissue<3>,AJSubgraphType>(m_BeforeTissue,successor);
					this->m_BeforeCellsDrawer.SetCellColor(cell,color);

					break;
				}
				case 2:
				{
					vtkColor3d color;
					color[0]=0.0;
					color[1]=0.0;
					color[2]=0.0;

					auto vertexIt = successor.BeginVertices();

					auto source = *vertexIt;
					++vertexIt;

					auto target = *vertexIt;
					auto edge=m_BeforeTissue->GetAJGraph()->GetAJEdgeHandler(source,target);
					this->m_BeforeEdgesDrawer.SetEdgeColor(edge,color);


					break;
				}

				case 1:
				{
					vtkColor3d color;
					color[0]=0.0;
					color[1]=0.0;
					color[2]=0.0;

					auto vertex = *(successor.BeginVertices());
					this->m_BeforeVertexDrawer.SetVertexColor(vertex,color);
					break;
				}
			}
		}
	}
#endif


	for(auto correspondence = this->m_Correspondences.BeginCorrespondences();correspondence!=m_Correspondences.EndCorrespondences();++correspondence){

		vtkColor3d color;

		if(m_Forward && m_Colors.find(correspondence->GetAntecessor())!=m_Colors.end()){
			color=m_Colors[correspondence->GetAntecessor()];

		}else if(m_Backward && m_Colors.find(correspondence->GetSuccessor())!=m_Colors.end()){
			color=m_Colors[correspondence->GetSuccessor()];
		}else{
			color[0]=double(rand())/RAND_MAX;
			color[1]=double(rand())/RAND_MAX;
			color[2]=double(rand())/RAND_MAX;
		}

		auto antecessor = correspondence->GetAntecessor();
		m_BeforeColors[antecessor]=color;
		switch(antecessor.GetOrder()){
		case 3: //cell
		{
			auto cell =vertexSubsetToCellHandler<OmmatidiaTissue<3>,AJSubgraphType>(m_BeforeTissue,antecessor);
			this->m_BeforeCellsDrawer.SetCellColor(cell,color);

			break;
		}
		case 2:
		{
			auto vertexIt = antecessor.BeginVertices();

			auto source = *vertexIt;
			++vertexIt;

			auto target = *vertexIt;
			auto edge=m_BeforeTissue->GetAJGraph()->GetAJEdgeHandler(source,target);
			this->m_BeforeEdgesDrawer.SetEdgeColor(edge,color);


			break;
		}

		case 1:
		{
			auto vertex = *(antecessor.BeginVertices());
			this->m_BeforeVertexDrawer.SetVertexColor(vertex,color);
			break;
		}
		}
		auto successor = correspondence->GetSuccessor();
		m_AfterColors[successor]=color;
		switch(successor.GetOrder()){
				case 3: //cell
				{
					auto cell =vertexSubsetToCellHandler<OmmatidiaTissue<3>,AJSubgraphType>(m_AfterTissue,successor);
					this->m_AfterCellsDrawer.SetCellColor(cell,color);

					break;
				}
				case 2:
				{
					auto vertexIt = successor.BeginVertices();

					auto source = *vertexIt;
					++vertexIt;

					auto target = *vertexIt;
					auto edge=m_AfterTissue->GetAJGraph()->GetAJEdgeHandler(source,target);
					this->m_AfterEdgesDrawer.SetEdgeColor(edge,color);


					break;
				}

				case 1:
				{
					auto vertex = *(successor.BeginVertices());
					this->m_AfterVertexDrawer.SetVertexColor(vertex,color);
					break;
				}
				}
	}

	this->m_IsVertexSelectedBefore=false;
	this->m_IsVertexSelectedAfter=false;
	this->m_IsEdgeSelectedBefore=false;
	this->m_IsEdgeSelectedAfter=false;
	this->m_IsCellSelectedBefore=false;
	this->m_IsCellSelectedAfter=false;
	this->PopulateTable();
	this->m_BeforeRenderWindow->Render();
	this->m_AfterRenderWindow->Render();
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




void AJTrackingFrame::slotComputeCorrespondences(){
#if 0
	ttt::MinCostMaxFlowAJAssociationCommand<DrosophilaOmmatidiaJSONProject::TissueType> associator;

	associator.SetTissue0(this->m_BeforeTissue);
	associator.SetTissue1(this->m_AfterTissue);

	this->PopulateTables();
#endif
}

