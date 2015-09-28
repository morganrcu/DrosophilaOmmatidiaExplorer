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
    explicit AJTrackingFrame(QWidget *parent = 0, DrosophilaOmmatidiaJSONProject * pProject=0);
    ~AJTrackingFrame();

public:
    void PopulateTables();
public slots:

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

public:
	void PopulateTable();
private:
	typename DrosophilaOmmatidiaJSONProject::CorrespondenceSetType m_Correspondences;


    Ui::AJTrackingFrame *m_pUI;

    DrosophilaOmmatidiaJSONProject * m_pProject;

    vtkSmartPointer<vtkRenderer> m_BeforeRenderer;
    vtkSmartPointer<vtkRenderer> m_AfterRenderer;

    vtkSmartPointer<vtkRenderWindow> m_BeforeRenderWindow;
    vtkSmartPointer<vtkRenderWindow> m_AfterRenderWindow;

    vtkSmartPointer<vtkRenderWindowInteractor> m_BeforeRenderWindowInteractor;
    vtkSmartPointer<vtkRenderWindowInteractor> m_AfterRenderWindowInteractor;

    unsigned int m_CurrentFrame;

    typedef OmmatidiaTissue<3>::AJGraphType AJGraphType;
    typedef DrosophilaOmmatidiaJSONProject::CellGraphType CellGraphType;


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

	typedef typename AJGraphType::AJVertexHandler VertexType;

	VertexType m_VertexSelectedBefore;
	bool m_IsVertexSelectedBefore;

	VertexType m_VertexSelectedAfter;
	bool m_IsVertexSelectedAfter;

	typedef typename AJGraphType::AJEdgeHandler EdgeType;

	EdgeType m_EdgeSelectedBefore;
	bool m_IsEdgeSelectedBefore;

	EdgeType m_EdgeSelectedAfter;
	bool m_IsEdgeSelectedAfter;

	typedef typename CellGraphType::CellVertexHandler CellType;

	CellType m_CellSelectedBefore;
	bool m_IsCellSelectedBefore;

	CellType m_CellSelectedAfter;
	bool m_IsCellSelectedAfter;

	typedef typename DrosophilaOmmatidiaJSONProject::CorrespondenceSetType CorrespondenceSetType;
	typedef typename CorrespondenceSetType::AJCorrespondenceType AJCorrespondenceType;


	typedef typename AJCorrespondenceType::AJSubgraphType AJSubgraphType;
	AJSubgraphType m_BeforeSelection;
	AJSubgraphType m_AfterSelection;


	std::vector<AJCorrespondenceType> m_PlottedCorrespondences;

};

#endif // AJTRACKINGFRAME_H