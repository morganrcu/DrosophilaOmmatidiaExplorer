#ifndef VERTEXMOLECULARDISTRIBUTIONDESCRIPTOR_H
#define VERTEXMOLECULARDISTRIBUTIONDESCRIPTOR_H
#include <itkObject.h>
#include <itkLevelSet.h>
#include <queue>
#include <utility>
#include <itkArray.h>

#include "SeedBasedMolecularDistributionDescriptor.h"
#include "FeatureContainer.h"
template<class TAJGraph,class TMolecularImage> class VertexMolecularDistributionDescriptor : public SeedBasedMolecularDistributionDescriptor<TMolecularImage>{
public:

    typedef VertexMolecularDistributionDescriptor Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer<Self> Pointer;


    typedef TMolecularImage MolecularImageType;
    typedef TAJGraph AJGraphType;

    typedef typename AJGraphType::AJVertexType AJVertexType;
    typedef typename AJGraphType::AJVertexHandler AJVertexHandler;
    typedef  typename AJVertexType::PointType PointType;
    typedef  typename TMolecularImage::IndexType IndexType;
    typedef  typename TMolecularImage::IndexType OffsetType;



    virtual bool PointInRegion(const PointType & point);

    virtual void Compute();



    itkNewMacro(Self)

    itkGetObjectMacro(AJGraph,AJGraphType)
    itkSetObjectMacro(AJGraph,AJGraphType)

    itkGetMacro(Radius,double)
    itkSetMacro(Radius,double)


protected:

    VertexMolecularDistributionDescriptor();


    ~VertexMolecularDistributionDescriptor();

private:


    typename AJGraphType::Pointer m_AJGraph;


    double m_Radius;

    PointType m_CurrentOrigin;


};

#include "VertexMolecularDistributionDescriptor.hxx"
#endif // VERTEXMOLECULARDISTRIBUTIONDESCRIPTOR_H
