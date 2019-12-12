#pragma once

// -- lcio headers
#include <EVENT/LCEvent.h>
#include <EVENT/Track.h>
#include <EVENT/CalorimeterHit.h>
#include <EVENT/Cluster.h>
#include <EVENT/ReconstructedParticle.h>
#include <EVENT/ParticleID.h>
#include <EVENT/LCCollection.h>

// -- lceve headers
#include <LCEve/DrawAttributes.h>
#include <LCEve/Geometry.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/HelixClass.h>

// -- root headers
#include <ROOT/REveElement.hxx>
#include <ROOT/REveTrack.hxx>
#include <ROOT/REveScene.hxx>
namespace REX = ROOT::Experimental ;

#include <DD4hep/Objects.h>

namespace lceve {

  class EventDisplay ;

  class LCCollectionConverter {
  public:
    LCCollectionConverter() = delete ;
    ~LCCollectionConverter() = default ;
    LCCollectionConverter(const LCCollectionConverter &) = delete ;
    LCCollectionConverter &operator =(const LCCollectionConverter &) = delete ;

    /// Constructor
    LCCollectionConverter( EventDisplay *lced ) ;

    /// Load the LCIO event in the Eve scene
    void VisualizeEvent( const EVENT::LCEvent *event, REX::REveScene *scene ) ;

    // /// Visualize LCIO tracks. Can be extract from a collection or can be
    // /// a sub-list of any object, e.g ReconstructedParticle
    // void VisualizeTracks( const std::vector<EVENT::Track*> &tracks,
    //   const std::string &name,
    //   REX::REveElement *parent,
    //   ColorIterator &colorIter ) ;
    // 
    // /// Visualize LCIO calorimeter hits. Can be extract from a collection or can be
    // /// a sub-list of any object, e.g Cluster
    // void VisualizeCaloHits( const std::vector<EVENT::CalorimeterHit*> &caloHits,
    //   const std::string &name,
    //   REX::REveElement *parent,
    //   ColorIterator &colorIter ) ;
    // 
    // /// Visualize LCIO clusters. Can be extract from a collection or can be
    // /// a sub-list of any object, e.g ReconstructedParticle
    // void VisualizeClusters( const std::vector<EVENT::Cluster*> &clusters,
    //   const std::string &name,
    //   REX::REveElement *parent,
    //   ColorIterator &colorIter ) ;
    // 
    // /// Visualize LCIO reco particles.
    // void VisualizeRecoParticles( const std::vector<EVENT::ReconstructedParticle*> &particles,
    //   const std::string &name,
    //   REX::REveElement *parent,
    //   ColorIterator &colorIter ) ;
    // 
    // /// Visualize LCIO reco particles.
    // void VisualizeVertices( const std::vector<EVENT::Vertex*> &vertices,
    //   const std::string &name,
    //   REX::REveElement *parent,
    //   ColorIterator &colorIter ) ;

  private:
    /// Convert a LCIO collection to a vector of daughter type
    template <class T>
    static std::vector<T*> asVector( const EVENT::LCCollection *const collection ) ;
    
    /// Compute the momentum of the track or trackstate object
    template <class TRK>
    REX::REveVector ComputeMomentum( const TRK *const trk ) const ;
    
    /// Generate a description string out of a track state object
    std::string GetTrackStateDescription( const EVENT::TrackState *const trkState ) const ;
    
    /// Generate the track state location as string
    std::string GetTrackStateLocationAsString( const EVENT::TrackState *const trkState ) const ;
    
    /// Generate a description string out of particle id objects
    std::string GetParticleIDsDescription( const std::vector<EVENT::ParticleID*> &pids ) const ;

  private:
    EventDisplay             *_eventDisplay {nullptr} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <class T>
  inline std::vector<T*> LCCollectionConverter::asVector( const EVENT::LCCollection *const collection ) {
    std::vector<T*> vec ;
    auto nelts = collection->getNumberOfElements() ;
    vec.reserve( nelts ) ;
    for( int e=0 ; e<nelts ; ++e ) {
      auto *elt = static_cast<T*>( collection->getElementAt(e) ) ;
      vec.push_back( elt ) ;
    }
    return vec ;
  }
  
  //--------------------------------------------------------------------------
  
  template <class TRK>
  inline REX::REveVector LCCollectionConverter::ComputeMomentum( const TRK *const trk ) const {
    // NOTE: strange convention with a minus sign.
    // See also BField class returning -b from dd4hep, not b  
    auto magField = _eventDisplay->GetGeometry()->GetBField()->GetFieldD( 0, 0, 0  ) ;
    const double bfield = -magField[2] ;
    HelixClass helix ;
    helix.Initialize_Canonical( trk->getPhi(), trk->getD0(), trk->getZ0(), trk->getOmega(), trk->getTanLambda(), bfield ) ;
    return REX::REveVector( helix.getMomentum() ) ;    
  }

}
