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

// -- root headers
#include <ROOT/REveTrack.hxx>

// -- std headers
#include <map>
#include <functional>

namespace lceve {
  
  class LCTrackConverter : public ICollectionConverter {
  public:
    /// Default constructor
    LCTrackConverter() ;
    
    ///  Create tracks out of EVENT::Track objects
    ROOT::REveElement* ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) override ;
    
  private:
    using SortTracksFunction_t = std::function<bool( const EVENT::Track *, const EVENT::Track * )> ;
    using SortTracksFunctionMap_t = std::map<std::string, SortTracksFunction_t> ;
    
    SortTracksFunctionMap_t         fSortTracksFunctions {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  LCTrackConverter::LCTrackConverter() {
    fSortTracksFunctions[ "None" ] = [](const EVENT::Track *, const EVENT::Track *) {
      return false ; 
    } ;
    fSortTracksFunctions[ "Momentum" ] = [this](const EVENT::Track *lhs, const EVENT::Track *rhs ) { 
      auto lhsMomentum = LCIOHelper::ComputeTrackMomentum(
        this->GetEventDisplay()->GetGeometry()->GetBField(), lhs->getTrackState( EVENT::TrackState::AtFirstHit ) ) ;
      auto rhsMomentum = LCIOHelper::ComputeTrackMomentum( 
        this->GetEventDisplay()->GetGeometry()->GetBField(), rhs->getTrackState( EVENT::TrackState::AtFirstHit ) ) ;
      return ( rhsMomentum.Mag() < lhsMomentum.Mag() ) ;
    } ;
    fSortTracksFunctions[ "D0" ] = [](const EVENT::Track *lhs, const EVENT::Track *rhs ) { 
      return lhs->getD0() > rhs->getD0() ;
    } ;
    fSortTracksFunctions[ "Z0" ] = [](const EVENT::Track *lhs, const EVENT::Track *rhs ) { 
      return lhs->getZ0() > rhs->getZ0() ;
    } ;
    fSortTracksFunctions[ "Chi2" ] = [](const EVENT::Track *lhs, const EVENT::Track *rhs ) { 
      return lhs->getChi2() > rhs->getChi2() ;
    } ;
  }
  
  //--------------------------------------------------------------------------
  
  ROOT::REveElement* LCTrackConverter::ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) {
    if( collection->getTypeName() != EVENT::LCIO::TRACK ) {
      std::cout << "ERROR: Expected collection type EVENT::LCIO::TRACK, got " << collection->getTypeName() << std::endl ;
      return nullptr ;
    }
    std::cout << "Converting track collection " << name << std::endl ;
    auto tracks = LCIOHelper::CollectionAsVector<EVENT::Track>( collection ) ;
    
    // Track coloring
    auto color = GetParameter<std::string>( "Color" ).value_or( "iter" ) ;
    auto colorFunctor = ColorHelper::GetColorFunction( color ) ;
    
    // Sort tracks
    auto sortPolicy = GetParameter<std::string>( "SortPolicy" ).value_or( "None" ) ;
    auto policyIter = fSortTracksFunctions.find( sortPolicy ) ;
    if( fSortTracksFunctions.end() == policyIter ) {
      policyIter = fSortTracksFunctions.find( "None" ) ;
    }
    std::sort( tracks.begin(), tracks.end(), policyIter->second ) ;
    
    LCObjectFactory lcFactory( this->GetEventDisplay() ) ;
    EveElementFactory eveFactory( this->GetEventDisplay() ) ;
    
    auto propagator = GetEventDisplay()->GetGeometry()->CreateTrackPropagator() ;
    auto eveTrackList = eveFactory.CreateTrackContainer() ;
    eveTrackList->SetName( name ) ;
    eveTrackList->SetMainColor(kTeal);

    for( auto lcTrack : tracks ) {
      auto params = lcFactory.ConvertTrack( lcTrack ) ;
      auto attr = params.fLineAttributes.value_or( LineAttributes {} ) ;
      attr.fColor = colorFunctor() ;
      params.fLineAttributes = attr ;
      auto eveTrack = eveFactory.CreateTrack( propagator, params ) ;
      eveTrackList->AddElement( eveTrack ) ;
    }
    return eveTrackList.release() ;
  }
  
}

using namespace lceve ;
// Declare converter plugin
LCEVE_DECLARE_CONVERTER_NS(lceve, LCTrackConverter)
