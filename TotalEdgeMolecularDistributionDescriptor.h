#ifndef TOTALEDGEMOLECULARDISTRIBUTIONDESCRIPTOR_H
#define TOTALEDGEMOLECULARDISTRIBUTIONDESCRIPTOR_H
#include <itkObject.h>
#include <itkLevelSet.h>
#include <queue>
#include <utility>
#include <itkArray.h>

#include "SeedBasedMolecularDistributionDescriptor.h"
#include "FeatureContainer.h"

template<class TAJGraph, class TMolecularImage> class TotalEdgeMolecularDistributionDescriptor : public SeedBasedMolecularDistributionDescriptor<TMolecularImage>{
public:

	typedef TotalEdgeMolecularDistributionDescriptor Self;
	typedef SeedBasedMolecularDistributionDescriptor Superclass;
	typedef itk::SmartPointer<Self> Pointer;


	typedef TMolecularImage MolecularImageType;
	typedef TAJGraph AJGraphType;

	//typedef typename AJGraphType::AJEdgeType AJEdgeType;
	typedef typename AJGraphType::AJEdgeHandler AJEdgeHandler;
	typedef  typename Superclass::PointType PointType;
	typedef  typename TMolecularImage::IndexType IndexType;
	typedef  typename TMolecularImage::IndexType OffsetType;

	virtual bool PointInRegion(const PointType & point);

	virtual void Compute();


	itkNewMacro(Self)

		itkGetObjectMacro(AJGraph, AJGraphType)
		itkSetObjectMacro(AJGraph, AJGraphType)

		itkGetMacro(Radius, double)
		itkSetMacro(Radius, double)

protected:

	TotalEdgeMolecularDistributionDescriptor();


	~TotalEdgeMolecularDistributionDescriptor();

private:


	typename AJGraphType::Pointer m_AJGraph;

	double m_Radius;

	PointType m_Point0;
	PointType m_Point1;

};

#include "TotalEdgeMolecularDistributionDescriptor.hxx"
#endif // VERTEXMOLECULARDISTRIBUTIONDESCRIPTOR_H
