#ifndef AJEDGE_H
#define AJEDGE_H

#include <itkDataObject.h>
#include <itkMacro.h>
#include <itkObjectFactory.h>
#include <itkPoint.h>
class AJEdge : public itk::DataObject{

public:
    typedef AJEdge Self;
    typedef itk::DataObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;

    typedef itk::Array<double> DescriptorType;

    itkNewMacro(Self)


    itkGetMacro(Descriptor,DescriptorType)
    itkSetMacro(Descriptor,DescriptorType)



protected:
	AJEdge(){


	}
private:


    DescriptorType m_Descriptor;


};

#endif // AJVERTEX_H
