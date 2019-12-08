// -- lceve headers
#include <LCEve/LCObjectFactory.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/Geometry.h>
#include <LCEve/BField.h>
#include <LCEve/HelixClass.h>

namespace lceve {
  
  LCObjectFactory::LCObjectFactory( EventDisplay *lced ) :
    fEventDisplay(lced) {
    /* nop */
  }
  
  //--------------------------------------------------------------------------
  
  TrackState LCObjectFactory::ConvertTrackState( const EVENT::TrackState *const trackState ) const {
    auto bFieldPtr = fEventDisplay->GetGeometry()->GetBField() ;
    auto p = trackState->getReferencePoint() ;
    auto bfield = -bFieldPtr->GetFieldD( p[0]*0.1, p[1]*0.1, p[2]*0.1 )[2] ;
    HelixClass helix ;
    helix.Initialize_Canonical( 
      trackState->getPhi(), trackState->getD0(), trackState->getZ0(), 
      trackState->getOmega(), trackState->getTanLambda(), bfield ) ;
    // Fill parameters
    TrackState parameters {} ;
    parameters.fType = static_cast<TrackStateType>(trackState->getLocation()) ;
    parameters.fReferencePoint = ROOT::REveVectorT<float>( p[0]*0.1, p[1]*0.1, p[2]*0.1 ) ;
    parameters.fMomentum = ROOT::REveVectorT<float>( helix.getMomentum() ) ;
    return parameters ;
  }  
  
  //--------------------------------------------------------------------------
  
  TrackParameters LCObjectFactory::ConvertTrack( const EVENT::Track *const track ) const {
    TrackParameters parameters {} ;
    auto color = RandomColor( track ) ;
    // line attributes
    LineAttributes lineAttr ;
    lineAttr.fColor = color ;
    lineAttr.fWidth = 2 ;
    lineAttr.fStyle = 1 ;
    parameters.fLineAttributes = lineAttr ;
    marker attributes
    MarkerAttributes markerAttr ;
    markerAttr.fColor = color ;
    markerAttr.fSize = 3 ;
    parameters.fMarkerAttributes = markerAttr ;
    
    parameters.fCharge = track->getOmega() / std::fabs(track->getOmega()) ;
    auto tsTypes = { 
      EVENT::TrackState::AtIP,
      EVENT::TrackState::AtFirstHit,
      EVENT::TrackState::AtLastHit,  
      EVENT::TrackState::AtCalorimeter
    } ;
    for( auto tst : tsTypes ) {
      auto trackState = track->getTrackState(tst) ;
      if( nullptr != trackState ) {
        auto trackStateParameters = ConvertTrackState( trackState ) ;
        parameters.fTrackStates.push_back( trackStateParameters ) ;
      }
      // TODO add track state parameters to track properties
    }
    auto ref = track->getTrackState( EVENT::TrackState::AtFirstHit ) ;
    auto p = ref->getReferencePoint() ;
    parameters.fReferencePoint = ROOT::REveVectorT<float>( p[0]*0.1, p[1]*0.1, p[2]*0.1 ) ;
    auto bFieldPtr = fEventDisplay->GetGeometry()->GetBField() ;
    auto bfield = -bFieldPtr->GetFieldD( 0, 0, 0 )[2] ;
    HelixClass helix ;
    helix.Initialize_Canonical( 
      ref->getPhi(), ref->getD0(), ref->getZ0(), 
      ref->getOmega(), ref->getTanLambda(), bfield ) ;
    parameters.fMomentum = ROOT::REveVectorT<float>( helix.getMomentum() ) ;
    return parameters ;
  }
  
  //--------------------------------------------------------------------------
  
  VertexParameters LCObjectFactory::ConvertVertex( const EVENT::Vertex *const vertex ) const {
    VertexParameters parameters {} ;
    // Vertex level: keep it simple for now
    parameters.fLevel = vertex->isPrimary() ? 1 : 2 ;
    // Vertex errors: if not 6, it will throw an error later on 
    // while constructing the REveEllipsoid
    if( vertex->getCovMatrix().size() == 6 ) {
      std::array<float, 6> errors {} ;
      auto &matrix = vertex->getCovMatrix() ;
      for( unsigned int i=0; i<matrix.size() ; i++ ) {
        errors[i] = matrix[i]*0.1 ;
      }
      parameters.fErrors = errors ;
    }
    auto p = vertex->getPosition() ;
    parameters.fPosition = ROOT::REveVectorT<float>( p[0]*0.1, p[1]*0.1, p[2]*0.1 ) ;
    LineAttributes attr ;
    attr.fColor = RandomColor( vertex ) ;
    parameters.fLineAttributes = attr ;
    PropertyMap properties ;
    properties["Algorithm"] = vertex->getAlgorithmType() ;
    properties["Chi2"] = vertex->getChi2() ;
    properties["Probability"] = vertex->getProbability() ;
    properties["Parameters"] = vertex->getParameters() ;
    parameters.fProperties = properties ;
    return parameters ;
  }
  
}