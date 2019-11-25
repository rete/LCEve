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

namespace lceve {

  using PropertyMap = nlohmann::json ;

  /// MarkerAttributes struct
  /// Describe the properties of a marker (color, size, style)
  struct MarkerAttributes {
    /// The marker color
    std::optional<Int_t>                 fColor ;
    /// The marker size
    std::optional<Size_t>                fSize ;
    /// The marker style
    std::optional<Int_t>                 fStyle ;
  };

  /// LineAttributes struct
  /// Describe the properties of a line (color, width, style)
  struct LineAttributes {
    /// The line color
    std::optional<Int_t>                 fColor ;
    /// The line width
    std::optional<Size_t>                fWidth ;
    /// The line style
    std::optional<Int_t>                 fStyle ;
  };

  /// FillAttributes struct
  /// Describe the properties for filling areas (color, style)
  struct FillAttributes {
    /// The fill color
    std::optional<Int_t>                 fColor ;
    /// The fill style
    std::optional<Int_t>                 fStyle ;
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
    /// The track D0 parameter
    std::optional<float>                               fD0 {} ;
    /// The track phi parameter
    std::optional<float>                               fPhi {} ;
    /// The track omega parameter
    std::optional<float>                               fOmega {} ;
    /// The track Z0 parameter
    std::optional<float>                               fZ0 {} ;
    /// The track tan lambda parameter
    std::optional<float>                               fTanLambda {} ;
  };


  /// TrackState struct
  struct TrackState : public TrackInfo {
    /// The track state type
    std::optional<TrackStateType>                      fType {} ;
    /// The value of the B field at the track state position
    std::optional<float>                               fBField {} ;
  };


  /// TrackParameters struct
  /// Necessary data to create a track in Eve
  struct TrackParameters : public TrackInfo {
    /// The track line attributes
    std::optional<LineAttributes>                      fLineAttributes {} ;
    /// The track marker attributes
    std::optional<MarkerAttributes>                    fMarkerAttributes {} ;
    /// The track propagator
    std::optional<ROOT::REveTrackPropagator*>          fPropagator {} ;
    /// The track starting position (vertex)
    std::optional<ROOT::REveVectorT<float>>            fStartPosition {} ;
    /// The track momentum at start position
    std::optional<ROOT::REveVectorT<float>>            fStartMomentum {} ;
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
    /// Additional vertex properties
    PropertyMap                                        fProperties {} ;
  };

  /// CaloHitParameters struct
  /// Necessary data to describe a calo hit
  struct CaloHitParameters {
    /// The Eve parent box set in which the calo hit will be appended
    std::optional<ROOT::REveBoxSet*>                   fParentBoxSet {} ;
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
    /// The cluster position
    std::optional<ROOT::REveVectorT<float>>            fPosition {} ;
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
    /// The reco particle position
    std::optional<ROOT::REveVectorT<float>>            fPosition {} ;
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

  class ObjectHelper {
  public:
    /// Compute the track momentum from track properties
    static ROOT::REveVectorT<float> ComputeMomentum( float bfield, float omega, float phi, float tanLambda ) ;
  };

#pragma message "TODO: Added missing classes for MCParticle and TrackerHit"

}
