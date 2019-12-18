#pragma once

// -- std headers
#include <type_traits>
#include <vector>

// -- lcio headers
#include <EVENT/LCCollection.h>
#include <EVENT/LCObject.h>
#include <EVENT/LCIO.h>

// -- lceve headers
#include <LCEve/HelixClass.h>

// -- root headers
#include <ROOT/REveTrackPropagator.hxx>

namespace lceve {
  
  /// LCIOHelper class
  /// Helper class dealing with LCIO objects
  class LCIOHelper {
  public:
    /// std::sort lambda functions
    template <typename T>
    using SortFunction_t = std::function<bool( const T *, const T * )> ;
    template <typename T>
    using SortFunctionMap_t = std::map<std::string, SortFunction_t<T>> ;
    
    /// Convert the LCIO collection to a vector. More convienient for sorting and looping
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
    
    template <typename T>
    static float GetEnergy( const T *hit ) {
      return hit->getEnergy() ;
    }
  };
  
  template <>
  static float LCIOHelper::GetEnergy( const EVENT::RawCalorimeterHit *hit ) {
    return hit->getAmplitude() ;
  }
  
}