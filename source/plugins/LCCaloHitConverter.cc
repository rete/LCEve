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

// -- std headers
#include <map>
#include <functional>

// -- root headers
#include <ROOT/REvePointSet.hxx>

namespace lceve {
  
  class LCCaloHitConverter : public ICollectionConverter {
  public:
    /// Default constructor
    LCCaloHitConverter() ;
    
    ///  Create tracks out of EVENT::Track objects
    ROOT::REveElement* ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) override ;
    
  private:
    using SortFunctionMap_t = LCIOHelper::SortFunctionMap_t<EVENT::CalorimeterHit> ;    
    SortFunctionMap_t                fSortFunctions {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  LCCaloHitConverter::LCCaloHitConverter() {
    fSortFunctions[ "None" ] = [](const EVENT::CalorimeterHit *, const EVENT::CalorimeterHit *) {
      return false ; 
    } ;
    fSortFunctions[ "Energy" ] = [](const EVENT::CalorimeterHit *lhs, const EVENT::CalorimeterHit *rhs ) { 
      return lhs->getEnergy() > rhs->getEnergy() ;
    } ;
  }
  
  //--------------------------------------------------------------------------
  
  ROOT::REveElement* LCCaloHitConverter::ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) {
    if( collection->getTypeName() != EVENT::LCIO::CLUSTER ) {
      std::cout << "ERROR: Expected collection type EVENT::LCIO::CLUSTER, got " << collection->getTypeName() << std::endl ;
      return nullptr ;
    }
    std::cout << "Converting cluster collection " << name << std::endl ;
    auto caloHits = LCIOHelper::CollectionAsVector<EVENT::CalorimeterHit>( collection ) ;
    
    // CalorimeterHit coloring
    auto color = GetParameter<std::string>( "Color" ).value_or( "iter" ) ;
    auto colorFunctor = ColorHelper::GetColorFunction( color ) ;
    
    // Sort cluster
    auto sortPolicy = GetParameter<std::string>( "SortPolicy" ).value_or( "None" ) ;
    auto policyIter = fSortFunctions.find( sortPolicy ) ;
    if( fSortFunctions.end() == policyIter ) {
      policyIter = fSortFunctions.find( "None" ) ;
    }
    std::sort( caloHits.begin(), caloHits.end(), policyIter->second ) ;
    
    LCObjectFactory lcFactory( this->GetEventDisplay() ) ;
    EveElementFactory eveFactory( this->GetEventDisplay() ) ;
    
    auto eveCaloHitList = eveFactory.CreateCaloHitContainer() ;
    eveCaloHitList->SetName( name ) ;
    eveCaloHitList->SetMainColor( kPink ) ;
    eveCaloHitList->SetMarkerColor( colorFunctor() ) ;
    eveCaloHitList->SetMarkerSize( 3 ) ;
    eveCaloHitList->SetMarkerStyle( 4 ) ;
    
    auto params = lcFactory.ConvertCaloHits( caloHits ) ;
    eveFactory.PopulateCaloHits( eveCaloHitList.get(), params ) ;

    return eveCaloHitList.release() ;
  }
  
}

using namespace lceve ;
// Declare converter plugin
LCEVE_DECLARE_CONVERTER_NS(lceve, LCCaloHitConverter)
