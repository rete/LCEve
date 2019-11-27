// -- lceve headers
#include <LCEve/LCObjectFactory.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/Geometry.h>
#include <LCEve/BField.h>

namespace lceve {
  
  LCObjectFactory::LCObjectFactory( EventDisplay *lced ) :
    fEventDisplay(lced) {
    /* nop */
  }
  
  //--------------------------------------------------------------------------
  
  TrackState LCObjectFactory::ConvertTrackState( const EVENT::TrackState *const trackState ) const {
    auto bFieldPtr = fEventDisplay->GetGeometry()->GetBField() ;
    auto p = trackState->getReferencePoint() ;
    // Fill parameters
    TrackState parameters {} ;
    parameters.fType = static_cast<TrackStateType>(trackState->getLocation()) ;
    parameters.fReferencePoint = ROOT::REveVectorT<float>( trackState->getReferencePoint() ) ;
    parameters.fBField = bFieldPtr->GetField( p[0], p[1], p[2] ).Mag() ;
    parameters.fD0 = trackState->getD0() ;
    parameters.fPhi = trackState->getPhi() ;
    parameters.fOmega = trackState->getOmega() ;
    parameters.fZ0 = trackState->getZ0() ;
    parameters.fTanLambda = trackState->getTanLambda() ;
    return parameters ;
  }  
  
  //--------------------------------------------------------------------------
  
  TrackParameters LCObjectFactory::ConvertTrack( const EVENT::Track *const track ) const {
    TrackParameters parameters {} ;
#pragma message "Implement LCIO track conversion"
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
        errors[i] = matrix[i] ;
      }
      parameters.fErrors = errors ;
    }
    parameters.fPosition = ROOT::REveVectorT<float>( vertex->getPosition() ) ;
    LineAttributes attr ;
    attr.fColor = LCObjectFactory::GuessColor( vertex ) ;
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