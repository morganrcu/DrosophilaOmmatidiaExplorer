/*
 * FeatureDescriptor.h
 *
 *  Created on: Mar 3, 2015
 *      Author: morgan
 */

#ifndef FEATUREDESCRIPTOR_H_
#define FEATUREDESCRIPTOR_H_
#include <itkDataObject.h>
template<class T> class FeatureDescriptor : public itk::DataObject{
public:
	typedef FeatureDescriptor<T> Self;
	typedef itk::SmartPointer<Self> Pointer;
	itkNewMacro(Self)
	itkGetMacro(Value,T)
	itkSetMacro(Value,T)
protected:
	FeatureDescriptor(){

	}
	~FeatureDescriptor(){

	}
private:
	T m_Value;
};



#endif /* FEATUREDESCRIPTOR_H_ */
