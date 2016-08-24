/*
 * tttEnergyModelTissueSmoother.h
 *
 *  Created on: Oct 28, 2015
 *      Author: morgan
 */

#ifndef TTTENERGYMODELTISSUESMOOTHER_H_
#define TTTENERGYMODELTISSUESMOOTHER_H_
#include <itkLinearInterpolateImageFunction.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <itkLevelSet.h>
#include <queue>
#include <map>

template<class TVelocityImage> class PathCost{

public:

	typedef TVelocityImage ImageType;
	typedef typename ImageType::PointType PointType;
	typedef typename ImageType::IndexType IndexType;

	typedef typename ImageType::PointType::VectorType VectorType;

public:
	void Do(){
		if(this->m_SourceIndex==this->m_TargetIndex){
			m_Cost=0;
			m_Proximal=m_SourceIndex;
			return;
		}
		if(m_ProximalCache.find(m_SourceIndex)!=m_ProximalCache.end() && m_ProximalCache[m_SourceIndex].find(m_TargetIndex)!=m_ProximalCache[m_SourceIndex].end()){
			m_Proximal=m_ProximalCache[m_SourceIndex][m_TargetIndex];
			m_Cost=m_CostCache[m_SourceIndex][m_TargetIndex];
			return;
		}


		this->m_LabelImage->FillBuffer(FarPoint);
		this->m_LevelSetImage->FillBuffer(itk::NumericTraits<double>::max());
		this->m_LabelImage->SetPixel(this->m_SourceIndex,InitialTrialPoint);
		this->m_LevelSetImage->SetPixel(this->m_SourceIndex,0);


	    LevelSetNodeType node;
	    node.SetIndex(this->m_SourceIndex);
	    node.SetValue(0);

		this->m_TrialHeap.push(node);

	    double       centerValue;
	    double       oldProgress = 0;

	    std::list<IndexType> visitedRegion;
	    // CACHE
	    while ( !m_TrialHeap.empty() )
	      {
	      // get the node with the smallest value
	      node = m_TrialHeap.top();
	      m_TrialHeap.pop();



	      IndexType centerIndex = node.GetIndex();
	      PointType centerPoint;
	      this->m_LabelImage->TransformIndexToPhysicalPoint(centerIndex,centerPoint);

	      // does this node contain the current value ?
	      centerValue = static_cast< double >( m_LevelSetImage->GetPixel( node.GetIndex() ) );
	      if(centerValue!=node.GetValue()){

	    	  continue;
	      }

	      if(centerIndex==m_TargetIndex){
	    	  m_Cost=centerValue;
	    	  break;
	      }

	      auto centerSpeed = m_InputImage->GetPixel(centerIndex);

	      double vmin=1e-3;
	      if ( m_LabelImage->GetPixel( node.GetIndex() ) == AlivePoint ||  m_LabelImage->GetPixel( node.GetIndex() ) == InitialTrialPoint ){
	          // set this node as alive
	          m_LabelImage->SetPixel(node.GetIndex(), VisitedPoint);

	          unsigned char label;

	          assert(m_LabelImage->GetBufferedRegion().IsInside(centerIndex));

	          itk::Offset<3> offset;
	          for(offset[0] =-1;offset[0]<2;offset[0]++){
	            for(offset[1] =-1;offset[1]<2;offset[1]++){
	                for(offset[2] =-1;offset[2]<2;offset[2]++){
	                    if(offset[0]==0 && offset[1]==0 && offset[2]==0) continue;

	                    IndexType neighIndex= centerIndex +offset;
	                    if(!m_InputImage->GetBufferedRegion().IsInside(neighIndex)) continue;

	                    if(this->m_LabelImage->GetPixel(neighIndex)==FarPoint || this->m_LabelImage->GetPixel(neighIndex)==AlivePoint ){
	                        PointType neighPoint;

	                        this->m_InputImage->TransformIndexToPhysicalPoint(neighIndex,neighPoint);

	                        auto neighSpeed= this->m_InputImage->GetPixel(neighIndex);
	                        //double newValue =centerValue + 1.0/(0.00000001+(neighSpeed+centerSpeed/2);
	                        double newValue = centerValue + 2*(centerPoint-neighPoint).GetNorm()/(vmin+neighSpeed+centerSpeed);
	                        double oldValue =this->m_LevelSetImage->GetPixel(neighIndex);

	                        if(newValue<oldValue){
	                        	this->m_Origins->SetPixel(neighIndex,centerIndex);
	                        	this->m_LabelImage->SetPixel(neighIndex,AlivePoint);
	                        	this->m_LevelSetImage->SetPixel(neighIndex,newValue);

	                        	LevelSetNodeType node;
	                        	node.SetIndex(neighIndex);
	                        	node.SetValue(newValue);
	                        	this->m_TrialHeap.push(node);
                       		}
	                    }
	                }
	            }
	          }
	      }
	    }

        PointType targetPoint,previousPoint;
        m_InputImage->TransformIndexToPhysicalPoint(m_TargetIndex,targetPoint);
        m_InputImage->TransformIndexToPhysicalPoint(m_Origins->GetPixel(m_TargetIndex),previousPoint);


        double proxValue= m_Cost;

        bool proxFound=false;


        m_Proximal=m_TargetIndex;


        while(!proxFound){
        	proxFound=true;
        	auto currentIndex = m_Proximal;
        	itk::Offset<3> offset;
        	for(offset[0] =-1;offset[0]<2;offset[0]++){
        		for(offset[1] =-1;offset[1]<2;offset[1]++){
        			for(offset[2] =-1;offset[2]<2;offset[2]++){
        				if(offset[0]==0 && offset[1]==0 && offset[2]==0) continue;
        				auto neighIndex= currentIndex+offset;
        				if(m_LevelSetImage->GetLargestPossibleRegion().IsInside(neighIndex)){

            				PointType neighPoint;
            		        m_InputImage->TransformIndexToPhysicalPoint(neighIndex,neighPoint);

        					auto neighProximalCost = m_LevelSetImage->GetPixel(neighIndex) + 0.5*(neighPoint-targetPoint).GetSquaredNorm();

        					if(neighProximalCost<proxValue){
        						proxValue=neighProximalCost;
        						proxFound=false;
        						m_Proximal=neighIndex;
        					}
        				}
        			}
        		}
        	}
        }
        //std::cout << m_TargetIndex << " " << m_Proximal << std::endl;

        //auto vector = targetPoint - previousPoint;
        //auto norm = vector.GetNorm();
        //m_TargetGradient[0]=(m_LevelSetImage->GetPixel(m_TargetIndex)-m_LevelSetImage->GetPixel(m_Origins->GetPixel(m_TargetIndex)))/(targetPoint[0]-previousPoint[0]);
        //m_TargetGradient[1]=(m_LevelSetImage->GetPixel(m_TargetIndex)-m_LevelSetImage->GetPixel(m_Origins->GetPixel(m_TargetIndex)))/(targetPoint[1]-previousPoint[1]);
        //m_TargetGradient[2]=(m_LevelSetImage->GetPixel(m_TargetIndex)-m_LevelSetImage->GetPixel(m_Origins->GetPixel(m_TargetIndex)))/(targetPoint[2]-previousPoint[2]);

        //auto targetValue = m_LevelSetImage->GetPixel(m_TargetIndex);
        //auto previousValue = m_LevelSetImage->GetPixel(m_Origins->GetPixel(m_TargetIndex));
        //m_Proximal=m_Origins->GetPixel(m_TargetIndex);
        //m_TargetGradient=vector*(targetValue-previousValue)/norm;

        //if(m_TargetGradient[0]>10){
        //	std::cout << m_TargetGradient << std::endl;
        //}
        m_ProximalCache[m_SourceIndex][m_TargetIndex]=m_Proximal;
        m_CostCache[m_SourceIndex][m_TargetIndex]=m_Cost;

	}
	void SetInputImage(const typename ImageType::Pointer & input){
		m_InputImage=input;
	}
	void SetSourceIndex(const IndexType & index){
		m_SourceIndex=index;
	}
	void SetTargetIndex(const IndexType & index){
		m_TargetIndex=index;
	}
	double GetCost(){
		return m_Cost;
	}
	IndexType GetProximal(){
		return m_Proximal;
	}
#if 0
	VectorType GetTargetGradient(){
		return m_TargetGradient;
	}
#endif
public:
	void Init(){
	    this->m_LabelImage=LabelImageType::New();
	    this->m_LabelImage->SetRegions(this->m_InputImage->GetLargestPossibleRegion());
	    this->m_LabelImage->CopyInformation(this->m_InputImage);
	    this->m_LabelImage->Allocate();


	    this->m_LevelSetImage=LevelSetImageType::New();
	    this->m_LevelSetImage->CopyInformation(this->m_InputImage);
	    this->m_LevelSetImage->SetRegions(this->m_InputImage->GetLargestPossibleRegion());
	    this->m_LevelSetImage->Allocate();

	    this->m_Origins=OriginImageType::New();
	    this->m_Origins->CopyInformation(this->m_InputImage);
	    this->m_Origins->SetRegions(this->m_InputImage->GetLargestPossibleRegion());
	    this->m_Origins->Allocate();


	}

private:

	typename ImageType::Pointer m_InputImage;
	IndexType m_SourceIndex;
	IndexType m_TargetIndex;
	std::vector<IndexType> m_PendingTargets;
	VectorType m_TargetGradient;
	typedef itk::Image<IndexType,ImageType::ImageDimension> OriginImageType;
	typename OriginImageType::Pointer m_Origins;

	double m_Cost;


	enum LabelType { FarPoint = 0, AlivePoint, VisitedPoint, InitialTrialPoint, OutsidePoint };
    typedef itk::Image<LabelType,ImageType::ImageDimension> LabelImageType;
    typedef itk::Image<double,ImageType::ImageDimension> LevelSetImageType;

    typedef itk::LevelSetTypeDefault< LevelSetImageType >            LevelSetType;
    typedef typename LevelSetType::LevelSetPointer      		  LevelSetPointer;
    typedef typename LevelSetType::PixelType            		  LevelSetPixelType;
    typedef typename LevelSetType::NodeType             		  LevelSetNodeType;

    typedef typename LevelSetNodeType::IndexType                NodeIndexType;
    typedef typename LevelSetType::NodeContainer        NodeContainer;
    typedef typename LevelSetType::NodeContainerPointer NodeContainerPointer;

    typename LabelImageType::Pointer m_LabelImage;
    typename LevelSetImageType::Pointer m_LevelSetImage;



    /** Trial points are stored in a min-heap. This allow efficient access
     * to the trial point with minimum value which is the next grid point
     * the algorithm processes. */
    typedef std::deque< LevelSetNodeType >  HeapContainer;
    typedef std::greater< LevelSetNodeType > NodeComparer;

    typedef std::priority_queue< LevelSetNodeType, HeapContainer, NodeComparer > HeapType;

    HeapType m_TrialHeap;

    IndexType m_Proximal;

    std::map<IndexType,std::map<IndexType,IndexType,typename IndexType::LexicographicCompare>,typename IndexType::LexicographicCompare > m_ProximalCache;
    std::map<IndexType,std::map<IndexType,double,typename IndexType::LexicographicCompare>,typename IndexType::LexicographicCompare > m_CostCache;
};


