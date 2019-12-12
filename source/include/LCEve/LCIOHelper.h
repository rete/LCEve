#pragma once

#include <type_traits>
#include <vector>

#include <EVENT/LCCollection.h>
#include <EVENT/LCObject.h>
#include <EVENT/LCIO.h>

#include <LCEve/HelixClass.h>

#include <ROOT/REveTrackPropagator.hxx>

namespace lceve {
  
  class LCIOHelper {
  public:
    template <typename T, typename = typename std::enable_if<std::is_base_of<EVENT::LCObject,T>::value>::type >
    static std::vector<T*> CollectionAsVector( const EVENT::LCCollection *const collection ) {
      std::vector<T*> vec ;
      vec.reserve( collection->getNumberOfElements() ) ;
      for( unsigned int e=0 ; e<collection->getNumberOfElements() ; ++e ) {
        vec.push_back( static_cast<T*>( collection->getElementAt(e) ) ) ;
      }
      return vec ;
    }
    
    /// Compute a track momentum out of a EVENT::Track or EVENT::TrackState
    template <typename TRK>
    static ROOT::REveVector ComputeTrackMomentum( const ROOT::REveMagField *field, const TRK *const trk ) {
      // NOTE: strange convention with a minus sign.
      // See also BField class returning -b from dd4hep, not b  
      auto magField = field->GetFieldD( 0, 0, 0  ) ;
      const double bfield = -magField[2] ;
      HelixClass helix ;
      helix.Initialize_Canonical( trk->getPhi(), trk->getD0(), trk->getZ0(), trk->getOmega(), trk->getTanLambda(), bfield ) ;
      return ROOT::REveVector( helix.getMomentum() ) ;  
    }
  };
  
}