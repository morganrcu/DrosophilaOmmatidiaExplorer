#ifndef PREDICTAJGRAPH_H
#define PREDICTAJGRAPH_H

#include <itkObject.h>

template<class TInputGraph,class TMotionImage,class TOutputGraph> class PredictAJGraph : public itk::Object{


public:
    typedef PredictAJGraph Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef AJCorrespondenceSet<typename TInputGraph::AJVertexHandler> AJCorrespondenceType;
    itkNewMacro(Self)

    itkGetObjectMacro(InputGraph,TInputGraph)
    itkSetObjectMacro(InputGraph,TInputGraph)

    itkGetObjectMacro(OutputGraph,TOutputGraph)
    itkSetObjectMacro(OutputGraph,TOutputGraph)

    itkGetObjectMacro(MotionImage,TMotionImage)
    itkSetObjectMacro(MotionImage,TMotionImage)

	itkGetObjectMacro(Correspondences,AJCorrespondenceType);

    void Compute(){
        m_OutputGraph=TOutputGraph::New();

        for(typename TInputGraph::VertexIterator it= m_InputGraph->VerticesBegin();it!=m_InputGraph->VerticesEnd();++it){

            auto vertexHandler=*it;
        	typename TInputGraph::AJVertexType::Pointer vertex = m_InputGraph->GetAJVertex(vertexHandler);
            typename TOutputGraph::AJVertexType::Pointer nextVertex = TOutputGraph::AJVertexType::New();

            typename TInputGraph::AJVertexType::PointType position = vertex->GetPosition();

            typename TMotionImage::IndexType index;

            this->m_MotionImage->TransformPhysicalPointToIndex(position,index);

            typename TOutputGraph::AJVertexType::PointType nextPosition = position+this->m_MotionImage->GetPixel(index);

            nextVertex->SetPosition(nextPosition);
            auto nextVertexHandler = m_OutputGraph->AddAJVertex(nextVertex);

#if 0
            typename AJCorrespondenceType::SuccesorType succesor(vertexHandler,nextVertexHandler);

            m_Correspondences->AddSuccesor(succesor);
#endif
        }

        for(typename TInputGraph::EdgeIterator it = m_InputGraph->EdgesBegin();it!=m_InputGraph->EdgesEnd();++it){
            typename TInputGraph::AJVertexHandler source = m_InputGraph->GetAJEdgeSource(*it);
            typename TInputGraph::AJVertexHandler target = m_InputGraph->GetAJEdgeTarget(*it);

            auto nextEdgeHandler = m_OutputGraph->AddAJEdge(source,target);
        }
    }
protected:
    PredictAJGraph(){

    }

private:

    AJCorrespondenceType m_Correspondences;
    typename TInputGraph::Pointer m_InputGraph;
    typename TOutputGraph::Pointer m_OutputGraph;
    typename TMotionImage::Pointer m_MotionImage;

};

#endif // PREDICTAJGRAPH_H
