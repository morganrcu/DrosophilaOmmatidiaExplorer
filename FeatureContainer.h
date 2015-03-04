#ifndef FEATURECONTAINER_H
#define FEATURECONTAINER_H

#include <memory>
#include <FeatureDescriptor.h>
template<class TObject,class TFeature> class FeatureContainer : public itk::DataObject{
private:

	typedef std::map<TObject,typename FeatureDescriptor<TFeature>::Pointer > FeatureMap;

public:

	typedef FeatureContainer Self;
	typedef itk::DataObject Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef typename FeatureMap::iterator FeatureIterator;

	itkNewMacro(Self)

	void AddFeature(const TObject & object, const typename FeatureDescriptor<TFeature>::Pointer & feature){
		m_FeatureMap[object]=feature;
	}
	const typename FeatureDescriptor<TFeature>::Pointer GetFeature(const TObject & object){
		return m_FeatureMap[object];
	}

	bool HasFeature(const TObject & object){
		return m_FeatureMap.find(object)!=m_FeatureMap.end();

	}

	FeatureIterator FeaturesBegin(){
		return m_FeatureMap.begin();
	}
	FeatureIterator FeaturesEnd(){
		return m_FeatureMap.end();
	}
protected:
	FeatureContainer(){

	}



	FeatureMap m_FeatureMap;

};

#endif
