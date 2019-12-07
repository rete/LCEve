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
    /// An additional factor to the vertex extents
    static constexpr float VertexExtentFactor = 500.f ;
    
  public:
    EveElementFactory() = default ;
    ~EveElementFactory() = default ;

    /** @defgroup Object factories
     *  Factory methods for single object creation
     *  @{
     */
    /// Create a track out track parameters
    EveTrack *CreateTrack( ROOT::REveTrackPropagator *propagator, const TrackParameters &parameters ) const ;

    /// Create a vertex out of vertex parameters
    EveVertex *CreateVertex( const VertexParameters &parameters ) const ;
    
    /// Create a cluster out of cluster parameters
    EveCluster *CreateCluster( const ClusterParameters &parameters ) const ;

    /// Create a reco particle out of reco particle parameters
    EveRecoParticle *CreateRecoParticle( const RecoParticleParameters &parameters ) const ;

    /// Create a jet cone out of jet parameters
    EveJet *CreateJet( const JetParameters &parameters ) const ;
    
    /// Create a eve MC particle out of MC particle parameters
    EveMCParticle *CreateMCParticle( const MCParticleParameters &parameters ) const ;
    /** @} */

    /** @defgroup Container factories
     *  Factory methods for containers creation
     *  @{
     */
    /// Create track container
    std::unique_ptr<TrackContainer> CreateTrackContainer() const ;
    
    /// Create vertex container
    std::unique_ptr<VertexContainer> CreateVertexContainer() const ;
    
    /// Create cluster container
    std::unique_ptr<ClusterContainer> CreateClusterContainer() const ;
    
    /// Create reco particle container
    std::unique_ptr<RecoParticleContainer> CreateRecoParticleContainer() const ;
    
    /// Create jet container
    std::unique_ptr<JetContainer> CreateJetContainer() const ;
    
    /// Create jet container
    std::unique_ptr<MCParticleContainer> CreateMCParticleContainer() const ;
    
    /// Create calorimeter hit container
    std::unique_ptr<CaloHitContainer> CreateCaloHitContainer() const ;
    
    /// Create tracker hit container
    std::unique_ptr<TrackerHitContainer> CreateTrackerHitContainer() const ;
    /** @} */
    
    /** @defgroup Helper functions
     *  Helper methods acting on Eve element objects
     *  @{
     */
    /// Populate the calo hit container with hits from parameters
    void PopulateCaloHits( CaloHitContainer *container, const std::vector<CaloHitParameters> &caloHits ) const ;
    
    /** @} */
    
  private:
    /// Convert the properties as string. Formatted to be displayed
    /// in an object tooltip
    std::string PropertiesAsString( const PropertyMap &properties ) const ;
  };

}
