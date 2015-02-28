#ifndef PREDICTAJGRAPH_H
#define PREDICTAJGRAPH_H

#include <itkObject.h>

template<class TInputGraph,class TMotionImage,class TOutputGraph> class PredictAJGraph : public itk::Object{


public:
    typedef PredictAJGraph Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer<Self> Pointer;

    itkNewMacro(Self)

    itkGetObjectMacro(InputGraph,TInputGraph)
    itkSetObjectMacro(InputGraph,TInputGraph)

    itkGetObjectMacro(OutputGraph,TOutputGraph)
    itkSetObjectMacro(OutputGraph,TOutputGraph)

    itkGetObjectMacro(MotionImage,TMotionImage)
    itkSetObjectMacro(MotionImage,TMotionImage)

    void Compute(){
        m_OutputGraph=TOutputGraph::New();

        for(typename TInputGraph::VertexIterator it= m_InputGraph->VerticesBegin();it!=m_InputGraph->VerticesEnd();++it){

            typename TInputGraph::AJVertexType::Pointer vertex = m_InputGraph->GetAJVertex(*it);
            typename TOutputGraph::AJVertexType::Pointer nextVertex = TOutputGraph::AJVertexType::New();

            typename TInputGraph::AJVertexType::PointType position = vertex->GetPosition();

            typename TMotionImage::IndexType index;

            this->m_MotionImage->TransformPhysicalPointToIndex(position,index);

            typename TOutputGraph::AJVertexType::PointType nextPosition = position+this->m_MotionImage->GetPixel(index);

            nextVertex->SetPosition(nextPosition);
            m_OutputGraph->AddAJVertex(nextVertex);
        }

        for(typename TInputGraph::EdgeIterator it = m_InputGraph->EdgesBegin();it!=m_InputGraph->EdgesEnd();++it){
            typename TInputGraph::AJVertexHandler source = m_InputGraph->GetAJEdgeSource(*it);
            typename TInputGraph::AJVertexHandler target = m_InputGraph->GetAJEdgeTarget(*it);

            m_OutputGraph->AddAJEdge(source,target);
        }
    }
protected:
    PredictAJGraph(){

    }

private:

    typename TInputGraph::Pointer m_InputGraph;
    typename TOutputGraph::Pointer m_OutputGraph;
    typename TMotionImage::Pointer m_MotionImage;

};

#endif // PREDICTAJGRAPH_H
