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
#include <QMainWindow>

#include <DrosophilaOmmatidiaJSONProject.h>

#include "MinMaxVolumeDrawer.h"
#include "MotionFieldVolumeDrawer.h"

// Forward Qt class declarations
class Ui_DrosophilaOmmatidiaExplorer;


class DrosophilaOmmatidiaExplorer : public QMainWindow
{
  Q_OBJECT

public:

  // Constructor/Destructor
  DrosophilaOmmatidiaExplorer();
  ~DrosophilaOmmatidiaExplorer();

public slots:

  virtual void slotOpenProject();
  virtual void slotExit();

  virtual void slotFrameChanged(int);

  virtual void slotShowDeconvolutedChanged(bool);
  virtual void slotShowOriginalChanged(bool);
  virtual void slotShowMotionFieldChanged(bool);

  virtual void slotShowOriginalModeChanged(const QString &);
  virtual void slotShowDeconvolutedModeChanged(const QString &);
protected:

protected slots:

private:

  void DrawFrame(int frame);
  vtkSmartPointer<vtkRenderer> m_Renderer;
  // Designer form
  Ui_DrosophilaOmmatidiaExplorer *m_pUI;

  enum VolumeVisualizationType{VOLUME,SLICE};

  VolumeVisualizationType m_ShowOriginalVolumeMode;
  VolumeVisualizationType m_ShowDeconvolutedVolumeMode;

  int m_CurrentFrame;
  QString m_ProjectPath;

  DrosophilaOmmatidiaJSONProject m_Project;

  MinMaxVolumeDrawer<typename DrosophilaOmmatidiaJSONProject::OriginalImageType> m_OriginalDrawer;
  MinMaxVolumeDrawer<typename DrosophilaOmmatidiaJSONProject::DeconvolutedImageType> m_DeconvolutedDrawer;

  MotionFieldVolumeDrawer<typename DrosophilaOmmatidiaJSONProject::MotionImageType>  m_MotionVolumeDrawer;

};

#endif // DrosophilaOmmatidiaExplorer_H
