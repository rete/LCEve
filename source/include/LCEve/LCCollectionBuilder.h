#pragma once

// -- lcio headers
#include <EVENT/LCEvent.h>
#include <EVENT/Track.h>
#include <EVENT/CalorimeterHit.h>
#include <EVENT/Cluster.h>
#include <EVENT/ReconstructedParticle.h>
#include <EVENT/LCCollection.h>

// -- lceve headers
#include <LCEve/DrawAttributes.h>

// -- root headers
#include <ROOT/REveElement.hxx>
#include <ROOT/REveTrack.hxx>
#include <ROOT/REveScene.hxx>
namespace REX = ROOT::Experimental ;

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

    /// Visualize LCIO tracks. Can be extract from a collection or can be
    /// a sub-list of any object, e.g ReconstructedParticle
    void VisualizeTracks( const std::vector<EVENT::Track*> &tracks,
      const std::string &name,
      REX::REveElement *parent,
      ColorIterator &colorIter ) ;

    /// Visualize LCIO calorimeter hits. Can be extract from a collection or can be
    /// a sub-list of any object, e.g Cluster
    void VisualizeCaloHits( const std::vector<EVENT::CalorimeterHit*> &caloHits,
      const std::string &name,
      REX::REveElement *parent,
      ColorIterator &colorIter ) ;

    /// Visualize LCIO clusters. Can be extract from a collection or can be
    /// a sub-list of any object, e.g ReconstructedParticle
    void VisualizeClusters( const std::vector<EVENT::Cluster*> &clusters,
      const std::string &name,
      REX::REveElement *parent,
      ColorIterator &colorIter ) ;

    /// Visualize LCIO reco particles.
    void VisualizeRecoParticles( const std::vector<EVENT::ReconstructedParticle*> &particles,
      const std::string &name,
      REX::REveElement *parent,
      ColorIterator &colorIter ) ;

  private:
    /// Convert a LCIO collection to a vector of daughter type
    template <class T>
    static std::vector<T*> asVector( const EVENT::LCCollection *const collection ) ;

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

}
