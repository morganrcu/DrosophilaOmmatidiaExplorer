/** \addtogroup TTTDrawers
 *  @{
 */
#ifndef DIFFUSED_IMAGE_DRAWER_H
#define DIFFUSED_IMAGE_DRAWER_H


#include "ScalarVolumeDrawer.h"
#include "Drawer.h"
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <itkMinimumMaximumImageCalculator.h>
namespace ttt{
/**
 * \brief Draws a \link DiffusedImageDrawer::DiffusedImageType \endlink image into the renderer
 */
class DeconvolutedImageDrawer : public ScalarVolumeDrawer<itk::Image<double,3> > {

    typedef itk::Image<double,3> DeconvolutedImageType;

public:
    DeconvolutedImageDrawer(){

    }

protected:
    virtual vtkSmartPointer<vtkVolumeProperty> GetVolumeProperty(){
        typedef itk::MinimumMaximumImageCalculator<DeconvolutedImageType> MinimumMaximumType;

        MinimumMaximumType::Pointer minimumMaximum = MinimumMaximumType::New();
        minimumMaximum->SetImage(this->m_Image);
        minimumMaximum->Compute();

        double minimum = minimumMaximum->GetMinimum();
        double maximum = minimumMaximum->GetMaximum();
        double range = maximum - minimum;

        vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
        volumeProperty->ShadeOff();
        volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

        vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();

        compositeOpacity->AddPoint(minimum, 0.0);
        compositeOpacity->AddPoint(minimum+0.3*range, 1.0);
        //compositeOpacity->AddPoint(minimumMaximum->GetMinimum()+0.31minimumMaximum->GetMaximum(), 1.0);
        compositeOpacity->AddPoint(maximum, 1.0);

        volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.

        vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();

        color->AddRGBPoint(minimumMaximum->GetMinimum(), 0.0, 0.0, 1.0);
        color->AddRGBPoint(minimum +0.5*range, 1.0, 0.0, 0.0);
        color->AddRGBPoint(maximum, 1.0, 1.0, 1.0);

        volumeProperty->SetColor(color);
        return volumeProperty;
    }
};
}
#endif
/** @}*/