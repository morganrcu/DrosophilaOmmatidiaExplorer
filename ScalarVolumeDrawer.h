/*
 * ScalarVolumeDrawer.h
 *
 *  Created on: Dec 9, 2013
 *      Author: morgan
 */

#ifndef SCALARVOLUMEDRAWER_H_
#define SCALARVOLUMEDRAWER_H_
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include "Drawer.h"
#include <vtkImagePlaneWidget.h>
#include <vtkLookupTable.h>
#include <vtkDoubleArray.h>
#include <vtkRenderWindow.h>


namespace ttt {

template<class TImage> class ScalarVolumeDrawer : public Drawer{

public:
	enum RenderingMode{VOLUME,SLICE};
protected:
        typename TImage::Pointer m_Image;
private:

    RenderingMode m_RenderingMode;
    RenderingMode m_DrawnMode;

	vtkSmartPointer<vtkVolume> m_Volume;

	typedef itk::ImageToVTKImageFilter<TImage> ImageToVTKType;
	typename ImageToVTKType::Pointer m_ToVTK;

	vtkSmartPointer<vtkSmartVolumeMapper> m_VolumeMapper;
	vtkSmartPointer<vtkImagePlaneWidget> m_PlaneWidget;
	bool m_Drawn;
	int m_Slice;
public:
	ScalarVolumeDrawer(){
		m_Image=0;
		m_Slice=0;
		m_Drawn=false;
	}
	virtual void SetSlice(int slice){
		m_Slice=slice;
	}
	virtual ~ScalarVolumeDrawer(){

	}
	/**
	 * Sets the pointer to the diffusedImage to be drawn.
	 * You need to call this method before \link DiffusedImageDrawer::Draw() \endlink
	 * @param diffusedImage The volume to draw
	 */
	inline void SetImage(const typename TImage::Pointer & image){
		m_Image=image;
		m_RenderingMode=VOLUME;
	}

	void Reset(){
		if(m_Drawn){
			switch(m_DrawnMode ){
			case VOLUME:
				m_Renderer->RemoveViewProp(m_Volume);
				break;
			case SLICE:
				this->m_PlaneWidget->Off();
			}

		}
	}

	void SetRenderingMode(const RenderingMode & mode){
		this->m_RenderingMode=mode;
	}
	void Draw(){
		assert(m_Image);

		Reset();

		m_ToVTK = ImageToVTKType::New();

		m_ToVTK->SetInput(m_Image);
		m_ToVTK->Update();

		switch(this->m_RenderingMode){
		case SLICE: {

			m_PlaneWidget=vtkSmartPointer<vtkImagePlaneWidget>::New();
			m_PlaneWidget->DisplayTextOn();
			m_PlaneWidget->SetInputData(m_ToVTK->GetOutput());
			m_PlaneWidget->SetPlaneOrientationToZAxes();
			m_PlaneWidget->SetSliceIndex(m_Slice);
			m_PlaneWidget->SetInteractor(this->m_Renderer->GetRenderWindow()->GetInteractor());

	        typedef itk::MinimumMaximumImageCalculator<TImage> MinimumMaximumType;
			typename MinimumMaximumType::Pointer minimumMaximum = MinimumMaximumType::New();
	        minimumMaximum->SetImage(this->m_Image);
	        minimumMaximum->Compute();

	        double minimum = minimumMaximum->GetMinimum();
	        double maximum = minimumMaximum->GetMaximum();



			vtkSmartPointer<vtkLookupTable> colorMap = vtkSmartPointer<vtkLookupTable>::New(); // hot color map

			colorMap->SetRange( 0.0, maximum );
			//colorMap->SetHueRange( 0.0, 0.1 );
			//colorMap->SetValueRange( 0.4, 0.8 );
			colorMap->Build();

			m_PlaneWidget->SetLookupTable(colorMap);
			m_Drawn=true;
			m_DrawnMode=SLICE;
			break;
		}


		case VOLUME: {
			m_Volume = vtkSmartPointer<vtkVolume>::New();
			m_VolumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
			m_VolumeMapper->SetInputData(m_ToVTK->GetOutput());
			m_VolumeMapper->Update();

			m_VolumeMapper->SetBlendModeToComposite();

			m_Volume->SetMapper(m_VolumeMapper);
			m_Volume->SetProperty(this->GetVolumeProperty());
			m_Volume->SetPickable(false);
			m_Volume->VisibilityOff();
			m_Renderer->AddViewProp(m_Volume);
			m_Drawn=true;
			m_DrawnMode=VOLUME;
			break;
		}
		}





	}

	void Show(){
		if(m_Drawn){
			switch(m_DrawnMode ){
					case VOLUME:
						m_Volume->VisibilityOn();
						break;
					case SLICE:
						m_PlaneWidget->On();
			}
		}
	}
	void Hide(){
		if(m_Drawn){
			switch(m_DrawnMode ){
								case VOLUME:
									m_Volume->VisibilityOff();
									break;
								case SLICE:
									m_PlaneWidget->Off();
						}
		}

	}
protected:
	virtual vtkSmartPointer<vtkVolumeProperty> GetVolumeProperty()=0;
};

}

#endif /* SCALARVOLUMEDRAWER_H_ */
