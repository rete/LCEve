// -- lceve headers
#include <LCEve/ICollectionConverter.h>
#include <LCEve/LCObjectFactory.h>
#include <LCEve/EveElementFactory.h>
#include <LCEve/LCIOHelper.h>
#include <LCEve/DrawAttributes.h>
#include <LCEve/Geometry.h>
#include <LCEve/Factories.h>

// -- lcio headers
#include <EVENT/LCCollection.h>
#include <EVENT/CalorimeterHit.h>
#include <EVENT/SimCalorimeterHit.h>
#include <UTIL/LCIOTypeInfo.h>

// -- std headers
#include <map>
#include <functional>

// -- root headers
#include <ROOT/REvePointSet.hxx>

namespace lceve {
  
  template <typename T>
  class LCCaloHitConverter : public ICollectionConverter {
  public:
    /// Default constructor
    LCCaloHitConverter() ;
    
    ///  Create tracks out of EVENT::Track objects
    ROOT::REveElement* ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) override ;
    
    /// Get the default marker style
    int GetDefaultMarkerStyle() const ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  template <typename T>
  LCCaloHitConverter<T>::LCCaloHitConverter() {
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  ROOT::REveElement* LCCaloHitConverter<T>::ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) {
    std::string typeName = UTIL::lctypename<T>() ;
    if( collection->getTypeName() != typeName ) {
      std::cout << "ERROR: Expected collection type " << typeName << " , got " << collection->getTypeName() << std::endl ;
      return nullptr ;
    }
    auto caloHits = LCIOHelper::CollectionAsVector<T>( collection ) ;
    
    // hit coloring
    auto color = GetParameter<std::string>( "Color" ).value() ;
    auto colorFunctor = ColorHelper::GetColorFunction( color ) ;
    
    LCObjectFactory lcFactory( this->GetEventDisplay() ) ;
    EveElementFactory eveFactory( this->GetEventDisplay() ) ;
    
    auto eveCaloHitList = eveFactory.CreateCaloHitContainer() ;
    eveCaloHitList->SetName( name ) ;
    eveCaloHitList->SetMainColor( kPink ) ;
    eveCaloHitList->SetMarkerColor( colorFunctor() ) ;
    eveCaloHitList->SetMarkerSize( GetParameter<int>( "MarkerSize" ).value_or( 3 ) ) ;
    eveCaloHitList->SetMarkerStyle( GetParameter<int>( "MarkerStyle" ).value_or( GetDefaultMarkerStyle() ) ) ;
    
    auto params = lcFactory.ConvertCaloHits( caloHits ) ;
    eveFactory.PopulateCaloHits( eveCaloHitList.get(), params ) ;

    return eveCaloHitList.release() ;
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  int LCCaloHitConverter<T>::GetDefaultMarkerStyle() const {
    return (UTIL::lctypename<T>() == EVENT::LCIO::SIMCALORIMETERHIT) ? 5 : 4 ;
  }
  
  //--------------------------------------------------------------------------
  
  using LCCalorimeterHitConverter = LCCaloHitConverter<EVENT::CalorimeterHit> ;
  using LCSimCalorimeterHitConverter = LCCaloHitConverter<EVENT::SimCalorimeterHit> ;
}

using namespace lceve ;
// Declare converter plugin
LCEVE_DECLARE_CONVERTER_NS(lceve, LCCalorimeterHitConverter)
LCEVE_DECLARE_CONVERTER_NS(lceve, LCSimCalorimeterHitConverter)
