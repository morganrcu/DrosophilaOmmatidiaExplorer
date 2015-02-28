#ifndef ADHERENSJUNCTIONVERTEX_H
#define ADHERENSJUNCTIONVERTEX_H
#include <itkDataObject.h>
#include <itkPoint.h>
template<int dim = 3> class AdherensJunctionVertex : public itk::DataObject{
public:
    /**
     * Location of the point in pixel coordinates
     */
    typedef itk::Point<double,dim> PointType;
    typedef AdherensJunctionVertex<dim> Self;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef itk::DataObject Superclass;

    itkTypeMacro(Self,Superclass)
    itkNewMacro(Self)

    itkGetMacro(Position,PointType)
    itkSetMacro(Position,PointType)

protected:

    /**
     * Default constructor. The attributes are fixed to default values
     */
    AdherensJunctionVertex() {
        m_Position.Fill(-1);
    }
    /**
     * Position of the point in 3D pixel coordinates
     */

    PointType m_Position;

private:
    AdherensJunctionVertex(const Self &);

};
#endif // ADHERENSJUNCTIONVERTEX_H
