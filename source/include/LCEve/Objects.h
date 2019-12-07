#pragma once

// -- lceve headers
#include <LCEve/ROOTTypes.h>
#include <LCEve/json.h>

// -- root headers
#include <ROOT/REveVector.hxx>

// -- std headers
#include <optional>
#include <string>
#include <array>
#include <map>
#include <sstream>

namespace lceve {

  /// Additional properties for objects
  /// Usage is implementation defined but usually
  /// the key-values go in the object tooltip
  using PropertyMap = nlohmann::json ;
  
  /// Eve element containers for each object type
  using TrackContainer = ROOT::REveElement ;
  using VertexContainer = ROOT::REveElement ;
  using ClusterContainer = ROOT::REveElement ;
  using RecoParticleContainer = ROOT::REveElement ;
  using JetContainer = ROOT::REveElement ;
  using MCParticleContainer = ROOT::REveElement ;
  using CaloHitContainer = ROOT::REvePointSet ;
  using TrackerHitContainer = ROOT::REvePointSet ;

  /// Eve element containers for each object type
  using EveTrack = ROOT::REveTrack ;
  using EveVertex = ROOT::REveEllipsoid ;
  using EveCluster = CaloHitContainer ;
  using EveRecoParticle = ROOT::REveElement ;
  using EveJet = ROOT::REveJetCone ;
  using EveMCParticle = ROOT::REveTrack ;
  // NOTE: Calo hit and tracker hit have no Eve equivalents
  // They are stored internally in their parent container
  

  /// MarkerAttributes struct
  /// Describe the properties of a marker (color, size, style)
  struct MarkerAttributes {
    /// The marker color
    std::optional<Int_t>                 fColor {} ;
    /// The marker size
    std::optional<Size_t>                fSize {} ;
    /// The marker style
    std::optional<Int_t>                 fStyle {} ;
  };

  /// LineAttributes struct
  /// Describe the properties of a line (color, width, style)
  struct LineAttributes {
    /// The line color
    std::optional<Int_t>                 fColor {} ;
    /// The line width
    std::optional<Size_t>                fWidth {} ;
    /// The line style
    std::optional<Int_t>                 fStyle {} ;
  };

  /// FillAttributes struct
  /// Describe the properties for filling areas (color, style)
  struct FillAttributes {
    /// The fill color
    std::optional<Int_t>                 fColor {} ;
    /// The fill style
    std::optional<Int_t>                 fStyle {} ;
  };


  /// TrackStateType enum
  enum class TrackStateType {
    /// Track state at IP point
    AtIP = 1,
    /// Track state at track start (first hit)
    AtStart = 2,
    /// Track state at track end (last hit)
    AtEnd = 3,
    /// Track state at calorimeter entry point
    AtCalorimeter = 4
  };


  /// TrackInfo struct
  struct TrackInfo {
    /// The track reference point
    std::optional<ROOT::REveVectorT<float>>            fReferencePoint {} ;
    /// The track momentum
    std::optional<ROOT::REveVectorT<float>>            fMomentum {} ;
  };


  /// TrackState struct
  struct TrackState : public TrackInfo {
    /// The track state type
    std::optional<TrackStateType>                      fType {} ;
  };


  /// TrackParameters struct
  /// Necessary data to create a track in Eve
  struct TrackParameters : public TrackInfo {
    /// The track line attributes
    std::optional<LineAttributes>                      fLineAttributes {} ;
    /// The track marker attributes
    std::optional<MarkerAttributes>                    fMarkerAttributes {} ;
    /// The track charge
    std::optional<int>                                 fCharge {} ;
    /// An optional list of track markers (AKA track state)
    std::vector<TrackState>                            fTrackStates {} ;
    /// Whether the track is pickable on the display (default is true)
    std::optional<bool>                                fPickable {true} ;
    /// Optional user data (framework track ?)
    std::optional<void*>                               fUserData {} ;
    /// Additional track properties
    PropertyMap                                        fProperties {} ;
  };


  /// VertexParameters struct
  /// Necessary data to describe a reconstructed vertex
  struct VertexParameters {
    /// The vertex level (1 == primary, 2 secondary, ...)
    std::optional<unsigned int>                        fLevel {} ;
    /// The vertex fit error: the lower triangle of the covariance matrix of the position
    std::optional<std::array<float,6>>                 fErrors {} ;
    /// The reconstructed vertex position
    std::optional<ROOT::REveVectorT<float>>            fPosition {} ;
    /// The vertex line attributes
    std::optional<LineAttributes>                      fLineAttributes {} ;
    /// Additional vertex properties
    PropertyMap                                        fProperties {} ;
  };