template<class TIterator, class TAJGraph> std::pair<itk::Vector<double, 3>, itk::Point<double, 3> > embeddingPlane(const TIterator & beginVertices, const TIterator & endVertices, const typename TAJGraph::Pointer & ajGraph){

	int total = 0;
	typedef itk::Point<double, 3> PointType;
	std::vector<PointType> points, centeredPoints;
	PointType meanPoint;
	meanPoint.Fill(0);
	for (auto vertexIt = beginVertices; vertexIt != endVertices; ++vertexIt) {
		itk::Point<double, 3> position = ajGraph->GetAJVertex(*vertexIt)->GetPosition();
		points.push_back(position);
		meanPoint[0] += position[0];
		meanPoint[1] += position[1];
		meanPoint[2] += position[2];
		total++;
	}

	meanPoint[0] /= total;
	meanPoint[1] /= total;
	meanPoint[2] /= total;

	//std::cout << "MeanPoint " << meanPoint << std::endl;
	vnl_matrix<double> A(total, 3);

	int row = 0;

	for (std::vector<PointType>::iterator pointIt = points.begin(); pointIt != points.end(); ++pointIt) {
		PointType diff = *pointIt - meanPoint;
		centeredPoints.push_back(diff);
		A(row, 0) = diff[0];
		A(row, 1) = diff[1];
		A(row, 2) = diff[2];
		row++;
	}
	//std::cout << row << std::endl;
	//assert(row>1);

	vnl_matrix_fixed<double, 3, 3> S = A.transpose() * A; //FEATURE: Scatter Matrix

	vnl_symmetric_eigensystem<double> eigensystem(S);

	vnl_vector_fixed<double, 3> normalPlane = eigensystem.get_eigenvector(0); //FEATURE: Supporting plane
	itk::Vector<double, 3> normal;
	normal.SetVnlVector(normalPlane);
	return std::make_pair(normal, meanPoint);

}

