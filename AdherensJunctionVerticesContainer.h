#ifndef ADHERENSJUNCTIONVERTICESCONTAINER_H
#define ADHERENSJUNCTIONVERTICESCONTAINER_H
#include <itkDataObject.h>
#include <itkMacro.h>
template<class TAdherensJunctionVertex> class AdherensJunctionVerticesContainer : public itk::DataObject{
public:

    typedef AdherensJunctionVerticesContainer Self;
    typedef itk::SmartPointer<Self> Pointer;

    itkNewMacro(Self)

    typedef TAdherensJunctionVertex AdherensJunctionVertexType;

    typedef std::vector<typename AdherensJunctionVertexType::Pointer> Container;
    Container m_Vertices;

    typedef typename Container::iterator Iterator;


    void AddVertex(const typename AdherensJunctionVertexType::Pointer & vertex){
        m_Vertices.push_back(vertex);
    }

    Iterator Begin(){
        return m_Vertices.begin();
    }
    Iterator End(){
        return m_Vertices.end();
    }

    inline unsigned int GetNumVertices(){
        return m_Vertices.size();
    }

protected:

    AdherensJunctionVerticesContainer(){


    }




};

#endif // ADHERENSJUNCTIONVERTICESCONTAINER_H
