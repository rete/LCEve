#pragma once

// -- root headers
#include <TParticlePDG.h>

namespace lceve {
  
  /// ParticleHelper class
  /// A collection of helper methods for particles
  class ParticleHelper {
  public:
    /// Find the particle from PDG id
    static TParticlePDG *GetParticle( Int_t pdg ) ;
    
    /// Whether the particle is stable
    static bool IsStable( TParticlePDG *particle ) ;
    
    /// Whether the particle is a hadron
    static bool IsHadron( TParticlePDG *particle ) ;
    
    /// Whether the particle is neutral
    static bool IsNeutral( TParticlePDG *particle ) ;
    
    /// Whether the particle is electron or positron
    static bool IsElectron( TParticlePDG *particle ) ;
    
    /// Whether the particle is a photon
    static bool IsPhoton( TParticlePDG *particle ) ;
    
    /// Whether the particle is a muon
    static bool IsMuon( TParticlePDG *particle ) ;
  };
  
}