template<class TTissue> typename TTissue::Pointer cloneTissueDescriptor(const typename TTissue::Pointer & inputTissue){

	typename TTissue::Pointer result = TTissue::New();

	typename TTissue::AJGraphType::Pointer resultAJgraph = TTissue::AJGraphType::New();

	for (auto vertexIterator = inputTissue->GetAJGraph()->VerticesBegin(); vertexIterator != inputTissue->GetAJGraph()->VerticesEnd(); ++vertexIterator++){
		auto vertex = inputTissue->GetAJGraph()->GetAJVertex(*vertexIterator);
		auto newVertex = TTissue::AJGraphType::AJVertexType::New();
		newVertex->SetPosition(vertex->GetPosition());
		newVertex->SetEmbeddedPosition(vertex->GetEmbeddedPosition());
		newVertex->SetEmbeddedSphericalPosition(vertex->GetEmbeddedSphericalPosition());
		newVertex->SetDescriptor(vertex->GetDescriptor());
		newVertex->SetVelocity(vertex->GetVelocity());
		for (auto cellsIt = vertex->BeginCells(); cellsIt != vertex->EndCells(); ++cellsIt){
			newVertex->AddCell(*cellsIt);
		}
		resultAJgraph->AddAJVertex(newVertex);
	}
	for (auto edgeIterator = inputTissue->GetAJGraph()->EdgesBegin(); edgeIterator != inputTissue->GetAJGraph()->EdgesEnd(); ++edgeIterator++){
		auto edgeHandler = *edgeIterator;

		auto newEdgeHandler=resultAJgraph->AddAJEdge(inputTissue->GetAJGraph()->GetAJEdgeSource(edgeHandler), inputTissue->GetAJGraph()->GetAJEdgeTarget(edgeHandler));
		auto edge = inputTissue->GetAJGraph()->GetAJEdge(edgeHandler);
		auto newEdge = resultAJgraph->GetAJEdge(newEdgeHandler);
		newEdge->SetDescriptor(edge->GetDescriptor());
		newEdge->SetCellPair(edge->GetCellPair());
	}
	result->SetAJGraph(resultAJgraph);

	typename TTissue::CellGraphType::Pointer resultCellGraph = TTissue::CellGraphType::New();

	for (auto cellIterator = inputTissue->GetCellGraph()->CellsBegin(); cellIterator != inputTissue->GetCellGraph()->CellsEnd(); cellIterator++){

		auto cell = inputTissue->GetCellGraph()->GetCell(*cellIterator);
		typename TTissue::CellGraphType::CellType::Pointer newCell = TTissue::CellGraphType::CellType::New();

		newCell->SetArea(cell->GetArea());
		newCell->SetCellType(cell->GetCellType());
		newCell->SetEccentricity(cell->GetEccentricity());
		newCell->SetMedioapicalDescriptor(cell->GetMedioapicalDescriptor());
		newCell->SetPerimeterLength(cell->GetPerimeterLength());
		newCell->SetPosition(cell->GetPosition());
		newCell->SetVelocity(cell->GetVelocity());
		newCell->SetXX(cell->GetXX());
		newCell->SetXY(cell->GetXY());
		newCell->SetYY(cell->GetYY());

		for (auto vertexIt = cell->BeginPerimeterAJVertices(); vertexIt != cell->EndPerimeterAJVertices(); ++vertexIt){
			newCell->AddAJVertexToPerimeter(*vertexIt);
		}
		for (auto edgeIt = cell->BeginPerimeterAJEdges(); edgeIt != cell->EndPerimeterAJEdges(); ++edgeIt){
			newCell->AddAJEdgeToPerimeter(*edgeIt);
		}
		resultCellGraph->AddCell(newCell);
	}
	for (auto cellEdgeIterator = inputTissue->GetCellGraph()->CellEdgesBegin(); cellEdgeIterator != inputTissue->GetCellGraph()->CellEdgesEnd(); cellEdgeIterator++){

		resultCellGraph->AddCellEdge(inputTissue->GetCellGraph()->GetCellEdgeSource(*cellEdgeIterator), inputTissue->GetCellGraph()->GetCellEdgeTarget(*cellEdgeIterator));

	}
	resultCellGraph->SetUnboundedFace(inputTissue->GetCellGraph()->GetUnboundedFace());
	result->SetCellGraph(resultCellGraph);

	return result;

}
template<class TTissue, class TPlatenessImage> class EnergyModelTissueSmoother{
public:
	typedef TTissue TissueType;
	typedef TPlatenessImage PlatenessImageType;
	typedef TPlatenessImage VertexnessImageType;
	typedef typename PlatenessImageType::IndexType IndexType;
	typedef typename TTissue::AJGraphType::AJVertexHandler AJVertexHandler;
	typedef typename TTissue::AJGraphType::AJEdgeHandler AJEdgeHandler;
	typedef typename TTissue::CellGraphType::CellVertexHandler CellVertexHandler;
	typedef itk::Vector<double,3> VectorType;
	typedef std::map<typename TissueType::AJGraphType::AJVertexHandler,itk::Vector<double,3>> GradientContainer;
private:

public:

	EnergyModelTissueSmoother(){

	}

	void DoADMM(){
//		vtkSmartPointer<vtkRenderer> renderer =		vtkSmartPointer<vtkRenderer>::New();
#if 0
		MinMaxVolumeDrawer<PlatenessImageType> platenessDrawer;
		platenessDrawer.SetRenderer(renderer);
		platenessDrawer.SetImage(m_VertexnessImage);
		platenessDrawer.SetOpacity(0.01);
		platenessDrawer.Draw();
		platenessDrawer.SetVisibility(true);
#endif

//#define WITH_VERTEXNESS
//#define WITH_EDGELENGTHS
///#define WITH_PLANE
//#define WITH_AREAS
//#define WITH_PERIMETER
#define WITH_PLATENESS
		double m_LambdaS=pow(2,-5);
		double m_LambdaP=pow(2,-10);
		double m_LambdaL=pow(2,-100);
		double m_LambdaA=pow(2,10);

		//double m_LambdaP=;
		//double m_LambdaL=10;

		typedef typename TTissue::AJGraphType::AJVertexHandler VertexHandler;
		typedef typename TTissue::AJGraphType::AJEdgeHandler EdgeHandler;
		typedef itk::Vector<double,3> LagrangeMultiplierType;
		typedef itk::Point<double,3> ZType;


		std::map<VertexHandler, std::map<EdgeHandler,LagrangeMultiplierType > > lagrangeEdgeLengths;
		std::map<VertexHandler, std::map<EdgeHandler,ZType> >zEdgeLengths;

		std::map<VertexHandler, std::map<EdgeHandler,LagrangeMultiplierType > > lagrangePlateness;
		std::map<VertexHandler, std::map<EdgeHandler,ZType> >zPlateness;

		std::map<VertexHandler, LagrangeMultiplierType > lagrangeVertexPlane;
		std::map<VertexHandler, ZType > zVertexPlane;

		std::map<VertexHandler, LagrangeMultiplierType > lagrangeVertexness;
		std::map<VertexHandler, ZType> zVertexness;

		std::map<VertexHandler,std::map<CellVertexHandler,LagrangeMultiplierType> > lagrangeAreas;
		std::map<VertexHandler,std::map<CellVertexHandler,ZType> > zAreas;

		std::map<VertexHandler,std::map<CellVertexHandler,LagrangeMultiplierType> > lagrangePerimeter;
		std::map<VertexHandler,std::map<CellVertexHandler,ZType> > zPerimeter;

		typedef itk::LinearInterpolateImageFunction<VertexnessImageType,double> InterpolatorType;
		typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
		interpolator->SetInputImage(m_VertexnessImage);

		auto x0=cloneTissueDescriptor<TTissue>(m_InputTissue);


//		drawPrimal(x0,renderer);
		//0. Init lagrange mutipliers
		std::for_each(x0->GetAJGraph()->VerticesBegin(),x0->GetAJGraph()->VerticesEnd(),[&](const AJVertexHandler & vertexHandler){
			std::for_each(x0->GetAJGraph()->BeginOutEdges(vertexHandler),x0->GetAJGraph()->EndOutEdges(vertexHandler),[&](const  AJEdgeHandler & edgeHandler){
				LagrangeMultiplierType multiplier;
				multiplier.Fill(0.0);
				lagrangeEdgeLengths[vertexHandler][edgeHandler]=multiplier;
				lagrangePlateness[vertexHandler][edgeHandler]=multiplier;
			});
			LagrangeMultiplierType multiplier;
			multiplier.Fill(0.0);
			lagrangeVertexness[vertexHandler]=multiplier;
			if(x0->GetAJGraph()->AJVertexDegree(vertexHandler)>=3){

				lagrangeVertexPlane[vertexHandler]=multiplier;
			}
		});

		std::for_each(x0->GetCellGraph()->CellsBegin(),x0->GetCellGraph()->CellsEnd(),[&](const CellVertexHandler & cellHandler){
			if(cellHandler!=x0->GetCellGraph()->GetUnboundedFace()){
				auto cell =x0->GetCellGraph()->GetCell(cellHandler);
				LagrangeMultiplierType multiplier;
				multiplier.Fill(0.0);
				std::for_each(cell->BeginPerimeterAJVertices(),cell->EndPerimeterAJVertices(),[&](const  AJVertexHandler & vertexHandler){
					lagrangeAreas[vertexHandler][cellHandler]=multiplier;
					lagrangePerimeter[vertexHandler][cellHandler]=multiplier;
				});
			}

		});

		int it=0;

		int MaxIt=5000;
		PathCost<PlatenessImageType> pathCost;
		pathCost.SetInputImage(m_PlatenessImage);
		pathCost.Init();

		while(it<MaxIt){

#ifdef WITH_PLATENESS

			std::for_each(x0->GetAJGraph()->EdgesBegin(),x0->GetAJGraph()->EdgesEnd(),[&](const typename TTissue::AJGraphType::AJEdgeHandler & edgeHandler){
				auto sourceHandler = x0->GetAJGraph()->GetAJEdgeSource(edgeHandler);
				auto targetHandler = x0->GetAJGraph()->GetAJEdgeTarget(edgeHandler);

				auto source = x0->GetAJGraph()->GetAJVertex(sourceHandler);
				auto target = x0->GetAJGraph()->GetAJVertex(targetHandler);

				auto sourcePosition = source->GetPosition() + lagrangePlateness[sourceHandler][edgeHandler];
				auto targetPosition = target->GetPosition() + lagrangePlateness[targetHandler][edgeHandler];;

				PlatenessImageType::IndexType sourceIndex,targetIndex;

				m_PlatenessImage->TransformPhysicalPointToIndex(sourcePosition,sourceIndex);
				m_PlatenessImage->TransformPhysicalPointToIndex(targetPosition,targetIndex);

				if(m_PlatenessImage->GetLargestPossibleRegion().IsInside(sourceIndex) && m_PlatenessImage->GetLargestPossibleRegion().IsInside(targetIndex) ){

					pathCost.SetSourceIndex(sourceIndex);
					pathCost.SetTargetIndex(targetIndex);

					pathCost.Do();


					auto zProxTargetIndex = pathCost.GetProximal();
					itk::Point<double,3> zProxTarget;
					m_PlatenessImage->TransformIndexToPhysicalPoint(zProxTargetIndex,zProxTarget);

					zPlateness[targetHandler][edgeHandler]=zProxTarget;

					pathCost.SetSourceIndex(targetIndex);
					pathCost.SetTargetIndex(sourceIndex);

					pathCost.Do();

					auto zProxSourceIndex = pathCost.GetProximal();
					itk::Point<double,3> zProxSource;
					m_PlatenessImage->TransformIndexToPhysicalPoint(zProxSourceIndex,zProxSource);

					zPlateness[sourceHandler][edgeHandler]=zProxSource;
				}else{
					zPlateness[sourceHandler][edgeHandler]=sourcePosition;
					zPlateness[targetHandler][edgeHandler]=targetPosition;
				}
			});
#endif
#ifdef WITH_VERTEXNESS

			//1.A Compute zVertexness
			std::for_each(x0->GetAJGraph()->VerticesBegin(),x0->GetAJGraph()->VerticesEnd(),[&](const typename TTissue::AJGraphType::AJVertexHandler & vertexHandler){
				auto vertex = x0->GetAJGraph()->GetAJVertex(vertexHandler);
				auto position = vertex->GetPosition();

				position = position + lagrangeVertexness[vertexHandler];

				IndexType index;
				this->m_VertexnessImage->TransformPhysicalPointToIndex(position,index);

				bool positionUpdated=true;
				bool isMaxima=false;

				while(!isMaxima){
					isMaxima=true;

					if(m_VertexnessImage->GetBufferedRegion().IsInside(index)){
						IndexType localMax;
						double maxValue=m_VertexnessImage->GetPixel(index);
						itk::Offset<3> offset;
						for(offset[0] =-1;offset[0]<2;offset[0]++){
							for(offset[1] =-1;offset[1]<2;offset[1]++){
								for(offset[2] =-1;offset[2]<2;offset[2]++){
									if(offset[0]==0 && offset[1]==0 && offset[2]==0) continue;

									IndexType neighIndex= index +offset;
									if(!m_VertexnessImage->GetBufferedRegion().IsInside(neighIndex)) continue;
									if(this->m_VertexnessImage->GetPixel(neighIndex)>maxValue){
										isMaxima=false;
										maxValue=this->m_VertexnessImage->GetPixel(neighIndex);
										localMax=neighIndex;
									}
								}
							}
						}
						if(!isMaxima){
							index=localMax;
							positionUpdated=true;
						}
					}else{

						isMaxima=true;
					}
				}
				if(positionUpdated){
					m_VertexnessImage->TransformIndexToPhysicalPoint(index,position);
				}
				zVertexness[vertexHandler]=position;


			});
#endif


#ifdef WITH_PLANE


			//1.B Compute zVertexPlane
			std::for_each(x0->GetAJGraph()->VerticesBegin(),x0->GetAJGraph()->VerticesEnd(),[&](const typename TTissue::AJGraphType::AJVertexHandler & vertexHandler){
				if(x0->GetAJGraph()->AJVertexDegree(vertexHandler)>=3){

					auto vertex = x0->GetAJGraph()->GetAJVertex(vertexHandler);
					auto position = vertex->GetPosition();

					position = position+lagrangeVertexPlane[vertexHandler];

					auto normalAndMean=embeddingPlane<typename TTissue::AJGraphType::AdjacencyIterator,typename TTissue::AJGraphType>(x0->GetAJGraph()->AdjacentAJVerticesBegin(vertexHandler),x0->GetAJGraph()->AdjacentAJVerticesEnd(vertexHandler), x0->GetAJGraph());


					auto normal = std::get<0>(normalAndMean);
					auto mean = std::get<1>(normalAndMean);


					auto v = position -mean;

					double dist = normal*v;

					auto projectedPosition = position -dist*normal;

					auto distPP = (position-projectedPosition).GetNorm();

					double shrinked = distPP-m_LambdaS;
					if(shrinked<=0){

						zVertexPlane[vertexHandler]=projectedPosition;

					}else{
						auto projection = position -m_LambdaS*normal;
						zVertexPlane[vertexHandler]=projection;

					}
				}
			});
#endif

#ifdef WITH_EDGELENGTHS


			//1.C Compute zEdgeLengths
			std::for_each(x0->GetAJGraph()->EdgesBegin(),x0->GetAJGraph()->EdgesEnd(),[&](const AJEdgeHandler & edgeHandler){
				auto sourceHandler = x0->GetAJGraph()->GetAJEdgeSource(edgeHandler);
				auto targetHandler = x0->GetAJGraph()->GetAJEdgeTarget(edgeHandler);

				auto sourceVertex = x0->GetAJGraph()->GetAJVertex(sourceHandler);
				auto targetVertex = x0->GetAJGraph()->GetAJVertex(targetHandler);

				auto sourcePosition = sourceVertex->GetPosition()+lagrangeEdgeLengths[sourceHandler][edgeHandler];
				auto targetPosition = targetVertex->GetPosition()+lagrangeEdgeLengths[targetHandler][edgeHandler];

				auto vector = targetPosition-sourcePosition;
				auto midPoint = sourcePosition + 0.5*vector;

				double dist = (sourcePosition -midPoint).GetNorm();
				double shrinked = dist-m_LambdaL;
				if(shrinked<=0){
					//std::cout << midPoint << std::endl;
					zEdgeLengths[sourceHandler][edgeHandler]=midPoint;
					zEdgeLengths[targetHandler][edgeHandler]=midPoint;
				}else{
					auto shrinkedSourcePosition = sourcePosition + (dist-shrinked)*vector;
					auto shrinkedTargetPosition = targetPosition - (dist-shrinked)*vector;
					zEdgeLengths[sourceHandler][edgeHandler] =shrinkedSourcePosition;
					zEdgeLengths[targetHandler][edgeHandler] =shrinkedTargetPosition;
				}


			});
#endif
#ifdef WITH_AREAS
			//1.C Compute areas

			std::for_each(x0->GetCellGraph()->CellsBegin(),x0->GetCellGraph()->CellsEnd(),[&](const CellVertexHandler & cellHandler){
				if(cellHandler!=x0->GetCellGraph()->GetUnboundedFace()){

					auto cell = x0->GetCellGraph()->GetCell(cellHandler);

					auto normalAndMean=embeddingPlane<typename TTissue::CellGraphType::CellType::PerimeterVertexIterator,typename TTissue::AJGraphType>(cell->BeginPerimeterAJVertices(), cell->EndPerimeterAJVertices(),x0->GetAJGraph());
					itk::Vector<double,3> total;
					total.Fill(0);
					double area=0;

					//A. Compute area
					for(auto perimeterEdgeIt = cell->BeginPerimeterAJEdges();perimeterEdgeIt!=cell->EndPerimeterAJEdges();++perimeterEdgeIt){

						auto next = std::next(perimeterEdgeIt);

						if(next==cell->EndPerimeterAJEdges()){
							next = cell->BeginPerimeterAJEdges();
						}
						auto source = x0->GetAJGraph()->GetAJEdgeSource(*perimeterEdgeIt);
						auto target = x0->GetAJGraph()->GetAJEdgeTarget(*perimeterEdgeIt);

						auto sourceN =x0->GetAJGraph()->GetAJEdgeSource(*next);
						auto targetN =x0->GetAJGraph()->GetAJEdgeTarget(*next);

						itk::Point<double,3> a,b,c;
						typename TTissue::AJGraphType::AJVertexHandler aHandler,bHandler,cHandler;
						if(source==sourceN){
							aHandler=target;
							bHandler=targetN;
							cHandler=source;
						}else if(source==targetN){
							aHandler=target;
							bHandler=sourceN;
							cHandler=source;
						}else if(target == sourceN){
							aHandler =source;
							bHandler=targetN;
							cHandler=target;
						}else if(target == targetN){
							aHandler=source;
							bHandler=sourceN;
							cHandler=target;
						}

						a = x0->GetAJGraph()->GetAJVertex(aHandler)->GetPosition() + lagrangeAreas[aHandler][cellHandler];
						b = x0->GetAJGraph()->GetAJVertex(bHandler)->GetPosition()+ lagrangeAreas[bHandler][cellHandler];
						c = x0->GetAJGraph()->GetAJVertex(cHandler)->GetPosition() + lagrangeAreas[cHandler][cellHandler];

						auto CA= c-a;
						auto CB= c-b;
						if(CA.GetNorm()==0){
							continue;
						}
						if(CB.GetNorm()==0){
							continue;
						}

						auto N=itk::CrossProduct(CA,CB);

						total+=N;


					}
					area=total*std::get<0>(normalAndMean)/2;
					//B. Project area

					for(auto perimeterEdgeIt = cell->BeginPerimeterAJEdges();perimeterEdgeIt!=cell->EndPerimeterAJEdges();++perimeterEdgeIt){

						auto next = std::next(perimeterEdgeIt);

						if(next==cell->EndPerimeterAJEdges()){
							next = cell->BeginPerimeterAJEdges();
						}
						auto source = x0->GetAJGraph()->GetAJEdgeSource(*perimeterEdgeIt);
						auto target = x0->GetAJGraph()->GetAJEdgeTarget(*perimeterEdgeIt);

						auto sourceN =x0->GetAJGraph()->GetAJEdgeSource(*next);
						auto targetN =x0->GetAJGraph()->GetAJEdgeTarget(*next);

						itk::Point<double,3> a,b,c;
						typename TTissue::AJGraphType::AJVertexHandler aHandler,bHandler,cHandler;
						if(source==sourceN){
							aHandler=target;
							bHandler=targetN;
							cHandler=source;
						}else if(source==targetN){
							aHandler=target;
							bHandler=sourceN;
							cHandler=source;
						}else if(target == sourceN){
							aHandler =source;
							bHandler=targetN;
							cHandler=target;
						}else if(target == targetN){
							aHandler=source;
							bHandler=sourceN;
							cHandler=target;
						}

						a = x0->GetAJGraph()->GetAJVertex(aHandler)->GetPosition() + lagrangeAreas[aHandler][cellHandler];
						b = x0->GetAJGraph()->GetAJVertex(bHandler)->GetPosition()+ lagrangeAreas[bHandler][cellHandler];
						c = x0->GetAJGraph()->GetAJVertex(cHandler)->GetPosition() + lagrangeAreas[cHandler][cellHandler];


						auto CA= c-a;
						auto CB= c-b;
						if(CA.GetNorm()==0){
							zAreas[cHandler][cellHandler]= c;
							continue;
						}
						if(CB.GetNorm()==0){
							zAreas[cHandler][cellHandler]= c;
							continue;
						}

#if 0
						auto bisector = (CA+CB)/2;
						auto N=itk::CrossProduct(CA,CB);

						auto gradient = area*m_LambdaA*bisector;
						zAreas[cHandler][cellHandler]= c - gradient;
#endif


						auto BA= b-a;
						auto N = itk::CrossProduct(CA,CB);
						N.Normalize();
						auto dir = itk::CrossProduct(N,BA);
						auto gradient = area*m_LambdaA*dir;


						zAreas[cHandler][cellHandler]= c + gradient;
						if(gradient.GetNorm()>1){
							std::cout << "A: " << a << "\t" << lagrangeAreas[aHandler][cellHandler]<<std::endl;
							std::cout << "B: " << b << "\t" << lagrangeAreas[bHandler][cellHandler]<<std::endl;
							std::cout << "C: " << c << "\t" << lagrangeAreas[cHandler][cellHandler]<<std::endl;
							std::cout << "BA: " << BA << std::endl;
							std::cout << "N: " << N << std::endl;
							std::cout << "Dir: " << dir << std::endl;
							std::cout << "Gradient: " << gradient << std::endl;
							std::cout << "Result: " << zAreas[cHandler][cellHandler] <<  "\t" << gradient.GetNorm() << std::endl;
						}
					}

				}

			});

#endif

#ifdef WITH_PERIMETER
			//1.C Compute areas

			std::for_each(x0->GetCellGraph()->CellsBegin(),x0->GetCellGraph()->CellsEnd(),[&](const CellVertexHandler & cellHandler){
				if(cellHandler!=x0->GetCellGraph()->GetUnboundedFace()){

					auto cell = x0->GetCellGraph()->GetCell(cellHandler);

					itk::Vector<double,3> total;
					total.Fill(0);
					double area=0;
					double perimeter=0;
					//A. Compute perimeter
					for(auto perimeterEdgeIt = cell->BeginPerimeterAJEdges();perimeterEdgeIt!=cell->EndPerimeterAJEdges();++perimeterEdgeIt){

						auto source = x0->GetAJGraph()->GetAJEdgeSource(*perimeterEdgeIt);
						auto target = x0->GetAJGraph()->GetAJEdgeTarget(*perimeterEdgeIt);


						{
							auto sourceP=x0->GetAJGraph()->GetAJVertex(source)->GetPosition()+ lagrangePerimeter[source][cellHandler];
							auto targetP=x0->GetAJGraph()->GetAJVertex(target)->GetPosition()+ lagrangePerimeter[target][cellHandler];
							perimeter+=(targetP-sourceP).GetNorm();
						}
					}

					//B. Project perimeter

					for(auto perimeterEdgeIt = cell->BeginPerimeterAJEdges();perimeterEdgeIt!=cell->EndPerimeterAJEdges();++perimeterEdgeIt){

						auto next = std::next(perimeterEdgeIt);

						if(next==cell->EndPerimeterAJEdges()){
							next = cell->BeginPerimeterAJEdges();
						}
						auto source = x0->GetAJGraph()->GetAJEdgeSource(*perimeterEdgeIt);
						auto target = x0->GetAJGraph()->GetAJEdgeTarget(*perimeterEdgeIt);

						auto sourceN =x0->GetAJGraph()->GetAJEdgeSource(*next);
						auto targetN =x0->GetAJGraph()->GetAJEdgeTarget(*next);

						itk::Point<double,3> a,b,c;
						typename TTissue::AJGraphType::AJVertexHandler aHandler,bHandler,cHandler;
						if(source==sourceN){
							aHandler=target;
							bHandler=targetN;
							cHandler=source;
						}else if(source==targetN){
							aHandler=target;
							bHandler=sourceN;
							cHandler=source;
						}else if(target == sourceN){
							aHandler =source;
							bHandler=targetN;
							cHandler=target;
						}else if(target == targetN){
							aHandler=source;
							bHandler=sourceN;
							cHandler=target;
						}

						a = x0->GetAJGraph()->GetAJVertex(aHandler)->GetPosition() + lagrangePerimeter[aHandler][cellHandler];
						b = x0->GetAJGraph()->GetAJVertex(bHandler)->GetPosition()+ lagrangePerimeter[bHandler][cellHandler];
						c = x0->GetAJGraph()->GetAJVertex(cHandler)->GetPosition() + lagrangePerimeter[cHandler][cellHandler];


						auto CA= c-a;
						auto CB= c-b;
						if(CA.GetNorm()==0){
							std::cout << "problem" << std::endl;
							continue;
						}
						if(CB.GetNorm()==0){
							std::cout << "problem" << std::endl;
							continue;
						}

						auto nCA=CA/CA.GetNorm();
						auto nCB=CB/CB.GetNorm();


						zPerimeter[cHandler][cellHandler]=c+ perimeter*m_LambdaP*(nCA+nCB);

					}

				}

			});

#endif
			//2 Compute za
			//auto x1 = cloneTissueDescriptor<TTissue>(x0);
			std::for_each(x0->GetAJGraph()->VerticesBegin(),x0->GetAJGraph()->VerticesEnd(),[&](const typename TTissue::AJGraphType::AJVertexHandler & vertexHandler){
				itk::Point<double,3> position;
				position.Fill(0);
				int total=0;
#ifdef WITH_VERTEXNESS
				for(int d=0;d<3;d++){
					position[d]+=zVertexness[vertexHandler][d]-lagrangeVertexness[vertexHandler][d];
				}
				total++;
#endif


#ifdef WITH_PLANE
				if(x0->GetAJGraph()->AJVertexDegree(vertexHandler)>=3){

					for(int d=0;d<3;d++){

						position[d]+=zVertexPlane[vertexHandler][d] - lagrangeVertexPlane[vertexHandler][d];
					}
					total++;
				}
#endif
#ifdef WITH_PLATENESS
				std::for_each(x0->GetAJGraph()->BeginOutEdges(vertexHandler),x0->GetAJGraph()->EndOutEdges(vertexHandler),[&](const AJEdgeHandler & edgeHandler){

					for(int d=0;d<3;d++){
						position[d]+=zPlateness[vertexHandler][edgeHandler][d]-lagrangePlateness[vertexHandler][edgeHandler][d];
					}
					total++;

				});
#endif
#ifdef WITH_EDGELENGTHS

				std::for_each(x0->GetAJGraph()->BeginOutEdges(vertexHandler),x0->GetAJGraph()->EndOutEdges(vertexHandler),[&](const AJEdgeHandler & edgeHandler){

					for(int d=0;d<3;d++){
						position[d]+=zEdgeLengths[vertexHandler][edgeHandler][d] - lagrangeEdgeLengths[vertexHandler][edgeHandler][d];
					}
					total++;

				});
#endif
				for(auto it =zAreas[vertexHandler].begin();it!=zAreas[vertexHandler].end();++it ){
					//std::cout << zAreas[vertexHandler][it->first] << "\t" << lagrangeAreas[vertexHandler][it->first] << std::endl;
					for(int d=0;d<3;d++){
#ifdef WITH_AREAS
						position[d]+=zAreas[vertexHandler][it->first][d] - lagrangeAreas[vertexHandler][it->first][d];
#endif
#ifdef WITH_PERIMETER
						position[d]+=zPerimeter[vertexHandler][it->first][d] - lagrangePerimeter[vertexHandler][it->first][d];
#endif

					}
					total+=2;

				}

				if(total==0){
					position =x0->GetAJGraph()->GetAJVertex(vertexHandler)->GetPosition();
				}else{
					for(int d=0;d<3;d++){
						position[d]/=total;
					}
				}


				//std::cout << x0->GetAJGraph()->GetAJVertex(vertexHandler)->GetPosition() << "\t->\t" << position << std::endl;
				x0->GetAJGraph()->GetAJVertex(vertexHandler)->SetPosition(position);
			});

			double f=0;

			std::for_each(x0->GetAJGraph()->EdgesBegin(),x0->GetAJGraph()->EdgesEnd(),[&](const typename TTissue::AJGraphType::AJEdgeHandler & edgeHandler){
				auto sourceHandler = x0->GetAJGraph()->GetAJEdgeSource(edgeHandler);
				auto targetHandler = x0->GetAJGraph()->GetAJEdgeTarget(edgeHandler);

				auto source = x0->GetAJGraph()->GetAJVertex(sourceHandler);
				auto target = x0->GetAJGraph()->GetAJVertex(targetHandler);

				auto sourcePosition = zPlateness[sourceHandler][edgeHandler];//source->GetPosition() + lagrangePlateness[sourceHandler][edgeHandler];
				auto targetPosition = zPlateness[targetHandler][edgeHandler];//target->GetPosition() + lagrangePlateness[targetHandler][edgeHandler];;

				PlatenessImageType::IndexType sourceIndex,targetIndex;

				m_PlatenessImage->TransformPhysicalPointToIndex(sourcePosition,sourceIndex);
				m_PlatenessImage->TransformPhysicalPointToIndex(targetPosition,targetIndex);

				if(m_PlatenessImage->GetLargestPossibleRegion().IsInside(sourceIndex) && m_PlatenessImage->GetLargestPossibleRegion().IsInside(targetIndex) ){
					pathCost.SetSourceIndex(sourceIndex);
					pathCost.SetTargetIndex(targetIndex);

					pathCost.Do();

					auto zProxTargetIndex = pathCost.GetProximal();
					itk::Point<double,3> zProxTarget;
					m_PlatenessImage->TransformIndexToPhysicalPoint(zProxTargetIndex,zProxTarget);

					f=f+pathCost.GetCost();

					pathCost.SetSourceIndex(targetIndex);
					pathCost.SetTargetIndex(sourceIndex);

					pathCost.Do();
					f=f+pathCost.GetCost();
					auto zProxSourceIndex = pathCost.GetProximal();
					itk::Point<double,3> zProxSource;
					m_PlatenessImage->TransformIndexToPhysicalPoint(zProxSourceIndex,zProxSource);


				}else{

				}

				f=f+lagrangePlateness[sourceHandler][edgeHandler]*(source->GetPosition()-sourcePosition);
				f=f+lagrangePlateness[targetHandler][edgeHandler]*(target->GetPosition()-targetPosition);

				f=f+0.5*(source->GetPosition()-sourcePosition).GetSquaredNorm();
				f=f+0.5*(target->GetPosition()-targetPosition).GetSquaredNorm();
			});

			//Compute F

			std::for_each(x0->GetAJGraph()->VerticesBegin(),x0->GetAJGraph()->VerticesEnd(),[&](const typename TTissue::AJGraphType::AJVertexHandler & vertexHandler){
				auto vertex = x0->GetAJGraph()->GetAJVertex(vertexHandler);
				auto position = vertex->GetPosition();
#ifdef WITH_VERTEXNESS
				itk::Index<3> index;

				m_VertexnessImage->TransformPhysicalPointToIndex(position, index);
				if (m_VertexnessImage->GetLargestPossibleRegion().IsInside(index)){
					f -= interpolator->Evaluate(position);
				}
#endif
#ifdef WITH_PLANE


				if(x0->GetAJGraph()->AJVertexDegree(vertexHandler)>=3){

					auto normalAndMean=embeddingPlane<typename TTissue::AJGraphType::AdjacencyIterator,typename TTissue::AJGraphType>(x0->GetAJGraph()->AdjacentAJVerticesBegin(vertexHandler),x0->GetAJGraph()->AdjacentAJVerticesEnd(vertexHandler), x0->GetAJGraph());


					auto normal = std::get<0>(normalAndMean);
					auto mean = std::get<1>(normalAndMean);


					auto v = position -mean;

					double dist = normal*v;

					f+=m_LambdaS*dist;
				}
#endif
			});

#ifdef WITH_EDGELENGTHS
			std::for_each(x0->GetAJGraph()->EdgesBegin(),x0->GetAJGraph()->EdgesEnd(),[&](const typename TTissue::AJGraphType::AJEdgeHandler & edgeHandler){
				auto sourceHandler = x0->GetAJGraph()->GetAJEdgeSource(edgeHandler);
				auto targetHandler = x0->GetAJGraph()->GetAJEdgeTarget(edgeHandler);

				auto sourceVertex = x0->GetAJGraph()->GetAJVertex(sourceHandler);
				auto targetVertex = x0->GetAJGraph()->GetAJVertex(targetHandler);

				auto sourcePosition = sourceVertex->GetPosition();
				auto targetPosition = targetVertex->GetPosition();

				auto vector = targetPosition-sourcePosition;
				//std::cout << vector.GetNorm() << std::endl;
				f+=m_LambdaL*vector.GetNorm();
			});
#endif

#ifdef WITH_AREAS

			std::for_each(x0->GetCellGraph()->CellsBegin(),x0->GetCellGraph()->CellsEnd(),[&](const CellVertexHandler & cellHandler){
				if(cellHandler!=x0->GetCellGraph()->GetUnboundedFace()){

					auto cell = x0->GetCellGraph()->GetCell(cellHandler);

					auto normalAndMean=embeddingPlane<typename TTissue::CellGraphType::CellType::PerimeterVertexIterator,typename TTissue::AJGraphType>(cell->BeginPerimeterAJVertices(), cell->EndPerimeterAJVertices(),x0->GetAJGraph());
					itk::Vector<double,3> total;
					total.Fill(0);
					double area=0;

					//A. Compute area
					for(auto perimeterEdgeIt = cell->BeginPerimeterAJEdges();perimeterEdgeIt!=cell->EndPerimeterAJEdges();++perimeterEdgeIt){

						auto next = std::next(perimeterEdgeIt);

						if(next==cell->EndPerimeterAJEdges()){
							next = cell->BeginPerimeterAJEdges();
						}
						auto source = x0->GetAJGraph()->GetAJEdgeSource(*perimeterEdgeIt);
						auto target = x0->GetAJGraph()->GetAJEdgeTarget(*perimeterEdgeIt);

						auto sourceN =x0->GetAJGraph()->GetAJEdgeSource(*next);
						auto targetN =x0->GetAJGraph()->GetAJEdgeTarget(*next);

						itk::Point<double,3> a,b,c;
						typename TTissue::AJGraphType::AJVertexHandler aHandler,bHandler,cHandler;
						if(source==sourceN){
							aHandler=target;
							bHandler=targetN;
							cHandler=source;
						}else if(source==targetN){
							aHandler=target;
							bHandler=sourceN;
							cHandler=source;
						}else if(target == sourceN){
							aHandler =source;
							bHandler=targetN;
							cHandler=target;
						}else if(target == targetN){
							aHandler=source;
							bHandler=sourceN;
							cHandler=target;
						}

						a = x0->GetAJGraph()->GetAJVertex(aHandler)->GetPosition();
						b = x0->GetAJGraph()->GetAJVertex(bHandler)->GetPosition();
						c = x0->GetAJGraph()->GetAJVertex(cHandler)->GetPosition();
						auto CA= c-a;
						auto CB= c-b;
						if(CA.GetNorm()==0){
							std::cout << "problem" << std::endl;
						}
						if(CB.GetNorm()==0){
							std::cout << "problem" << std::endl;
						}
						auto bisector = (CA+CB)/2;
						auto N=itk::CrossProduct(CA,CB);
						total+=N;
					}
					area=std::abs(total*std::get<0>(normalAndMean))/2;
					f+=-0.5*m_LambdaA*area;
				}
			});
#endif
#ifdef WITH_PERIMETER
			double perimeter=0;
			std::for_each(x0->GetCellGraph()->CellsBegin(),x0->GetCellGraph()->CellsEnd(),[&](const CellVertexHandler & cellHandler){
				if(cellHandler!=x0->GetCellGraph()->GetUnboundedFace()){

					auto cell = x0->GetCellGraph()->GetCell(cellHandler);

					itk::Vector<double,3> total;
					total.Fill(0);
					double area=0;
					double perimeter=0;
					//A. Compute perimeter
					for(auto perimeterEdgeIt = cell->BeginPerimeterAJEdges();perimeterEdgeIt!=cell->EndPerimeterAJEdges();++perimeterEdgeIt){

						auto source = x0->GetAJGraph()->GetAJEdgeSource(*perimeterEdgeIt);
						auto target = x0->GetAJGraph()->GetAJEdgeTarget(*perimeterEdgeIt);

							auto sourceP=x0->GetAJGraph()->GetAJVertex(source)->GetPosition()+ lagrangePerimeter[source][cellHandler];
							auto targetP=x0->GetAJGraph()->GetAJVertex(target)->GetPosition()+ lagrangePerimeter[target][cellHandler];
							perimeter+=0.5*m_LambdaP*(targetP-sourceP).GetNorm();

					}
				}
				});
			f=f-perimeter;
#endif
			std::cout << it << "\t" << f << std::endl;

#ifdef WITH_VERTEXNESS
			//3.A Computer lagrangeVertexness
			std::for_each(x0->GetAJGraph()->VerticesBegin(),x0->GetAJGraph()->VerticesEnd(),[&](const typename TTissue::AJGraphType::AJVertexHandler & vertexHandler){
				lagrangeVertexness[vertexHandler]+=(x0->GetAJGraph()->GetAJVertex(vertexHandler)->GetPosition() -zVertexness[vertexHandler]);
			});
#endif
#ifdef WITH_PLANE

			//3.B Computer lagrangeVertexPlane
			std::for_each(x0->GetAJGraph()->VerticesBegin(),x0->GetAJGraph()->VerticesEnd(),[&](const typename TTissue::AJGraphType::AJVertexHandler & vertexHandler){
				if(x0->GetAJGraph()->AJVertexDegree(vertexHandler)>=3){

					lagrangeVertexPlane[vertexHandler]+=(x0->GetAJGraph()->GetAJVertex(vertexHandler)->GetPosition() -zVertexPlane[vertexHandler]);
				}

			});
#endif


#ifdef WITH_EDGELENGTHS
			//3.C Computer lagrangeEdgeLengths
			std::for_each(x0->GetAJGraph()->VerticesBegin(),x0->GetAJGraph()->VerticesEnd(),[&](const AJVertexHandler & vertexHandler){

				std::for_each(x0->GetAJGraph()->BeginOutEdges(vertexHandler),x0->GetAJGraph()->EndOutEdges(vertexHandler),[&](const AJEdgeHandler & edgeHandler){
					lagrangeEdgeLengths[vertexHandler][edgeHandler]+=(x0->GetAJGraph()->GetAJVertex(vertexHandler)->GetPosition() -zEdgeLengths[vertexHandler][edgeHandler]);

				});
			});
#endif
			std::for_each(x0->GetAJGraph()->VerticesBegin(),x0->GetAJGraph()->VerticesEnd(),[&](const AJVertexHandler & vertexHandler){

				std::for_each(x0->GetAJGraph()->BeginOutEdges(vertexHandler),x0->GetAJGraph()->EndOutEdges(vertexHandler),[&](const AJEdgeHandler & edgeHandler){
					lagrangePlateness[vertexHandler][edgeHandler]+=(x0->GetAJGraph()->GetAJVertex(vertexHandler)->GetPosition() -zPlateness[vertexHandler][edgeHandler]);

				});
			});


			std::for_each(x0->GetCellGraph()->CellsBegin(),x0->GetCellGraph()->CellsEnd(),[&](const CellVertexHandler & cellHandler){
				if(cellHandler!=x0->GetCellGraph()->GetUnboundedFace()){

				auto cell = x0->GetCellGraph()->GetCell(cellHandler);
				std::for_each(cell->BeginPerimeterAJVertices(),cell->EndPerimeterAJVertices(),[&](const AJVertexHandler & vertexHandler){
#ifdef WITH_AREAS
					auto update = (x0->GetAJGraph()->GetAJVertex(vertexHandler)->GetPosition() -zAreas[vertexHandler][cellHandler]);
					if(update.GetNorm()>1){
						//std::cout << x0->GetAJGraph()->GetAJVertex(vertexHandler)->GetPosition() << "\t" <<  zAreas[vertexHandler][cellHandler] << "\t"<<update << std::endl;
					}
					lagrangeAreas[vertexHandler][cellHandler]+=update;
#endif
#ifdef WITH_PERIMETER
					lagrangePerimeter[vertexHandler][cellHandler]+=(x0->GetAJGraph()->GetAJVertex(vertexHandler)->GetPosition() -zPerimeter[vertexHandler][cellHandler]);
#endif
				});
				}
			});

			++it;
		
		}

		m_OutputTissue = x0;
		


	}
#if 0
	void Do(){


		int it=0;
		vtkSmartPointer<vtkRenderer> renderer =		vtkSmartPointer<vtkRenderer>::New();
#if 1
		MinMaxVolumeDrawer<PlatenessImageType> platenessDrawer;
		platenessDrawer.SetRenderer(renderer);
		platenessDrawer.SetImage(m_VertexnessImage);
		platenessDrawer.SetOpacity(0.10);
		platenessDrawer.Draw();
		platenessDrawer.SetVisibility(true);
#endif
		m_GradientFilter=GradientFilterType::New();
		m_GradientFilter->SetInput(m_VertexnessImage);
		m_GradientFilter->Update();
		drawPrimal(m_InputTissue,renderer);

		typename TTissue::Pointer m_X0= cloneTissueDescriptor<TTissue>(m_InputTissue);
		double f,f1;
		GradientContainer gradient,gradient1;
		std::tie(f,gradient) = this->Eval(m_X0);

		m_MaxIterations=100;
		double maxEvals=100;

		int numEvals=1;
		//double alpha =1e-5;

		while(it<m_MaxIterations && numEvals < maxEvals){
			//double alpha =1e-5;
			double alpha=1;
			std::cout <<it << "\t" << f << std::endl;

			double gradientMagnitudeSquared = std::accumulate(gradient.begin(),gradient.end(),0.0,[](double sum,typename GradientContainer::value_type & v ){
				return sum+v.second.GetSquaredNorm();
			});
			std::cout << "\t\t\t\t" << gradientMagnitudeSquared << std::endl;


			typename TTissue::Pointer m_X1;
			bool minimizerFound=false;
			while(!minimizerFound && numEvals < maxEvals){
				m_X1 = this->ApplyGradientToTissue(m_X0,gradient,alpha);
				std::pair<double,GradientContainer> newValueAndGradient;
				std::tie(f1,gradient1)=this->Eval(m_X1);
				numEvals++;
				std::cout << "\t" << f1 << std::endl;

				if(f1>=f){
					alpha=alpha/2;
					minimizerFound=false;
				}else{
					minimizerFound=true;
				}
			}
			if(minimizerFound){
				f=f1;
				gradient=gradient1;
				m_X0=m_X1;
			}
			it++;
		}
		drawPrimal(m_X0,renderer);

	}
#endif
	void SetInputTissue(const typename TissueType::Pointer & inputTissue){
		m_InputTissue=inputTissue;
	}
	typename TissueType::Pointer GetOutputTissue(){
		return m_OutputTissue;
	}
	void SetPlatenessImage(const typename PlatenessImageType::Pointer & plateness){
		m_PlatenessImage=plateness;
	}
	void SetVertexnessImage(const typename PlatenessImageType::Pointer & vertexness){
		m_VertexnessImage=vertexness;
	}


	typename TissueType::Pointer m_InputTissue;
	typename TissueType::Pointer m_OutputTissue;
	typename PlatenessImageType::Pointer m_PlatenessImage;
	typename PlatenessImageType::Pointer m_VertexnessImage;

//	typedef itk::GradientImageFilter<PlatenessImageType> GradientFilterType;
//	typename GradientFilterType::Pointer m_GradientFilter;
	unsigned m_MaxIterations;
};
#if 0
#include <itkLevelSet.h>
#include <queue>
//#include <vtkSmartPointer.h>
//#include <vtkRenderer.h>
//#include <drawPrimal.h>
#include <MinMaxVolumeDrawer.h>
#include <itkGradientImageFilter.h>


