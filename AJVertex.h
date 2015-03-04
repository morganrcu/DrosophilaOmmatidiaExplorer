#ifndef AJVERTEX_H
#define AJVERTEX_H

#include <itkDataObject.h>
#include <itkMacro.h>
#include <itkObjectFactory.h>
#include <itkPoint.h>
class AJVertex : public itk::DataObject{

public:
    typedef AJVertex Self;
    typedef itk::DataObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;

    typedef itk::Point<double,3> PointType;
    typedef itk::Vector<double,3> VectorType;
    typedef itk::Array<double> DescriptorType;

    itkNewMacro(Self)

    itkGetMacro(Position,PointType)
    itkSetMacro(Position,PointType)

    itkGetMacro(Descriptor,DescriptorType)
    itkSetMacro(Descriptor,DescriptorType)

    itkGetMacro(Velocity,VectorType)
    itkSetMacro(Velocity,VectorType)



protected:
	AJVertex(){
		m_Position.Fill(0);
		m_Velocity.Fill(0);

	}
private:

    PointType m_Position;
    DescriptorType m_Descriptor;
    VectorType m_Velocity;


};

#endif // AJVERTEX_H
