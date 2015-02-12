/** \addtogroup TTTDrawers
 *  @{
 */
#ifndef DIFFUSED_IMAGE_DRAWER_H
#define DIFFUSED_IMAGE_DRAWER_H


#include "ScalarVolumeDrawer.h"
#include "Drawer.h"
namespace ttt{
/**
 * \brief Draws a \link DiffusedImageDrawer::DiffusedImageType \endlink image into the renderer
 */
class DeconvolutedImageDrawer : public ScalarVolumeDrawer<itk::Image<double,3> > {

    typedef itk::Image<double,3> DeconvolutedImageType;

protected:
	virtual vtkSmartPointer<vtkVolumeProperty> GetVolumeProperty();
};
}
#endif
/** @}*/