  /// CaloHitParameters struct
  /// Necessary data to describe a calo hit
  struct CaloHitParameters {
    /// The calorimeter hit position
    std::optional<ROOT::REveVectorT<float>>            fPosition {} ;
    /// The calo hit color.
    /// If not set, the amplitude will be used to determine the color
    /// according to the internal ROOT Eve palette
    std::optional<Color_t>                             fColor {} ;
    /// The calo hit amplitude
    std::optional<float>                               fAmplitude {} ;
    /// The coordinates of the 8 corners of the calo hit box (8 x 3)
    std::optional<std::array<float,24>>                fBoxCorners {} ;
    /// The calo hit transparency
    std::optional<Char_t>                              fTransparency {0} ;
  };


  /// ClusterParameters struct
  /// Necessary data to describe and build a cluster in Eve
  struct ClusterParameters {
    /// The reconstructed cluster energy (unit GeV)
    std::optional<float>                               fEnergy {} ;
    /// The cluster marker attributes
    std::optional<MarkerAttributes>                    fMarkerAttributes {} ;
    /// The list of calorimeter hits
    std::optional<std::vector<CaloHitParameters>>      fCaloHits {} ;
    /// The cluster color.
    /// If set, all calo hits will have the same color
    /// Else, the individual calo hit amplitudes are used
    std::optional<Color_t>                             fColor {} ;
    /// Additional cluster properties
    PropertyMap                                        fProperties {} ;
  };


  /// RecoParticleParameters struct
  /// Necessary data to describe and build a reco particle in Eve
  struct RecoParticleParameters {
    /// The reco particle energy
    std::optional<float>                               fEnergy {} ;
    /// The reco particle momentum
    std::optional<ROOT::REveVectorT<float>>            fMomentum {} ;
    /// The reco particle mass. If not given, computed from energy and momentum
    std::optional<float>                               fMass {} ;
    /// The list of tracks
    std::optional<std::vector<TrackParameters>>        fTracks {} ;
    /// The list of cluster
    std::optional<std::vector<ClusterParameters>>      fClusters {} ;
    /// Additional particle properties
    PropertyMap                                        fProperties {} ;
  };


  /// JetParameters struct
  /// Necessary data to describe and build a jet cone in Eve
  struct JetParameters {
    /// The 3 jet cone parameters: eta, phi, R
    std::optional<std::array<float,3>>                 fConeParameters {} ;
    /// The jet energy
    std::optional<float>                               fEnergy {} ;
    /// The jet momentum
    std::optional<ROOT::REveVectorT<float>>            fMomentum {} ;
    /// The jet mass. If not given, computed from energy and momentum
    std::optional<float>                               fMass {} ;
    /// An optional list of reco particle to build
    std::optional<std::vector<RecoParticleParameters>> fParticles {} ;
  };
  
  /// MCParticleParameters struct 
  /// Necessary data to describe and build a MC particle in Eve
  /// MC particles are drawn as tracks. If the charge is not zero,
  /// the track propagator is expected to be set
  struct MCParticleParameters {
    /// The MC particle energy
    std::optional<float>                               fEnergy {} ;
    /// The MC particle charge
    std::optional<int>                                 fCharge {} ;
    /// The MC particle mass
    std::optional<float>                               fMass {} ;
    /// The MC particle PDG code
    std::optional<int>                                 fPDG {} ;
    /// The MC particle line attributes
    std::optional<LineAttributes>                      fLineAttributes {} ;
    /// The MC particle marker attributes
    std::optional<MarkerAttributes>                    fMarkerAttributes {} ;
    /// The MC particle vertex position
    std::optional<ROOT::REveVectorT<float>>            fVertexPosition {} ;
    /// The MC particle endpoint position
    std::optional<ROOT::REveVectorT<float>>            fEndpointPosition {} ;
    /// The MC particle momentum
    std::optional<ROOT::REveVectorT<float>>            fVertexMomentum {} ;
    /// The MC particle endpoint momentum
    std::optional<ROOT::REveVectorT<float>>            fEndpointMomentum {} ;
    /// Additional MC particle properties
    PropertyMap                                        fProperties {} ;
  };
  
  /// TrackerHitParameters struct
  /// Necessary data to describe and build a tracker hit in Eve
  struct TrackerHitParameters {
    /// The tracker hit position
    std::optional<ROOT::REveVectorT<float>>            fPosition {} ;
    /// The tracker hit marker attributes
    std::optional<MarkerAttributes>                    fMarkerAttributes {} ;
  };
  
  template <typename T>
  inline static std::string FormatReal( const T &val ) {
    std::stringstream sstr ;
    sstr << std::scientific << (val >= 0. ? "+" : "") << val ;
    return sstr.str() ;
  }

  class ObjectHelper {
  public:
    /// Compute the track momentum from track properties
    static ROOT::REveVectorT<float> ComputeMomentum( float bfield, float omega, float phi, float tanLambda ) ;
  };

}
