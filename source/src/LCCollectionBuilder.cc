
#include <LCEve/LCCollectionBuilder.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/Geometry.h>

#include <TParticle.h>

namespace lceve {

  // LCCollectionConverter::LCCollectionConverter( EventDisplay *lced ) :
  //   _eventDisplay(lced) {
  //   /* nop */
  // }
  //
  // REX::REveElement *LCCollectionConverter::CreateEveTracks( const EVENT::LCCollection *const collection,
  //                                                           const std::string &name,
  //                                                           REX::REveElement *parent ) {
  //   if( collection->getTypeName() != EVENT::LCIO::TRACK ) {
  //     std::stringstream ss ;
  //     ss << "CreateEveTracks: wrong collection type. Got " << collection->getTypeName() ;
  //     throw std::runtime_error( ss.str().c_str() ) ;
  //   }
  //   auto &detector = _eventDisplay->GetGeometry().GetDetector() ;
  //   const auto *recdata = _eventDisplay->GetGeometry()->GetLayeredCaloData(
  //     ( dd4hep::DetType::CALORIMETER | dd4hep::DetType::ELECTROMAGNETIC | dd4hep::DetType::BARREL),
  //     ( dd4hep::DetType::AUXILIARY | dd4hep::DetType::FORWARD ) ) ;
  //   const float maxR = recdata.extent[1] ;
  //   recdata = _eventDisplay->GetGeometry()->GetLayeredCaloData(
  //     ( dd4hep::DetType::CALORIMETER | dd4hep::DetType::ELECTROMAGNETIC | dd4hep::DetType::ENDCAP),
  //     ( dd4hep::DetType::AUXILIARY | dd4hep::DetType::FORWARD ) ) ;
  //   const float maxZ = recdata.extent[3] ;
  //
  //   unsigned int nelts = collection->getNumberOfElements() ;
  //   TEveTrackList *eveTrackList = new TEveTrackList() ;
  //   eveTrackList->SetMainColor(kTeal);
  //
  //   for( unsigned int e=0 ; e<nelts ; e++ ) {
  //     const EVENT::Track *const lcTrack = static_cast<const EVENT::Track *>( collection->getElementAt(e) ) ;
  //
  //     auto eveTrack = new REX::REveTrack() ;
  //     auto prop = _eventDisplay->GetGeometry()->CreateTrackPropagator() ;
  //     prop->SetMaxOrbs(5) ;
  //     prop->SetMaxR( maxR ) ;
  //     prop->SetMaxZ( maxZ ) ;
  //     eveTrack->SetPropagator( prop ) ;
  //
  //     const auto *lcTrackState = lcTrack->getTrackState( EVENT::TrackState::AtFirstHit ) ;
  //     REX::REveVector startPoint( lcTrackState->getReferencePoint() ) ;
  //     prop->
  //     REX::REveRecTrack recTrack ;
  //
  //
  //   const double pt(m_settings.m_bField * 2.99792e-4 / std::fabs(pTrackState->getOmega()));
  //
  //   const double px(pt * std::cos(pTrackState->getPhi()));
  //   const double py(pt * std::sin(pTrackState->getPhi()));
  //   const double pz(pt * pTrackState->getTanLambda());
  //
  //     recTrack.fV.Set( lcTrackState.getReferencePoint() ) ;
  //     recTrack.fV.Set( lcTrackState.getReferencePoint() ) ;
  //     // eveTrack->SetPdg( track->getType() ) ;
  //   }
  //
  //
  //
  //
  //
  //
  //   return eveTrack ;
  // }
  //

}
