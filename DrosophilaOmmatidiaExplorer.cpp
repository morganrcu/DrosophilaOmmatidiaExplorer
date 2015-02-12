/*
 * Copyright 2007 Sandia Corporation.
 * Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 * license for use of this work by or on behalf of the
 * U.S. Government. Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that this Notice and any
 * statement of authorship are reproduced on all copies.
 */


#include "ui_DrosophilaOmmatidiaExplorer.h"
#include "DrosophilaOmmatidiaExplorer.h"

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkVectorText.h>
#include "vtkSmartPointer.h"

#include <QFileDialog>
#include <QMessageBox>


#include <vtkHedgeHog.h>
#include <vtkProperty.h>

#include <itkImageToVTKImageFilter.h>
#include <vtkImageData.h>

#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

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

#if 0
  // Geometry
  VTK_CREATE(vtkVectorText, text);
  text->SetText("Drosophila Ommatidia Explorer");
  VTK_CREATE(vtkElevationFilter, elevation);
  elevation->SetInputConnection(text->GetOutputPort());
  elevation->SetLowPoint(0,0,0);
  elevation->SetHighPoint(10,0,0);

  // Mapper
  VTK_CREATE(vtkPolyDataMapper, mapper);
  mapper->ImmediateModeRenderingOn();
  mapper->SetInputConnection(elevation->GetOutputPort());

  // Actor in scene
  VTK_CREATE(vtkActor, actor);
  actor->SetMapper(mapper);
  // Add Actor to renderer
  this->m_Renderer->AddActor(actor);
#endif


  //Set up combo boxes

  this->m_pUI->showDeconvolutedComboBox->insertItem(0,"Volume");
  this->m_pUI->showDeconvolutedComboBox->setCurrentIndex(0);

  this->m_pUI->showOriginalComboBox->insertItem(0,"Volume");
  this->m_pUI->showOriginalComboBox->setCurrentIndex(0);


  // Set up action signals and slots
  connect(this->m_pUI->actionOpenFile, SIGNAL(triggered()), this, SLOT(slotOpenProject()));
  connect(this->m_pUI->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
  connect(this->m_pUI->frameSlider,SIGNAL(valueChanged(int)),this,SLOT(slotFrameChanged(int)));

  connect(this->m_pUI->showOriginalGroupBox,SIGNAL(toggled(bool)),this,SLOT(slotShowOriginalChanged(bool)));
  connect(this->m_pUI->showDeconvolutedGroupBox,SIGNAL(toggled(bool)),this,SLOT(slotShowDeconvolutedChanged(bool)));
  connect(this->m_pUI->showMotionFieldGroupBox,SIGNAL(toggled(bool)),this,SLOT(slotShowMotionFieldChanged(bool)));

  connect(this->m_pUI->showOriginalComboBox,SIGNAL(currentIndexChanged(QString)),SLOT(slotShowOriginalModeChanged(QString)));
  connect(this->m_pUI->showDeconvolutedComboBox,SIGNAL(currentIndexChanged(QString)),SLOT(slotShowDeconvolutedModeChanged(QString)));



  m_DeconvolutedDrawer.SetRenderer(this->m_Renderer);
  m_OriginalDrawer.SetRenderer(this->m_Renderer);
  m_MotionVolumeDrawer.SetRenderer(this->m_Renderer);

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

void DrosophilaOmmatidiaExplorer::slotShowDeconvolutedChanged(bool value){

    m_DeconvolutedDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();
}

void DrosophilaOmmatidiaExplorer::slotShowOriginalChanged(bool value){

    m_OriginalDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();
}

void DrosophilaOmmatidiaExplorer::slotShowMotionFieldChanged(bool value){

    m_MotionVolumeDrawer.SetVisibility(value);
    this->m_pUI->qvtkWidget->GetRenderWindow()->Render();
}


DrosophilaOmmatidiaExplorer::~DrosophilaOmmatidiaExplorer()
{
  // The smart pointers should clean up for up

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
    //ttt::ScalarDrawer originalDrawer;
    //ttt::ScalarDrawer deconvolutedDrawer;
    //ttt::VectorFieldDrawer deconvolutedDrawer;
    //this->Draw

    this->m_Renderer->RemoveAllViewProps();
    m_OriginalDrawer.SetImage(m_Project.GetOriginalImage(frame));
    m_OriginalDrawer.Draw();
    m_OriginalDrawer.SetVisibility(this->m_pUI->showOriginalGroupBox->isChecked());





    m_DeconvolutedDrawer.SetImage(m_Project.GetDeconvolutedImage(frame));
    m_DeconvolutedDrawer.Draw();
    m_DeconvolutedDrawer.SetVisibility(this->m_pUI->showDeconvolutedGroupBox->isChecked());

    if(frame!=this->m_Project.GetNumFrames()-1){
        typename DrosophilaOmmatidiaJSONProject::MotionImageType::Pointer motionImage = m_Project.GetMotionImage(frame);
        m_MotionVolumeDrawer.SetMotionImage(motionImage);
        m_MotionVolumeDrawer.Draw();
        m_MotionVolumeDrawer.SetVisibility(this->m_pUI->showMotionFieldGroupBox->isChecked());

        this->m_Renderer->Render();
    }else{
        m_MotionVolumeDrawer.Reset();
    }

        this->m_pUI->qvtkWidget->GetRenderWindow()->Render();


#if 0

    typedef itk::ImageToVTKImageFilter<DrosophilaOmmatidiaJSONProject::MotionImageType> MotionToVTKImageFilterType;

    MotionToVTKImageFilterType::Pointer motionToVTK = MotionToVTKImageFilterType::New();
    typename DrosophilaOmmatidiaJSONProject::MotionImageType::Pointer motionImage=m_Project.GetMotionImage(frame);
    motionToVTK->SetInput(motionImage);
    motionToVTK->Update();
    vtkSmartPointer<vtkImageData> motionField = motionToVTK->GetOutput();
    std::cout << motionField->GetNumberOfScalarComponents() << std::endl;

      // Create the structured grid.
      //vtkSmartPointer<vtkStructuredGrid> sgrid =
      //  vtkSmartPointer<vtkStructuredGrid>::New();
      //CreateData(sgrid);

      // We create a simple pipeline to display the data.
      vtkSmartPointer<vtkHedgeHog> hedgehog =
        vtkSmartPointer<vtkHedgeHog>::New();

      hedgehog->SetInputData(motionField);
      hedgehog->SetScaleFactor(0.1);

      vtkSmartPointer<vtkPolyDataMapper> sgridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      sgridMapper->SetInputConnection(hedgehog->GetOutputPort());

      vtkSmartPointer<vtkActor> sgridActor =   vtkSmartPointer<vtkActor>::New();
      sgridActor->SetMapper(sgridMapper);
      sgridActor->GetProperty()->SetColor(1.0,1.0,1.0);

      this->m_Renderer->AddActor(sgridActor);
      this->m_pUI->qvtkWidget->GetRenderWindow()->Render();
#endif
    }


// Action to be taken upon file open
void DrosophilaOmmatidiaExplorer::slotOpenProject()
{
    QFileDialog fileDialog(this);

    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setOption(QFileDialog::ShowDirsOnly);

    if(fileDialog.exec()){
        this->m_ProjectPath=fileDialog.selectedFiles().first();

        std::cout << this->m_ProjectPath.toStdString() << std::endl;

        if(this->m_Project.Open(this->m_ProjectPath.toStdString())){

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

void DrosophilaOmmatidiaExplorer::slotExit() {
  qApp->exit();
}
