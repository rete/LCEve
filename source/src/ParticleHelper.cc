
// -- lceve headers
#include <LCEve/ParticleHelper.h>

// -- root headers
#include <TDatabasePDG.h>

// -- std headers
#include <string>
#include <limits>

namespace lceve {
  
  TParticlePDG *ParticleHelper::GetParticle( Int_t pdg ) {
    return TDatabasePDG::Instance()->GetParticle( pdg ) ;
  }
  
  //--------------------------------------------------------------------------

  bool ParticleHelper::IsStable( TParticlePDG *particle ) {
    return particle->Stable() == 1 ;
  }

  //--------------------------------------------------------------------------

  bool ParticleHelper::IsHadron( TParticlePDG *particle ) {
    // Check if it has c, b, s or t quarks
    std::string particleClass( particle->ParticleClass() ) ;
    if( particleClass.find("Meson") != std::string::npos ) {
      return true ;
    }
    if( particleClass.find("Baryon") != std::string::npos ) {
      return true ;
    }
    return false ;
  }

  //--------------------------------------------------------------------------

  bool ParticleHelper::IsNeutral( TParticlePDG *particle ) {
    return (fabs( particle->Charge() < std::numeric_limits<Double_t>::epsilon() )) ; 
  }
  
  //--------------------------------------------------------------------------

  bool ParticleHelper::IsElectron( TParticlePDG *particle ) {
    return (fabs( particle->PdgCode() ) == 11 ) ;
  }
  
  //--------------------------------------------------------------------------

  bool ParticleHelper::IsPhoton( TParticlePDG *particle ) {
    return ( particle->PdgCode() == 22 ) ;
  }
  
  //--------------------------------------------------------------------------

  bool ParticleHelper::IsMuon( TParticlePDG *particle ) {
    return (fabs( particle->PdgCode() ) == 13 ) ;  
  }

}