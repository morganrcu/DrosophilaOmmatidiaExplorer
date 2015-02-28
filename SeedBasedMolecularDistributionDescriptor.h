#ifndef SEEDBASEDMOLECULARDISTRIBUTIONDESCRIPTOR_H_
#define SEEDBASEDMOLECULARDISTRIBUTIONDESCRIPTOR_H_
#include <itkObject.h>
#include <itkLevelSet.h>
#include <queue>
#include <utility>
#include <itkArray.h>

template<class TMolecularImage> class SeedBasedMolecularDistributionDescriptor : public itk::Object{
public:
	typedef SeedBasedMolecularDistributionDescriptor Self;
	typedef itk::Object Superclass;
	typedef itk::SmartPointer<Self> Pointer;

	typedef TMolecularImage MolecularImageType;




	typedef unsigned short ClusterDescriptor;
	virtual void Compute()=0;

	typedef itk::Array<double> DescriptorType;

	itkGetObjectMacro(MolecularImage,MolecularImageType)
    itkSetObjectMacro(MolecularImage,MolecularImageType)

protected:
	typedef typename MolecularImageType::PointType PointType;
	typedef typename MolecularImageType::IndexType IndexType;

	virtual void SetSeed(const IndexType & index, const ClusterDescriptor & cluster);
	virtual void SetBanned(const IndexType & index);
	SeedBasedMolecularDistributionDescriptor();
	~SeedBasedMolecularDistributionDescriptor();

	virtual DescriptorType ComputeDescriptor();

	virtual void Init();
	virtual void Reset();
	typename MolecularImageType::Pointer m_MolecularImage;
	virtual bool PointInRegion(const PointType & point)=0;

	itkGetMacro(NumberOfClusters,unsigned short)
	itkSetMacro(NumberOfClusters,unsigned short)

private:

	enum LabelType { FarPoint = 0, AlivePoint, VisitedPoint, InitialTrialPoint, OutsidePoint };
    typedef itk::Image<LabelType,TMolecularImage::ImageDimension> LabelImageType;
    typedef itk::Image<unsigned short,TMolecularImage::ImageDimension> ClusterImageType;
    typedef itk::Image<double,TMolecularImage::ImageDimension> LevelSetImageType;

    typedef itk::LevelSetTypeDefault< LevelSetImageType >            LevelSetType;
    typedef typename LevelSetType::LevelSetPointer      		  LevelSetPointer;
    typedef typename LevelSetType::PixelType            		  LevelSetPixelType;
    typedef typename LevelSetType::NodeType             		  LevelSetNodeType;

    typedef typename LevelSetNodeType::IndexType                NodeIndexType;
    typedef typename LevelSetType::NodeContainer        NodeContainer;
    typedef typename LevelSetType::NodeContainerPointer NodeContainerPointer;



    typename LabelImageType::Pointer m_LabelImage;
    typename LevelSetImageType::Pointer m_LevelSetImage;
    typename ClusterImageType::Pointer m_ClusterImage;


    /** Trial points are stored in a min-heap. This allow efficient access
     * to the trial point with minimum value which is the next grid point
     * the algorithm processes. */
    typedef std::deque< LevelSetNodeType >  HeapContainer;
    typedef std::greater< LevelSetNodeType > NodeComparer;

    typedef std::priority_queue< LevelSetNodeType, HeapContainer, NodeComparer > HeapType;

    HeapType m_TrialHeap;

    unsigned short m_NumberOfClusters;



};
#include "SeedBasedMolecularDistributionDescriptor.hxx"
#endif
