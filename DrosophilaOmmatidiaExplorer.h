/*=========================================================================

  Program:   Visualization Toolkit
  Module:    SimpleView.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright 2009 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
  license for use of this work by or on behalf of the
  U.S. Government. Redistribution and use in source and binary forms, with
  or without modification, are permitted provided that this Notice and any
  statement of authorship are reproduced on all copies.

=========================================================================*/
#ifndef DrosophilaOmmatidiaExplorer_H
#define DrosophilaOmmatidiaExplorer_H

#include <vtkSmartPointer.h>    // Required for smart pointer internal ivars.
#include <vtkRenderer.h>
#include <QVTKInteractor.h>
#include <QMainWindow>

#include <vtkEventQtSlotConnect.h>
#include <vtkPointWidget.h>
#include <DrosophilaOmmatidiaJSONProject.h>
#include "MinMaxVolumeDrawer.h"

#include "VertexLocationsDrawer.h"
#include "EdgesDrawer.h"
#include "MotionFieldVolumeDrawer.h"
#include "VertexMotionsDrawer.h"

#include "AJGraph.h"
#include "AJVertex.h"



#include "EdgeListDockWidget.h"
#include "VertexListDockWidget.h"
#include "GraphPlotterDockWidget.h"


// Forward Qt class declarations
class Ui_DrosophilaOmmatidiaExplorer;


class DrosophilaOmmatidiaExplorer : public QMainWindow
{
  Q_OBJECT

private:

    class vtkMoveVertexCallback : public vtkCommand
    {
    public:
      static vtkMoveVertexCallback *New()
      {
        return new vtkMoveVertexCallback;
      }
      virtual void Execute(vtkObject *caller, unsigned long, void*)
      {
        vtkPointWidget *pointWidget = reinterpret_cast<vtkPointWidget*>(caller);
        //pointWidget->GetPolyData(this->PolyData);
        double position[3];
        pointWidget->GetPosition(position);
         DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::PointType pointPosition;
        pointPosition[0]=position[0];
        pointPosition[1]=position[1];
        pointPosition[2]=position[2];
        m_AJVertex->SetPosition(pointPosition);
        m_pDrawer->UpdateAJVertex(m_AJVertexHandler);
      }
      vtkMoveVertexCallback(){}
      DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexType::Pointer m_AJVertex;
      DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler m_AJVertexHandler;
      VertexLocationsDrawer<DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> * m_pDrawer;

    };

public:

  // Constructor/Destructor
  DrosophilaOmmatidiaExplorer();
  ~DrosophilaOmmatidiaExplorer();

public slots:

   //Project handling
  virtual void slotOpenProject();
  virtual void slotExit();

  //Algorithms
  virtual void slotDoVertexLocation();
  virtual void slotDoRefineVertexLocation();

  virtual void slotDoVertexTracking();
  virtual void slotDoVertexMolecularDistribution();
  virtual void slotDoEdgeMolecularDistribution();
  virtual void slotDoExportMovie();

  // Visualization slots

  virtual void slotFrameChanged(int);


  virtual void slotShowOriginalChanged(bool);
  virtual void slotShowOriginalModeChanged(const QString &);
  virtual void slotShowOriginalOpacityChanged(int);

  virtual void slotShowDeconvolutedChanged(bool);
  virtual void slotShowDeconvolutedModeChanged(const QString &);
  virtual void slotShowDeconvolutedOpacityChanged(int);

  virtual void slotShowMotionFieldChanged(bool);

  virtual void slotShowVertexMotionChanged(bool);

  virtual void slotShowVertexLocationsChanged(bool);

  virtual void slotShowPlatenessChanged(bool);
  virtual void slotShowPlatenessModeChanged(const QString &);
  virtual void slotShowPlatenessOpacityChanged(int);
  virtual void slotShowPlatenessSliceChanged(int);

  virtual void slotShowVertexnessChanged(bool);
  virtual void slotShowVertexnessOpacityChanged(int);

  virtual void slotShowMolecularChanged(bool);
  virtual void slotShowMolecularOpacityChanged(int);





  //Vertex interaction
  virtual void slotVertexSelectionToggled(bool);
  virtual void slotLeftClickVertexSelectionMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);
  virtual void slotRightClickVertexSelectionMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);

  virtual void slotVertexAdditionToggled(bool);
  virtual void slotDeleteVertex();

  virtual void slotLeftClickVertexAddMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);
  virtual void slotRightClickVertexAddMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);

  virtual void slotVertexMoveToggled(bool);
  virtual void slotRightClickVertexMoveMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);


  virtual void slotVertexTableSelectionChanged(AJGraph<AJVertex,AJEdge>::AJVertexHandler );





  //Edge interaction

  virtual void slotEdgeAdditionToggled(bool);
  virtual void slotEdgeSelectionToggled(bool);

  virtual void slotFirstLeftClickAddEdgeMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);
  virtual void slotSecondLeftClickAddEdgeMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);
  virtual void slotMouseMoveAddEdgeMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);
  virtual void slotDeleteEdge();
  virtual void slotEdgeTableSelectionChanged(AJGraph<AJVertex,AJEdge>::AJEdgeHandler );

  virtual void slotRightClickAddEdgeMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);
  virtual void slotLeftClickEdgeSelectionMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);
  virtual void slotRightClickEdgeSelectionMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);

