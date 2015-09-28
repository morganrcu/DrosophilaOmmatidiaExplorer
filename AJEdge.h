#ifndef AJEDGE_H
#define AJEDGE_H

#include <itkDataObject.h>
#include <itkMacro.h>
#include <itkObjectFactory.h>
#include <itkPoint.h>
template<class TCellVertex> class AJEdge : public itk::DataObject{

public:
    typedef AJEdge Self;
    typedef itk::DataObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;

    typedef itk::Array<double> DescriptorType;

    itkNewMacro(Self)


    itkGetMacro(Descriptor,DescriptorType)
    itkSetMacro(Descriptor,DescriptorType)

	void SetCellPair(const std::pair<TCellVertex,TCellVertex> & cellPair){
		m_Cells=cellPair;
	}
	std::pair<TCellVertex,TCellVertex> GetCellPair(){
		return m_Cells;
	}

protected:
	AJEdge(){


	}

private:


    DescriptorType m_Descriptor;

    std::pair<TCellVertex,TCellVertex> m_Cells;

};

#endif // AJVERTEX_H
