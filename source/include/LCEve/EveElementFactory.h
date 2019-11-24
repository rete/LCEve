#pragma once

// -- lceve headers
#include <LCEve/ROOTTypes.h>
#include <LCEve/Objects.h>

namespace lceve {

  /// EveElementFactory class
  /// Helper class to convert objects (see LCEve/Objects.h)
  /// to Eve elements
  class EveElementFactory {
  public:
    EveElementFactory() = default ;
    ~EveElementFactory() = default ;

    /// Create a track out track parameters
    ROOT::REveTrack *CreateTrack( const TrackParameters &parameters ) const ;

    /// Create a vertex out of vertex parameters
    ROOT::REveEllipsoid *CreateVertex( const VertexParameters &parameters ) const ;

    /// Create a vertex out of vertex parameters
    /// TODO/FIXME, Use a REveBoxSet instead of a REvePointSet
    ROOT::REvePointSet *CreateCalorimeterHits( const CaloHitParameters &parameters ) const ;

    /// Create a cluster out of cluster parameters
    ROOT::REveElement *CreateCluster( const ClusterParameters &parameters ) const ;

    /// Create a reco particle out of reco particle parameters
    ROOT::REveElement *CreateRecoParticle( const RecoParticleParameters &parameters ) const ;

    /// Create a jet cone out of jet parameters
    ROOT::REveJetCone *CreateJet( const JetParameters &parameters ) const ;
  };

}