//Interaction

  // Charting slots
  virtual void slotPlotEdgeLength(const AJGraph<AJVertex,AJEdge>::AJEdgeHandler & edge);
  virtual void slotPlotSelectedEdgeLength();

  virtual void slotPlotSelectedEdgeMolecularDistribution();
  virtual void slotPlotEdgeMolecularDistribution(const AJGraph<AJVertex,AJEdge>::AJEdgeHandler & edge);

  virtual void slotPlotSelectedVertexMolecularDistribution();
  virtual void slotPlotVertexMolecularDistribution(const AJGraph<AJVertex,AJEdge>::AJVertexHandler & vertex);



  //virtual void slotShowEdges(bool);
  //virtual void slotLeftClickSelectEdgeMode(vtkObject*, unsigned long, void*,void*,vtkCommand*);


protected slots:

private:
  void PlotDescriptor(const std::vector<itk::Array<double> > & descriptors);
  void DrawFrame(int frame);

  void SetSelectedVertex(const  DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler & );
  void ClearSelectedVertex();

  void SetSelectedEdge(const  DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJEdgeHandler & );
  void ClearSelectedEdge();

  vtkSmartPointer<vtkRenderer> m_Renderer;
  vtkSmartPointer<vtkRenderWindow> m_RenderWindow;
  vtkSmartPointer<QVTKInteractor> m_RenderWindowInteractor;

  Ui_DrosophilaOmmatidiaExplorer *m_pUI;

  int m_CurrentFrame;
  QString m_ProjectPath;

  DrosophilaOmmatidiaJSONProject m_Project;


  typedef MinMaxVolumeDrawer<typename DrosophilaOmmatidiaJSONProject::OriginalImageType> OriginalDrawerType;
  OriginalDrawerType m_OriginalDrawer;
  typedef MinMaxVolumeDrawer<typename DrosophilaOmmatidiaJSONProject::OriginalImageType> DeconvolutedDrawerType;
  DeconvolutedDrawerType m_DeconvolutedDrawer;

  typedef MinMaxVolumeDrawer<typename DrosophilaOmmatidiaJSONProject::OriginalImageType> PlatenessDrawerType;
  PlatenessDrawerType m_PlatenessDrawer;

  typedef MinMaxVolumeDrawer<typename DrosophilaOmmatidiaJSONProject::OriginalImageType> VertexnessDrawerType;
  VertexnessDrawerType m_VertexnessDrawer;

  typedef MinMaxVolumeDrawer<typename DrosophilaOmmatidiaJSONProject::OriginalImageType> MolecularDrawerType;
  MolecularDrawerType m_MolecularImageDrawer;

  typedef VertexMotionsDrawer<typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> VertexMotionsDrawerType;
  VertexMotionsDrawerType m_VertexMotionsDrawer;

  VertexLocationsDrawer<typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> m_VertexLocationsDrawer;

  EdgesDrawer<typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType> m_EdgesDrawer;

  MotionFieldVolumeDrawer<typename DrosophilaOmmatidiaJSONProject::MotionImageType>  m_MotionVolumeDrawer;

   DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer m_CurrentAJGraph;

   DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler m_SelectedVertex;

  bool m_IsSelectedEdge;
   DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJEdgeHandler m_SelectedEdge;


   DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::AJVertexHandler  m_AddingEdgeSource;

#if 0
  enum VolumeVisualizationType{VOLUME,SLICE};

  VolumeVisualizationType m_ShowOriginalVolumeMode;
  VolumeVisualizationType m_ShowDeconvolutedVolumeMode;
#endif

  vtkSmartPointer<vtkEventQtSlotConnect> m_QtToVTKConnections;


  typedef struct{
     vtkSmartPointer<vtkLineSource> lineSource;
     vtkSmartPointer<vtkPolyDataMapper> lineMapper;
     vtkSmartPointer<vtkActor> lineActor;
  } TemporaryRenderedLineStruct;

  TemporaryRenderedLineStruct * m_pTemporaryLineStruct;

  vtkSmartPointer<vtkPointWidget> m_PointWidget;


  EdgeListDockWidget * m_pEdgeListDockWidget;
  VertexListDockWidget * m_pVertexListDockWidget;
  GraphPlotterDockWidget * m_pGraphPlotterDockWidget;


};

#endif // DrosophilaOmmatidiaExplorer_H