#include <itkVectorLinearInterpolateImageFunction.h>
#include <utility>
template<class TVelocityImage> class PathCost{

public:

	typedef TVelocityImage ImageType;
	typedef typename ImageType::PointType PointType;
	typedef typename ImageType::IndexType IndexType;

	typedef typename ImageType::PointType::VectorType VectorType;

public:
	void Do(){
		if(this->m_SourceIndex==this->m_TargetIndex){
			m_Cost=0;
			m_TargetGradient.Fill(0);
			return;
		}
		this->m_LabelImage->FillBuffer(FarPoint);
		this->m_LevelSetImage->FillBuffer(itk::NumericTraits<double>::max());
		this->m_LabelImage->SetPixel(this->m_SourceIndex,InitialTrialPoint);
		this->m_LevelSetImage->SetPixel(this->m_SourceIndex,0);


	    LevelSetNodeType node;
	    node.SetIndex(this->m_SourceIndex);
	    node.SetValue(0);

		this->m_TrialHeap.push(node);

	    double       centerValue;
	    double       oldProgress = 0;

	    std::list<IndexType> visitedRegion;
	    // CACHE
	    while ( !m_TrialHeap.empty() )
	      {
	      // get the node with the smallest value
	      node = m_TrialHeap.top();
	      m_TrialHeap.pop();



	      IndexType centerIndex = node.GetIndex();
	      PointType centerPoint;
	      this->m_LabelImage->TransformIndexToPhysicalPoint(centerIndex,centerPoint);

	      // does this node contain the current value ?
	      centerValue = static_cast< double >( m_LevelSetImage->GetPixel( node.GetIndex() ) );
	      if(centerValue!=node.GetValue()){

	    	  continue;
	      }

	      if(centerIndex==m_TargetIndex){
	    	  m_Cost=centerValue;
	    	  break;
	      }

	      auto centerSpeed = m_InputImage->GetPixel(centerIndex);

	      double vmin=1e-3;
	      if ( m_LabelImage->GetPixel( node.GetIndex() ) == AlivePoint ||  m_LabelImage->GetPixel( node.GetIndex() ) == InitialTrialPoint ){
	          // set this node as alive
	          m_LabelImage->SetPixel(node.GetIndex(), VisitedPoint);

	          unsigned char label;

	          assert(m_LabelImage->GetBufferedRegion().IsInside(centerIndex));

	          itk::Offset<3> offset;
	          for(offset[0] =-1;offset[0]<2;offset[0]++){
	            for(offset[1] =-1;offset[1]<2;offset[1]++){
	                for(offset[2] =-1;offset[2]<2;offset[2]++){
	                    if(offset[0]==0 && offset[1]==0 && offset[2]==0) continue;

	                    IndexType neighIndex= centerIndex +offset;
	                    if(!m_InputImage->GetBufferedRegion().IsInside(neighIndex)) continue;

	                    if(this->m_LabelImage->GetPixel(neighIndex)==FarPoint || this->m_LabelImage->GetPixel(neighIndex)==AlivePoint ){
	                        PointType neighPoint;

	                        this->m_InputImage->TransformIndexToPhysicalPoint(neighIndex,neighPoint);

	                        auto neighSpeed= this->m_InputImage->GetPixel(neighIndex);
	                        //double newValue =centerValue + 1.0/(0.00000001+(neighSpeed+centerSpeed/2);
	                        double newValue = centerValue + 2*(centerPoint-neighPoint).GetNorm()/(vmin+neighSpeed+centerSpeed);
	                        double oldValue =this->m_LevelSetImage->GetPixel(neighIndex);

	                        if(newValue<oldValue){
	                            	this->m_Origins->SetPixel(neighIndex,centerIndex);
									this->m_LabelImage->SetPixel(neighIndex,AlivePoint);
									this->m_LevelSetImage->SetPixel(neighIndex,newValue);

									LevelSetNodeType node;
									node.SetIndex(neighIndex);
									node.SetValue(newValue);
									this->m_TrialHeap.push(node);
	                            }
	                    }
	                }
	            }
	          }
	      }
	    }

        PointType targetPoint,previousPoint;
        m_InputImage->TransformIndexToPhysicalPoint(m_TargetIndex,targetPoint);
        m_InputImage->TransformIndexToPhysicalPoint(m_Origins->GetPixel(m_TargetIndex),previousPoint);

        auto vector = targetPoint - previousPoint;
        auto norm = vector.GetNorm();
        //m_TargetGradient[0]=(m_LevelSetImage->GetPixel(m_TargetIndex)-m_LevelSetImage->GetPixel(m_Origins->GetPixel(m_TargetIndex)))/(targetPoint[0]-previousPoint[0]);
        //m_TargetGradient[1]=(m_LevelSetImage->GetPixel(m_TargetIndex)-m_LevelSetImage->GetPixel(m_Origins->GetPixel(m_TargetIndex)))/(targetPoint[1]-previousPoint[1]);
        //m_TargetGradient[2]=(m_LevelSetImage->GetPixel(m_TargetIndex)-m_LevelSetImage->GetPixel(m_Origins->GetPixel(m_TargetIndex)))/(targetPoint[2]-previousPoint[2]);
        auto targetValue = m_LevelSetImage->GetPixel(m_TargetIndex);
        auto previousValue = m_LevelSetImage->GetPixel(m_Origins->GetPixel(m_TargetIndex));
        m_TargetGradient=vector*(targetValue-previousValue)/norm;

        //if(m_TargetGradient[0]>10){
        //	std::cout << m_TargetGradient << std::endl;
        //}


	}
	void SetInputImage(const typename ImageType::Pointer & input){
		m_InputImage=input;
	}
	void SetSourceIndex(const IndexType & index){
		m_SourceIndex=index;
	}
	void SetTargetIndex(const IndexType & index){
		m_TargetIndex=index;
	}
	double GetCost(){
		return m_Cost;
	}
	VectorType GetTargetGradient(){
		return m_TargetGradient;
	}
public:
	void Init(){
	    this->m_LabelImage=LabelImageType::New();
	    this->m_LabelImage->SetRegions(this->m_InputImage->GetLargestPossibleRegion());
	    this->m_LabelImage->CopyInformation(this->m_InputImage);
	    this->m_LabelImage->Allocate();


	    this->m_LevelSetImage=LevelSetImageType::New();
	    this->m_LevelSetImage->CopyInformation(this->m_InputImage);
	    this->m_LevelSetImage->SetRegions(this->m_InputImage->GetLargestPossibleRegion());
	    this->m_LevelSetImage->Allocate();

	    this->m_Origins=OriginImageType::New();
	    this->m_Origins->CopyInformation(this->m_InputImage);
	    this->m_Origins->SetRegions(this->m_InputImage->GetLargestPossibleRegion());
	    this->m_Origins->Allocate();


	}

private:

	typename ImageType::Pointer m_InputImage;
	IndexType m_SourceIndex;
	IndexType m_TargetIndex;
	std::vector<IndexType> m_PendingTargets;
	VectorType m_TargetGradient;
	typedef itk::Image<IndexType,ImageType::ImageDimension> OriginImageType;
	typename OriginImageType::Pointer m_Origins;

	double m_Cost;


	enum LabelType { FarPoint = 0, AlivePoint, VisitedPoint, InitialTrialPoint, OutsidePoint };
    typedef itk::Image<LabelType,ImageType::ImageDimension> LabelImageType;
    typedef itk::Image<double,ImageType::ImageDimension> LevelSetImageType;

    typedef itk::LevelSetTypeDefault< LevelSetImageType >            LevelSetType;
    typedef typename LevelSetType::LevelSetPointer      		  LevelSetPointer;
    typedef typename LevelSetType::PixelType            		  LevelSetPixelType;
    typedef typename LevelSetType::NodeType             		  LevelSetNodeType;

    typedef typename LevelSetNodeType::IndexType                NodeIndexType;
    typedef typename LevelSetType::NodeContainer        NodeContainer;
    typedef typename LevelSetType::NodeContainerPointer NodeContainerPointer;

    typename LabelImageType::Pointer m_LabelImage;
    typename LevelSetImageType::Pointer m_LevelSetImage;



    /** Trial points are stored in a min-heap. This allow efficient access
     * to the trial point with minimum value which is the next grid point
     * the algorithm processes. */
    typedef std::deque< LevelSetNodeType >  HeapContainer;
    typedef std::greater< LevelSetNodeType > NodeComparer;

    typedef std::priority_queue< LevelSetNodeType, HeapContainer, NodeComparer > HeapType;

    HeapType m_TrialHeap;
};

