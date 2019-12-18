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
#include <EVENT/Cluster.h>

// -- std headers
#include <map>
#include <functional>

// -- root headers
#include <ROOT/REvePointSet.hxx>

namespace lceve {
  
  class LCClusterConverter : public ICollectionConverter {
  public:
    /// Default constructor
    LCClusterConverter() ;
    
    ///  Create tracks out of EVENT::Track objects
    ROOT::REveElement* ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) override ;
    
  private:
    using SortFunctionMap_t = LCIOHelper::SortFunctionMap_t<EVENT::Cluster> ;    
    SortFunctionMap_t                fSortFunctions {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  LCClusterConverter::LCClusterConverter() {
    fSortFunctions[ "None" ] = [](const EVENT::Cluster *, const EVENT::Cluster *) {
      return false ; 
    } ;
    fSortFunctions[ "Energy" ] = [](const EVENT::Cluster *lhs, const EVENT::Cluster *rhs ) { 
      return lhs->getEnergy() > rhs->getEnergy() ;
    } ;
  }
  
  //--------------------------------------------------------------------------
  
  ROOT::REveElement* LCClusterConverter::ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) {
    if( collection->getTypeName() != EVENT::LCIO::CLUSTER ) {
      std::cout << "ERROR: Expected collection type EVENT::LCIO::CLUSTER, got " << collection->getTypeName() << std::endl ;
      return nullptr ;
    }
    std::cout << "Converting cluster collection " << name << std::endl ;
    auto clusters = LCIOHelper::CollectionAsVector<EVENT::Cluster>( collection ) ;
    
    // Cluster coloring
    auto color = GetParameter<std::string>( "Color" ).value_or( "iter" ) ;
    auto colorFunctor = ColorHelper::GetColorFunction( color ) ;
    
    // Sort cluster
    auto sortPolicy = GetParameter<std::string>( "SortPolicy" ).value_or( "None" ) ;
    auto policyIter = fSortFunctions.find( sortPolicy ) ;
    if( fSortFunctions.end() == policyIter ) {
      policyIter = fSortFunctions.find( "None" ) ;
    }
    std::sort( clusters.begin(), clusters.end(), policyIter->second ) ;
    
    LCObjectFactory lcFactory( this->GetEventDisplay() ) ;
    EveElementFactory eveFactory( this->GetEventDisplay() ) ;
    
    auto eveClusterList = eveFactory.CreateClusterContainer() ;
    eveClusterList->SetName( name ) ;
    eveClusterList->SetMainColor( kPink ) ;

    for( auto lcCluster : clusters ) {
      auto params = lcFactory.ConvertCluster( lcCluster ) ;
      auto attr = params.fMarkerAttributes.value_or( MarkerAttributes() ) ;
      attr.fColor = colorFunctor() ;
      params.fMarkerAttributes = attr ;
      auto eveCluster = eveFactory.CreateCluster( params ) ;
      eveClusterList->AddElement( eveCluster ) ;
    }
    return eveClusterList.release() ;
  }
  
}

using namespace lceve ;
// Declare converter plugin
LCEVE_DECLARE_CONVERTER_NS(lceve, LCClusterConverter)
