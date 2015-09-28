#ifndef AJVERTEX_H
#define AJVERTEX_H

#include <itkDataObject.h>
#include <itkMacro.h>
#include <itkObjectFactory.h>
#include <itkPoint.h>
template<int dim=3,class TCellVertex=unsigned long > class AJVertex : public itk::DataObject{

public:
    typedef AJVertex Self;
    typedef itk::DataObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;

    typedef itk::Point<double,dim> PointType;
    typedef itk::Point<double,dim> SphericalPointType;
    typedef itk::Vector<double,dim> VectorType;
    typedef itk::Array<double> DescriptorType;
    typedef std::set<TCellVertex> CellsContainer;
    typedef typename CellsContainer::iterator CellsIterator;
    itkNewMacro(Self)

    itkGetMacro(EmbeddedSphericalPosition,SphericalPointType)
	itkSetMacro(EmbeddedSphericalPosition,SphericalPointType)

	itkGetMacro(EmbeddedPosition,PointType)
	itkSetMacro(EmbeddedPosition,PointType)

    itkGetMacro(Position,PointType)
    itkSetMacro(Position,PointType)

    itkGetMacro(Descriptor,DescriptorType)
    itkSetMacro(Descriptor,DescriptorType)

    itkGetMacro(Velocity,VectorType)
    itkSetMacro(Velocity,VectorType)


	void AddCell(const TCellVertex & vertex){
    	m_Cells.insert(vertex);
    }

    CellsIterator BeginCells(){
    	return m_Cells.begin();
    }
    CellsIterator EndCells(){
    	return m_Cells.end();
    }
protected:
	AJVertex(){
		m_Position.Fill(0);
		m_Velocity.Fill(0);
		m_EmbeddedPosition.Fill(0);
		m_EmbeddedSphericalPosition.Fill(0);
	}
private:
	SphericalPointType m_EmbeddedSphericalPosition;
	PointType m_EmbeddedPosition;

    PointType m_Position;
    DescriptorType m_Descriptor;
    VectorType m_Velocity;

    CellsContainer m_Cells;

};

#endif // AJVERTEX_H
