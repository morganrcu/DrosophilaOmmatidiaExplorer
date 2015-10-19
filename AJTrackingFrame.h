#ifndef AJTRACKINGFRAME_H
#define AJTRACKINGFRAME_H

#include <QDialog>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkPointWidget.h>
#include <vtkEventQtSlotConnect.h>
#include "VertexLocationsDrawer.h"
#include "EdgesDrawer.h"
#include "DrosophilaOmmatidiaJSONProject.h"
#include "OmmatidiaTissue.h"
#include "CellDrawer.h"
namespace Ui {
class AJTrackingFrame;
}

class AJTrackingFrame : public QDialog
{
    Q_OBJECT

public:
    explicit AJTrackingFrame(QWidget *parent, DrosophilaOmmatidiaJSONProject & project);
    ~AJTrackingFrame();

public:

    typedef OmmatidiaTissue<3>::AJGraphType AJGraphType;
    typedef DrosophilaOmmatidiaJSONProject::CellGraphType CellGraphType;

	typedef CellGraphType::CellVertexHandler CellType;

	typedef AJGraphType::AJVertexHandler VertexType;
	typedef AJGraphType::AJEdgeHandler EdgeType;

    void clearBeforeSelection();
    void clearAfterSelection();
    void setSelectedCellAfter(const CellType & cell);
    void setSelectedCellBefore(const CellType & cell);
    void setSelectedEdgeAfter(const EdgeType & edge);
    void setSelectedEdgeBefore(const EdgeType & edge);
    void setSelectedVertexAfter(const VertexType & vertex);
    void setSelectedVertexBefore(const VertexType & vertex);
    void setFrame(int frame);
public slots:

	void slotFrameForward();
	void slotFrameBackward();
	void slotFrameChanged(int frame);
	void slotComputeCorrespondences();

	void slotDeleteSelectedCorrespondences();
	void slotAddCorrespondence();
	void slotSelectVertexBefore();
	void slotSelectVertexAfter();
	void slotSelectEdgeBefore();
	void slotSelectEdgeAfter();
	void slotSelectCellBefore();
	void slotSelectCellAfter();

	void slotLeftClickBeforeVertexSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*);
	void slotLeftClickAfterVertexSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*);
	void slotLeftClickBeforeEdgeSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*);
	void slotLeftClickAfterEdgeSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*);
	void slotLeftClickBeforeCellSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*);
	void slotLeftClickAfterCellSelection(vtkObject*,unsigned long, void*,void*,vtkCommand*);

	void slotCorrespondenceSelectionChanged();
public:
	void PopulateTable();

private:
	DrosophilaOmmatidiaJSONProject::CorrespondenceSetType m_Correspondences;


    Ui::AJTrackingFrame *m_pUI;

    DrosophilaOmmatidiaJSONProject & m_Project;

    vtkSmartPointer<vtkRenderer> m_BeforeRenderer;
    vtkSmartPointer<vtkRenderer> m_AfterRenderer;

    vtkSmartPointer<vtkRenderWindow> m_BeforeRenderWindow;
    vtkSmartPointer<vtkRenderWindow> m_AfterRenderWindow;

    vtkSmartPointer<vtkRenderWindowInteractor> m_BeforeRenderWindowInteractor;
    vtkSmartPointer<vtkRenderWindowInteractor> m_AfterRenderWindowInteractor;

    unsigned int m_CurrentFrame;


    typedef VertexLocationsDrawer<AJGraphType > VertexLocationsDrawerType;

    VertexLocationsDrawerType m_BeforeVertexDrawer;
    VertexLocationsDrawerType m_AfterVertexDrawer;

    typedef EdgesDrawer<AJGraphType > EdgesDrawerType;

    EdgesDrawerType m_BeforeEdgesDrawer;
    EdgesDrawerType m_AfterEdgesDrawer;

    typedef CellDrawer<CellGraphType,AJGraphType> CellsDrawerType;

    CellsDrawerType m_BeforeCellsDrawer;
    CellsDrawerType m_AfterCellsDrawer;


    OmmatidiaTissue<3>::Pointer m_BeforeTissue;
    OmmatidiaTissue<3>::Pointer m_AfterTissue;

    vtkSmartPointer<vtkCamera> m_Camera;

	vtkSmartPointer<vtkPointWidget> m_PointWidget;
	vtkSmartPointer<vtkEventQtSlotConnect> m_QtToVTKConnections;

	VertexType m_VertexSelectedBefore;
	bool m_IsVertexSelectedBefore;

	VertexType m_VertexSelectedAfter;
	bool m_IsVertexSelectedAfter;



	EdgeType m_EdgeSelectedBefore;
	bool m_IsEdgeSelectedBefore;

	EdgeType m_EdgeSelectedAfter;
	bool m_IsEdgeSelectedAfter;


	CellType m_CellSelectedBefore;
	bool m_IsCellSelectedBefore;

	CellType m_CellSelectedAfter;
	bool m_IsCellSelectedAfter;

	typedef DrosophilaOmmatidiaJSONProject::CorrespondenceSetType CorrespondenceSetType;
	typedef CorrespondenceSetType::AJCorrespondenceType AJCorrespondenceType;


	typedef AJCorrespondenceType::AJSubgraphType AJSubgraphType;
	AJSubgraphType m_BeforeSelection;
	AJSubgraphType m_AfterSelection;


	std::vector<AJCorrespondenceType> m_PlottedCorrespondences;

	std::map<AJSubgraphType,vtkColor3d > m_BeforeColors,m_AfterColors,m_Colors;

	bool m_Forward;
	bool m_Backward;
};

#endif // AJTRACKINGFRAME_H
