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

    itkNewMacro(Self)

    itkGetMacro(Position,PointType)
    itkSetMacro(Position,PointType)



protected:

private:

    PointType m_Position;

};

#endif // AJVERTEX_H
