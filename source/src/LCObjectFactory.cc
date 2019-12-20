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
    auto color = ColorHelper::RandomColor( track ) ;
    // line attributes
    LineAttributes lineAttr ;
    lineAttr.fColor = color ;
    lineAttr.fWidth = 2 ;
    lineAttr.fStyle = 1 ;
    parameters.fLineAttributes = lineAttr ;
    // marker attributes
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
    attr.fColor = ColorHelper::RandomColor( vertex ) ;
    parameters.fLineAttributes = attr ;
    PropertyMap properties ;
    properties["Algorithm"] = vertex->getAlgorithmType() ;
    properties["Chi2"] = vertex->getChi2() ;
    properties["Probability"] = vertex->getProbability() ;
    properties["Parameters"] = vertex->getParameters() ;
    parameters.fProperties = properties ;
    return parameters ;
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  std::vector<CaloHitParameters> LCObjectFactory::ConvertCaloHits( const std::vector<T*> &caloHits ) const {
    if( caloHits.empty() ) {
      return {} ;
    }
    std::vector<CaloHitParameters> parametersList {} ;
    parametersList.reserve( caloHits.size() ) ;
    auto color = ColorHelper::RandomColor( *caloHits.begin() ) ;
    for( auto &caloHit : caloHits ) {
      CaloHitParameters parameters {} ;
      auto pos = caloHit->getPosition() ;
      parameters.fPosition = ROOT::REveVectorT<float>( pos[0]*0.1, pos[1]*0.1, pos[2]*0.1 ) ;
      parameters.fColor = color ;
      parameters.fAmplitude = caloHit->getEnergy() ;
      parametersList.push_back( parameters ) ;
    }
    return parametersList ;
  }
  
  //--------------------------------------------------------------------------
  
  ClusterParameters LCObjectFactory::ConvertCluster( const EVENT::Cluster *const cluster ) const {
    ClusterParameters parameters {} ;
    auto color = ColorHelper::RandomColor( cluster ) ;
    MarkerAttributes attr {} ;
    attr.fColor = color ; attr.fSize = 3 ; attr.fStyle = 4 ;
    parameters.fMarkerAttributes = attr ;
    parameters.fEnergy = cluster->getEnergy() ;
    parameters.fCaloHits = this->ConvertCaloHits( cluster->getCalorimeterHits() ) ;
    return parameters ;
  }
  
  //--------------------------------------------------------------------------
  
  RecoParticleParameters LCObjectFactory::ConvertRecoParticle( const EVENT::ReconstructedParticle *const recoParticle ) const {
    RecoParticleParameters parameters {} ;
    parameters.fEnergy = recoParticle->getEnergy() ;
    parameters.fMomentum = ROOT::REveVectorT<float>( recoParticle->getMomentum() ) ;
    parameters.fMass = recoParticle->getMass() ;
    parameters.fColor = ColorHelper::RandomColor( recoParticle ) ;
    auto &tracks = recoParticle->getTracks() ;
    if( not tracks.empty() ) {
      std::vector<TrackParameters> trackParams {} ;
      trackParams.reserve( tracks.size() ) ;
      for( auto &trk : tracks ) {
        trackParams.push_back( this->ConvertTrack( trk ) ) ;
      }
      parameters.fTracks = trackParams ;
    }
    auto &clusters = recoParticle->getClusters() ;
    if( not clusters.empty() ) {
      std::vector<ClusterParameters> clusterParams {} ;
      clusterParams.reserve( clusters.size() ) ;
      for( auto &cl : clusters ) {
        clusterParams.push_back( this->ConvertCluster( cl ) ) ;
      }
      parameters.fClusters = clusterParams ;
    }
    return parameters ;
  }
  
  //--------------------------------------------------------------------------
  
  MCParticleParameters LCObjectFactory::ConvertMCParticle( const EVENT::MCParticle *const mcp ) const {
    
    auto color = ColorHelper::RandomColor( mcp ) ;
    LineAttributes lineAttr {} ;
    lineAttr.fColor = color ;
    lineAttr.fStyle = kDashed ;
    lineAttr.fWidth = 2 ;
    MarkerAttributes markerAttr {} ;
    markerAttr.fColor = color ;
    
    MCParticleParameters parameters {} ;
    parameters.fEnergy = mcp->getEnergy() ;
    parameters.fCharge = mcp->getCharge() ;
    parameters.fMass = mcp->getMass() ;
    parameters.fPDG = mcp->getPDG() ;
    parameters.fLineAttributes = lineAttr ;
    parameters.fMarkerAttributes = markerAttr ;
    auto v = mcp->getVertex() ;
    auto ep = mcp->getEndpoint() ;
    parameters.fVertexPosition = ROOT::REveVectorT<float>( v[0]*0.1, v[1]*0.1, v[2]*0.1 ) ;
    parameters.fVertexMomentum = ROOT::REveVectorT<float>( mcp->getMomentum() ) ;
    parameters.fEndpointPosition = ROOT::REveVectorT<float>( ep[0]*0.1, ep[1]*0.1, ep[2]*0.1 ) ;
    parameters.fEndpointMomentum = ROOT::REveVectorT<float>( mcp->getMomentumAtEndpoint() ) ;
    
    return parameters ;
  }
  
  //--------------------------------------------------------------------------
  
  template std::vector<CaloHitParameters> LCObjectFactory::ConvertCaloHits( const std::vector<EVENT::CalorimeterHit*> &caloHits ) const ;
  template std::vector<CaloHitParameters> LCObjectFactory::ConvertCaloHits( const std::vector<EVENT::SimCalorimeterHit*> &caloHits ) const ;
}