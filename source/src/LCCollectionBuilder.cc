
#include <LCEve/LCCollectionBuilder.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/Geometry.h>

#include <TParticle.h>
#include <ROOT/REveScene.hxx>

#include <EVENT/LCIO.h>

namespace lceve {

  LCCollectionConverter::LCCollectionConverter( EventDisplay *lced ) :
    _eventDisplay(lced) {
    /* nop */
  }


  void LCCollectionConverter::VisualizeEvent( const EVENT::LCEvent *event, REX::REveScene *scene ) {
    auto colnames = event->getCollectionNames() ;
    std::cout << "Loading event in Eve: event " << event->getEventNumber()
              << ", run " << event->getRunNumber()
              << ", " << colnames->size() << " collections" << std::endl ;
    for( auto &colname : *colnames ) {
      auto collection = event->getCollection( colname ) ;
      /// Load track collection
      if( collection->getTypeName() == EVENT::LCIO::TRACK ) {
        scene->AddElement( CreateEveTracks( collection, colname ) ) ;
      }
    }
  }


  REX::REveElement *LCCollectionConverter::CreateEveTracks( const EVENT::LCCollection *const collection,
                                                            const std::string &name ) {
    if( collection->getTypeName() != EVENT::LCIO::TRACK ) {
      std::stringstream ss ;
      ss << "CreateEveTracks: wrong collection type. Got " << collection->getTypeName() ;
      throw std::runtime_error( ss.str().c_str() ) ;
    }
    unsigned int nelts = collection->getNumberOfElements() ;
    REX::REveTrackList *eveTrackList = new REX::REveTrackList() ;
    eveTrackList->SetMainColor(kTeal);
    eveTrackList->SetNameTitle( name, name ) ;
    std::cout << "Loading LCIO Track collection " << name << ", nelts: " << nelts << std::endl ;

    for( unsigned int e=0 ; e<nelts ; e++ ) {
      const EVENT::Track *const lcTrack = static_cast<const EVENT::Track *>( collection->getElementAt(e) ) ;
      const auto *startTrackState = lcTrack->getTrackState( EVENT::TrackState::AtFirstHit ) ;
      const auto *lastTrackState = lcTrack->getTrackState( EVENT::TrackState::AtLastHit ) ;
      REX::REveVector startPoint( startTrackState->getReferencePoint() ) ;
      REX::REveVector endPoint( startTrackState->getReferencePoint() ) ;

      auto prop = _eventDisplay->GetGeometry()->CreateTrackPropagator() ;
      prop->SetMaxOrbs(5) ;
      prop->SetMaxR( endPoint.R() ) ;
      prop->SetMaxZ( endPoint[2] ) ;

      auto eveTrack = new REX::REveTrack() ;
      eveTrack->SetName( Form("Track %d", e) ) ;
      eveTrack->SetPropagator( prop ) ;
      eveTrack->MakeTrack() ;
      eveTrackList->AddElement( eveTrack ) ;
      // TODO fill track parameters correctly !
    }
    return eveTrackList ;
  }


}