#endif
#if 0
typename TTissue::Pointer ApplyGradientToTissue(typename TTissue::Pointer & tissue, GradientContainer & vertexGradients, double alpha){
	auto newTissue = cloneTissueDescriptor<TTissue>(tissue);

	std::for_each(newTissue->GetAJGraph()->VerticesBegin(), newTissue->GetAJGraph()->VerticesEnd(), [&](const typename TTissue::AJGraphType::AJVertexHandler & vertexHandler){
		auto vertex = newTissue->GetAJGraph()->GetAJVertex(vertexHandler);
		auto position = vertex->GetPosition();

		auto position1 = position + alpha *vertexGradients[vertexHandler];
		typename PlatenessImageType::IndexType index;

		m_PlatenessImage->TransformPhysicalPointToIndex(position1, index);
		if (m_PlatenessImage->GetLargestPossibleRegion().IsInside(index)){
			vertex->SetPosition(position1);
		}
		else{
			for (int d = 0; d<3; d++){
				if (index[d] >= (long)m_PlatenessImage->GetLargestPossibleRegion().GetSize(d)){
					//std::cout << "Adjusting " << index << " to ";
					index[d] = m_PlatenessImage->GetLargestPossibleRegion().GetSize(d) - 1;
					//std::cout << index << std::endl;
				}
				if (index[d]<0){
					//std::cout << "Adjusting " << index << " to ";

					index[d] = 0;
					//std::cout << index << std::endl;
				}
			}
			decltype(position) position2;
			m_PlatenessImage->TransformIndexToPhysicalPoint(index, position2);
			vertex->SetPosition(position2);
		}
	});
	return newTissue;
}

