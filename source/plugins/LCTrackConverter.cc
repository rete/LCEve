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

namespace lceve {
  
  class LCTrackConverter : public ICollectionConverter {
  public:
    /// Default constructor
    LCTrackConverter() = default ;
    
    ///  Create tracks out of EVENT::Track objects
    ROOT::REveElement* ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) override ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  ROOT::REveElement* LCTrackConverter::ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) {
    if( collection->getTypeName() != EVENT::LCIO::TRACK ) {
      std::cout << "ERROR: Expected collection type EVENT::LCIO::TRACK, got " << collection->getTypeName() << std::endl ;
      return nullptr ;
    }
    std::cout << "Converting track collection " << name << std::endl ;
    auto tracks = LCIOHelper::CollectionAsVector<EVENT::Track>( collection ) ;
    auto color = GetParameter<std::string>( "Color" ).value_or( "iter" ) ;
    auto colorFunctor = ColorHelper::GetColorFunction( color ) ;
    
    std::sort( tracks.begin(), tracks.end(), [this](auto lhs, auto rhs) {
      auto lhsMomentum = LCIOHelper::ComputeTrackMomentum(
        this->GetEventDisplay()->GetGeometry()->GetBField(), lhs->getTrackState( EVENT::TrackState::AtFirstHit ) ) ;
      auto rhsMomentum = LCIOHelper::ComputeTrackMomentum( 
        this->GetEventDisplay()->GetGeometry()->GetBField(), rhs->getTrackState( EVENT::TrackState::AtFirstHit ) ) ;
      return ( rhsMomentum.Mag() < lhsMomentum.Mag() ) ;
    } ) ;
    
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
