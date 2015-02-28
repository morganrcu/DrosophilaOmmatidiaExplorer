#ifndef AJGRAPH_H
#define AJGRAPH_H

#include <itkDataObject.h>
#include <itkMacro.h>
#include <itkObjectFactory.h>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

template<class TAJVertex> class AJGraph : public itk::DataObject {


private:

    struct AJVertexPropertyTag {
        typedef boost::vertex_property_tag kind;
    };

    typedef boost::property<AJVertexPropertyTag, typename TAJVertex::Pointer,boost::property<boost::vertex_index_t, int> >  VertexProperty;
    typedef boost::property<boost::edge_index_t,int> EdgeProperty;

    typedef boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,VertexProperty,EdgeProperty> BoostGraphType;


    BoostGraphType m_Graph;
public:

    typedef AJGraph<TAJVertex> Self;
    typedef itk::DataObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;


    typedef TAJVertex AJVertexType;


    typedef typename boost::graph_traits<BoostGraphType>::vertex_descriptor AJVertexHandler;
    typedef typename boost::graph_traits<BoostGraphType>::edge_descriptor AJEdgeHandler;

    typedef typename boost::graph_traits<BoostGraphType>::vertex_iterator VertexIterator;
    typedef typename boost::graph_traits<BoostGraphType>::edge_iterator EdgeIterator;

    typedef typename boost::graph_traits<BoostGraphType>::adjacency_iterator AdjacencyIterator;
public:

    itkNewMacro(Self)

   virtual  AJVertexHandler GetAJEdgeSource(const AJEdgeHandler & source){
        return boost::source(source,m_Graph);
    }
    virtual AJEdgeHandler GetAJEdgeHandler(const AJVertexHandler & source,const AJVertexHandler & target){
        return boost::edge(source,target,m_Graph).first;

    }

    virtual AJVertexHandler GetAJEdgeTarget(const AJEdgeHandler & target){
        return boost::target(target,m_Graph);
    }

   virtual typename AJVertexType::Pointer GetAJVertex(const AJVertexHandler & vertex){
        return boost::get(AJVertexPropertyTag(),m_Graph,vertex);

    }

    virtual int AJVertexDegree(const AJVertexHandler & vertex){
        return boost::degree(vertex,m_Graph);
    }

    virtual AdjacencyIterator AdjacentAJVerticesBegin(const AJVertexHandler & vertex){
        return boost::adjacent_vertices(vertex,m_Graph).first;
    }

    virtual AdjacencyIterator AdjacentAJVerticesEnd(const AJVertexHandler & vertex){
        return boost::adjacent_vertices(vertex,m_Graph).second;
    }

   virtual AJVertexHandler AddAJVertex(const typename AJVertexType::Pointer & vertex){

        AJVertexHandler result=boost::add_vertex(m_Graph);

        boost::get(AJVertexPropertyTag(),m_Graph,result)=vertex;
        return result;

    }
    virtual void DeleteAJVertex(const AJVertexHandler & vertex){
        boost::clear_vertex(vertex,m_Graph);
        boost::remove_vertex(vertex,m_Graph);
    }

    virtual void DeleteAJEdge(const AJEdgeHandler & edge){
        boost::remove_edge(edge,m_Graph);
    }


    virtual AJEdgeHandler AddAJEdge(const AJVertexHandler & a, const AJVertexHandler & b){
        return boost::add_edge(a,b,m_Graph).first;
    }

    unsigned long GetNumVertices(){
        return boost::num_vertices(m_Graph);
    }

    unsigned long GetNumEdges(){
        return boost::num_edges(m_Graph);
    }

    EdgeIterator EdgesBegin(){
        return boost::edges(m_Graph).first;
    }


    EdgeIterator EdgesEnd(){
        return boost::edges(m_Graph).second;
    }

    VertexIterator VerticesBegin(){
        return boost::vertices(m_Graph).first;
    }

    VertexIterator VerticesEnd(){
        return boost::vertices(m_Graph).second;
    }



protected:
    AJGraph(){

    }

};

#endif // AJGRAPH_H
