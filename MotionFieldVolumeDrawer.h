#ifndef MOTIONFIELDVOLUMEDRAWER_H
#define MOTIONFIELDVOLUMEDRAWER_H


#include <Drawer.h>
#include <itkImageToVTKImageFilter.h>
#include <vtkHedgeHog.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkGlyph3D.h>
#include <vtkArrowSource.h>

template<class TMotionImage> class MotionFieldVolumeDrawer : public ttt::Drawer{

private:

    vtkSmartPointer<vtkVolume> m_Volume;

    vtkSmartPointer<vtkPolyDataMapper> m_VolumeMapper;
    bool m_Drawn;


protected:

public:
    typedef TMotionImage MotionImageType;
    MotionFieldVolumeDrawer(){
        m_Drawn=false;
        m_SgridActor=NULL;
    }

    inline void SetMotionImage(typename MotionImageType::Pointer & motionImage){
        m_MotionImage=motionImage;
    }

    virtual void Reset(){
        if(this->m_Drawn){
            this->m_Renderer->RemoveActor(m_SgridActor);
        }
    }

    virtual void Draw(){

        this->Reset();
#if 0
        typedef itk::ImageToVTKImageFilter<MotionImageType> MotionToVTKImageFilterType;

        typename MotionToVTKImageFilterType::Pointer motionToVTK = MotionToVTKImageFilterType::New();

        motionToVTK->SetInput(m_MotionImage);
        motionToVTK->Update();

        vtkSmartPointer<vtkImageData> motionField = motionToVTK->GetOutput();
#endif
        vtkSmartPointer<vtkDoubleArray> vec = vtkSmartPointer<vtkDoubleArray>::New();

        vec->SetName("v1");
        vec->SetNumberOfComponents(3);
        vec->SetNumberOfTuples(m_MotionImage->GetLargestPossibleRegion().GetSize(0)*m_MotionImage->GetLargestPossibleRegion().GetSize(1)*m_MotionImage->GetLargestPossibleRegion().GetSize(2));

        vtkSmartPointer<vtkPoints> points = vtkPoints::New();

        points -> Allocate(m_MotionImage->GetLargestPossibleRegion().GetSize(0)*m_MotionImage->GetLargestPossibleRegion().GetSize(1)*m_MotionImage->GetLargestPossibleRegion().GetSize(2));

        int index=0;
        for(int k = 0; k< m_MotionImage->GetLargestPossibleRegion().GetSize(2); k++) {               // assign dummy vectors (1,1,1)
            for(int j = 0; j<m_MotionImage->GetLargestPossibleRegion().GetSize(1); j=j+10){
                for(int i = 0; i< m_MotionImage->GetLargestPossibleRegion().GetSize(0); i=i+10){

                    float x[3];
                    float v[3];

                    x[0]=i*m_MotionImage->GetSpacing()[0];
                    x[1]=j*m_MotionImage->GetSpacing()[1];
                    x[2]=k*m_MotionImage->GetSpacing()[2];
                    typename MotionImageType::IndexType indexImage;
                    indexImage[0]=i;
                    indexImage[1]=j;
                    indexImage[2]=k;

                    v[0]= m_MotionImage->GetPixel(indexImage)[0];
                    v[1]= m_MotionImage->GetPixel(indexImage)[1];
                    v[2]= m_MotionImage->GetPixel(indexImage)[2];

                    //std::cout << v[0] << " " << v[1] << " " << v[2] << std::endl;

                    points -> InsertPoint(index,x);
                    vec -> InsertTuple(index,v);
                    index++;
                }
            }
        }

        vtkSmartPointer<vtkStructuredGrid> sgrid = vtkStructuredGrid::New();

        sgrid->SetPoints(points);
        sgrid -> GetPointData() -> SetVectors( vec );



#if 0
          // We create a simple pipeline to display the data.
          vtkSmartPointer<vtkHedgeHog> hedgehog =
            vtkSmartPointer<vtkHedgeHog>::New();

          hedgehog->SetInputData(sgrid);
          hedgehog->SetScaleFactor(0.1);
#endif
        vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();

        arrowSource->SetTipResolution(6);
        arrowSource->SetTipRadius(.1);
        arrowSource->SetTipLength(.35);
        arrowSource->SetShaftResolution(6);
        arrowSource->SetShaftRadius(0.03);

          vtkSmartPointer<vtkGlyph3D> glyph3D =
            vtkSmartPointer<vtkGlyph3D>::New();

          glyph3D->SetInputData(sgrid);
          glyph3D->SetSourceConnection(arrowSource->GetOutputPort());
          glyph3D->SetVectorModeToUseVector();
          glyph3D->SetColorModeToColorByScalar();
          glyph3D->SetScaleModeToDataScalingOff();
          glyph3D->OrientOn();
          glyph3D->SetScaleFactor(0.002);


          vtkSmartPointer<vtkPolyDataMapper> sgridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      //    sgridMapper->SetInputConnection(hedgehog->GetOutputPort());
          sgridMapper->SetInputConnection(glyph3D->GetOutputPort());
          sgridMapper->ScalarVisibilityOn();

          m_SgridActor =   vtkSmartPointer<vtkActor>::New();
          m_SgridActor->SetMapper(sgridMapper);
          m_SgridActor->GetProperty()->SetColor(1.0,1.0,1.0);

          this->m_Renderer->AddActor(m_SgridActor);
          this->m_Drawn=true;

    }

    virtual void Show(){
        if(m_Drawn) m_SgridActor->VisibilityOn();
    }

    virtual void Hide(){
        if(m_Drawn) m_SgridActor->VisibilityOff();
    }
private:

    typename MotionImageType::Pointer m_MotionImage;
    vtkSmartPointer<vtkActor> m_SgridActor;
};

#endif // MOTIONFIELDVOLUMEDRAWER_H