std::tuple<double, GradientContainer> Eval(const typename TTissue::Pointer & tissue){

	//#define DO_AREA
#define DO_VERTEXNESS
#define DO_PLANAR
	//#define DO_PERIMETER
#define DO_EDGELENGTHS
	GradientContainer vertexGradients;

	double e = 0;


	std::for_each(tissue->GetAJGraph()->VerticesBegin(), tissue->GetAJGraph()->VerticesEnd(), [&vertexGradients](const typename TTissue::AJGraphType::AJVertexHandler & vertex){
		VectorType vector;
		vector.Fill(0);
		vertexGradients[vertex] = vector;
	});
	//Function E= P(T) + E(A) +E(P) -E(l)

	//PathCost<PlatenessImageType> pathCost;
	//pathCost.SetInputImage(m_PlatenessImage);
	//pathCost.Init();



	double per = 0;
	double areas = 0;
	double eta = 0;
	double w_per = 1e3;
	double w_areas = 1;
	double w_v = 1;
	double w_l = 2;

	double w_d = 2e2;
#ifdef DO_VERTEXNESS

	//double weigthV=0;
	typedef itk::LinearInterpolateImageFunction<VertexnessImageType, double> InterpolatorType;
	typedef itk::VectorLinearInterpolateImageFunction<typename GradientFilterType::OutputImageType, double> GradientInterpolatorType;
	typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
	interpolator->SetInputImage(m_PlatenessImage);
	typename GradientInterpolatorType::Pointer gradientInterpolator = GradientInterpolatorType::New();
	gradientInterpolator->SetInputImage(m_GradientFilter->GetOutput());

	std::for_each(tissue->GetAJGraph()->VerticesBegin(), tissue->GetAJGraph()->VerticesEnd(), [&](const typename TTissue::AJGraphType::AJVertexHandler & vertexHandler){
		auto vertex = tissue->GetAJGraph()->GetAJVertex(vertexHandler);

		//std::cout << vertex->GetPosition() << std::endl;

		eta -= w_v*interpolator->Evaluate(vertex->GetPosition());
		auto vector = gradientInterpolator->Evaluate(vertex->GetPosition());
		itk::Vector<double, 3> gradient;
		gradient[0] = vector[0] * w_v;
		gradient[1] = vector[1] * w_v;
		gradient[2] = vector[2] * w_v;
		vertexGradients[vertexHandler] += gradient;
		//std::cout << gradient;
	});
	std::cout << "ETA " << eta << std::endl;
	e = e + eta;

#endif
#ifdef DO_PLANAR

	double gamma = 0;

	std::for_each(tissue->GetAJGraph()->VerticesBegin(), tissue->GetAJGraph()->VerticesEnd(), [&](const typename TTissue::AJGraphType::AJVertexHandler & vertexHandler){

		if (tissue->GetAJGraph()->AJVertexDegree(vertexHandler) >= 3){
			auto vertex = tissue->GetAJGraph()->GetAJVertex(vertexHandler);
			auto position = vertex->GetPosition();
			auto normalAndMean = embeddingPlane<typename TTissue::AJGraphType::AdjacencyIterator, typename TTissue::AJGraphType>(tissue->GetAJGraph()->AdjacentAJVerticesBegin(vertexHandler), tissue->GetAJGraph()->AdjacentAJVerticesEnd(vertexHandler), tissue->GetAJGraph());


			auto normal = std::get<0>(normalAndMean);
			auto mean = std::get<1>(normalAndMean);


			auto v = position -mean;

			double dist = normal*v;

			//auto project = position -dist*normal;

			//std::cout << position << " " << normal << " " << mean << " " << B << std::endl;
			//double dist=(position-project).GetSquaredNorm();
			gamma+=w_d*std::abs(dist);
			//auto gradient = 2*w_d*(position-project).GetNorm()*normal;
			auto gradient = 2*w_d*dist*normal;
			//std::cout << gradient;
			vertexGradients[vertexHandler]+=-gradient;

		}
	});

	std::cout << "Dist " << gamma << std::endl;
	e= e+gamma;
#endif


#ifdef DO_PLATENESS
	double phi=0;

	double w_e=1e-2;
	std::for_each(tissue->GetAJGraph()->EdgesBegin(),tissue->GetAJGraph()->EdgesEnd(),[&](const typename TTissue::AJGraphType::AJEdgeHandler & edge){

		auto sourceHandler = tissue->GetAJGraph()->GetAJEdgeSource(edge);
		auto targetHandler = tissue->GetAJGraph()->GetAJEdgeTarget(edge);

		auto source = tissue->GetAJGraph()->GetAJVertex(sourceHandler);
		auto target = tissue->GetAJGraph()->GetAJVertex(targetHandler);

		auto sourcePosition = source->GetPosition();
		auto targetPosition = target->GetPosition();

		typename PlatenessImageType::IndexType sourceIndex,targetIndex;

		m_PlatenessImage->TransformPhysicalPointToIndex(sourcePosition,sourceIndex);
		m_PlatenessImage->TransformPhysicalPointToIndex(targetPosition,targetIndex);


		pathCost.SetSourceIndex(sourceIndex);
		pathCost.SetTargetIndex(targetIndex);

		pathCost.Do();
		double cost = pathCost.GetCost();
		phi=phi-w_e*cost;

		vertexGradients[targetHandler]+= w_e*pathCost.GetTargetGradient();

		pathCost.SetSourceIndex(targetIndex);
		pathCost.SetTargetIndex(sourceIndex);

		pathCost.Do();
		cost = pathCost.GetCost();

		vertexGradients[sourceHandler]+= w_e*pathCost.GetTargetGradient();

		phi=phi-w_e*cost;



		//std::cout << "Cost: " << cost << std::endl;


		//e=e+

	});
	e=e+phi;
	std::cout << "PHI: " <<phi << std::endl;
#endif

#ifdef DO_EDGELENGTHS


	double l=0;
	std::for_each(tissue->GetAJGraph()->EdgesBegin(),tissue->GetAJGraph()->EdgesEnd(),[&](const typename TTissue::AJGraphType::AJEdgeHandler & edge){

		auto sourceHandler = tissue->GetAJGraph()->GetAJEdgeSource(edge);
		auto targetHandler = tissue->GetAJGraph()->GetAJEdgeTarget(edge);

		auto source = tissue->GetAJGraph()->GetAJVertex(sourceHandler);
		auto target = tissue->GetAJGraph()->GetAJVertex(targetHandler);

		auto sourcePosition = source->GetPosition();
		auto targetPosition = target->GetPosition();

		auto lineVector = (targetPosition-sourcePosition);

		l = l+w_l*lineVector.GetNorm();
		auto gradient =w_l*lineVector;
		//std::cout << gradient;

		vertexGradients[sourceHandler]-=gradient;
		vertexGradients[targetHandler]-=-gradient;
	});

	std::cout << "L: " <<l << std::endl;
	e+=l;
#endif

	std::for_each(tissue->GetCellGraph()->CellsBegin(),tissue->GetCellGraph()->CellsEnd(),[&](const typename TTissue::CellGraphType::CellVertexHandler & cellHandler){
		if(cellHandler!=tissue->GetCellGraph()->GetUnboundedFace()){
			auto cell = tissue->GetCellGraph()->GetCell(cellHandler);
#ifdef DO_PERIMETER
			for(auto perimeterEdgeIt=cell->BeginPerimeterAJEdges();perimeterEdgeIt!=cell->EndPerimeterAJEdges();++perimeterEdgeIt){
				auto edgeHandler=*perimeterEdgeIt;
				auto sourceHandler=tissue->GetAJGraph()->GetAJEdgeSource(edgeHandler);
				auto targetHandler=tissue->GetAJGraph()->GetAJEdgeTarget(edgeHandler);

				auto source = tissue->GetAJGraph()->GetAJVertex(sourceHandler);
				auto target = tissue->GetAJGraph()->GetAJVertex(targetHandler);

				auto sourcePosition = source->GetPosition();
				auto targetPosition = target->GetPosition();

				auto lineVector = (targetPosition-sourcePosition);
				per+=w_per*lineVector.GetSquaredNorm();

				auto gradient =- 2.0*w_per*lineVector*lineVector.GetNorm();
				//std::cout << gradient;
				vertexGradients[sourceHandler]+=-gradient;
				vertexGradients[targetHandler]+=gradient;
			}
#endif

#ifdef DO_AREA

			auto normalAndMean=embeddingPlane<typename TTissue::CellGraphType::CellType::PerimeterVertexIterator,typename TTissue::AJGraphType>(cell->BeginPerimeterAJVertices(), cell->EndPerimeterAJVertices(),tissue->GetAJGraph());
			itk::Vector<double,3> total;
			total.Fill(0);

			for(auto perimeterEdgeIt = cell->BeginPerimeterAJEdges();perimeterEdgeIt!=cell->EndPerimeterAJEdges();++perimeterEdgeIt){

				auto next = std::next(perimeterEdgeIt);

				if(next==cell->EndPerimeterAJEdges()){
					next = cell->BeginPerimeterAJEdges();
				}
				auto source = tissue->GetAJGraph()->GetAJEdgeSource(*perimeterEdgeIt);
				auto target = tissue->GetAJGraph()->GetAJEdgeTarget(*perimeterEdgeIt);

				auto sourceN =tissue->GetAJGraph()->GetAJEdgeSource(*next);
				auto targetN =tissue->GetAJGraph()->GetAJEdgeTarget(*next);

				itk::Point<double,3> a,b,c;
				typename TTissue::AJGraphType::AJVertexHandler cHandler;
				if(source==sourceN){
					a = tissue->GetAJGraph()->GetAJVertex(target)->GetPosition();
					b = tissue->GetAJGraph()->GetAJVertex(targetN)->GetPosition();
					c = tissue->GetAJGraph()->GetAJVertex(source)->GetPosition();
					cHandler=source;
				}else if(source==targetN){
					a = tissue->GetAJGraph()->GetAJVertex(target)->GetPosition();
					b = tissue->GetAJGraph()->GetAJVertex(sourceN)->GetPosition();
					c = tissue->GetAJGraph()->GetAJVertex(source)->GetPosition();
					cHandler=source;
				}else if(target == sourceN){
					a = tissue->GetAJGraph()->GetAJVertex(source)->GetPosition();
					b = tissue->GetAJGraph()->GetAJVertex(targetN)->GetPosition();
					c = tissue->GetAJGraph()->GetAJVertex(target)->GetPosition();
					cHandler=target;

				}else if(target == targetN){
					a = tissue->GetAJGraph()->GetAJVertex(source)->GetPosition();
					b = tissue->GetAJGraph()->GetAJVertex(sourceN)->GetPosition();
					c = tissue->GetAJGraph()->GetAJVertex(target)->GetPosition();
					cHandler=target;
				}

				auto CA= c-a;
				auto CB= c-b;
				if(CA.GetNorm()==0){
					std::cout << "problem" << std::endl;
				}
				if(CB.GetNorm()==0){
					std::cout << "problem" << std::endl;
				}


				auto bisector = (CA+CB)/2;

				auto N=itk::CrossProduct(CA,CB);
				auto gradient = w_areas*bisector;
				//std::cout << gradient;
				vertexGradients[cHandler]+= gradient;
				total+=N;
			}
			double area=std::abs(total*std::get<0>(normalAndMean))/2;
			areas+=-w_areas*area;
#endif
		}

	});
	std::cout << "PER: " <<per << std::endl;
	std::cout << "AREA: " <<areas << std::endl;
	e = e + per;
	e = e + areas;




	//std::cout << "E: " <<e << std::endl;


	return std::make_tuple(e,vertexGradients);

}

#endif
#endif /* TTTENERGYMODELTISSUESMOOTHER_H_ */
