#pragma once

// -- lceve headers
#include <LCEve/Objects.h>
#include <LCEve/DrawAttributes.h>

// -- lcio headers
#include <EVENT/Track.h>
#include <EVENT/ReconstructedParticle.h>
#include <EVENT/Cluster.h>
#include <EVENT/Vertex.h>
#include <EVENT/CalorimeterHit.h>
#include <EVENT/SimCalorimeterHit.h>
#include <EVENT/RawCalorimeterHit.h>
#include <EVENT/MCParticle.h>
#include <EVENT/SimTrackerHit.h>
#include <EVENT/TrackerHit.h>
#include <EVENT/TrackerHitPlane.h>
#include <EVENT/TrackerHitZCylinder.h>
#include <EVENT/TrackerPulse.h>
#include <EVENT/TPCHit.h>

namespace lceve {
  
  class EventDisplay ;

  /// LCObjectFactory class
  /// Helper class to convert LCIO object to Eve object parameters (see Objects.h)
  class LCObjectFactory {        
  public:
    /// Constructor
    LCObjectFactory( EventDisplay *lced ) ;
    
    /// Convert a LCIO track state object
    TrackState ConvertTrackState( const EVENT::TrackState *const trackState ) const ;
    
    /// Convert a LCIO track object
    TrackParameters ConvertTrack( const EVENT::Track *const track ) const ;
    
    /// Convert a LCIO vertex object
    VertexParameters ConvertVertex( const EVENT::Vertex *const vertex ) const ;
    
    // /// Convert a LCIO calo hit object
    // CaloHitParameters ConvertCaloHit( const EVENT::CalorimeterHit *const caloHit ) const ;
    // 
    // /// Convert a LCIO sim calo hit object
    // CaloHitParameters ConvertCaloHit( const EVENT::SimCalorimeterHit *const caloHit ) const ;
    // 
    // /// Convert a LCIO cluster object
    // ClusterParameters ConvertCluster( const EVENT::Cluster *const cluster ) const ;
    // 
    // /// Convert a LCIO reco particle object
    // RecoParticleParameters ConvertRecoParticle( const EVENT::ReconstructedParticle *const recoParticle ) const ;
    // 
    // /// Convert a LCIO jet (reco particle) object
    // JetParameters ConvertJet( const EVENT::ReconstructedParticle *const jet ) const ;
    
    // /// Convert a LCIO MC particle object
    // MCParticleParameters ConvertMCParticle( const EVENT::MCParticle *const mcp ) const ;
    
    // TODO Tracker Hit, etc ...
    
  private:     
    EventDisplay          *fEventDisplay {nullptr} ;
  };
    
}