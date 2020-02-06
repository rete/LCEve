// -- lceve headers
#include <LCEve/ICollectionConverter.h>
#include <LCEve/LCObjectFactory.h>
#include <LCEve/EveElementFactory.h>
#include <LCEve/LCIOHelper.h>
#include <LCEve/DrawAttributes.h>
#include <LCEve/Geometry.h>
#include <LCEve/Factories.h>

// -- lcio headers
#include <EVENT/LCCollection.h>
#include <EVENT/ReconstructedParticle.h>

// -- std headers
#include <map>
#include <functional>

#include <ROOT/REveCompound.hxx>

namespace lceve {
  
  class LCRecoParticleConverter : public ICollectionConverter {
  public:
    /// Default constructor
    LCRecoParticleConverter() ;
    
    ///  Create reco particles out of EVENT::ReconstructedParticle objects
    ROOT::REveElement* ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) override ;
    
  private:
    using SortFunctionMap_t = LCIOHelper::SortFunctionMap_t<EVENT::ReconstructedParticle> ;    
    SortFunctionMap_t                fSortFunctions {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  LCRecoParticleConverter::LCRecoParticleConverter() {
    fSortFunctions[ "None" ] = [](const EVENT::ReconstructedParticle *, const EVENT::ReconstructedParticle *) {
      return false ; 
    } ;
    fSortFunctions[ "Energy" ] = [](const EVENT::ReconstructedParticle *lhs, const EVENT::ReconstructedParticle *rhs ) { 
      return lhs->getEnergy() > rhs->getEnergy() ;
    } ;
    fSortFunctions[ "Momentum" ] = [](const EVENT::ReconstructedParticle *lhs, const EVENT::ReconstructedParticle *rhs ) { 
      return ROOT::REveVectorT<float>( lhs->getMomentum() ).Mag() > ROOT::REveVectorT<float>( rhs->getMomentum() ).Mag() ;
    } ;
    fSortFunctions[ "Mass" ] = [](const EVENT::ReconstructedParticle *lhs, const EVENT::ReconstructedParticle *rhs ) { 
      return lhs->getMass() > rhs->getMass() ;
    } ;
    fSortFunctions[ "GoodnessOfPID" ] = [](const EVENT::ReconstructedParticle *lhs, const EVENT::ReconstructedParticle *rhs ) { 
      return lhs->getGoodnessOfPID() > rhs->getGoodnessOfPID() ;
    } ;
  }
  
  //--------------------------------------------------------------------------
  
  ROOT::REveElement* LCRecoParticleConverter::ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) {
    if( collection->getTypeName() != EVENT::LCIO::RECONSTRUCTEDPARTICLE ) {
      std::cout << "ERROR: Expected collection type EVENT::LCIO::RECONSTRUCTEDPARTICLE, got " << collection->getTypeName() << std::endl ;
      return nullptr ;
    }
    auto particles = LCIOHelper::CollectionAsVector<EVENT::ReconstructedParticle>( collection ) ;
    
    // Particle coloring
    auto color = GetParameter<std::string>( "Color" ).value_or( "iter" ) ;
    auto colorFunctor = ColorHelper::GetColorFunction( color ) ;
    
    // Sort particle
    auto sortPolicy = GetParameter<std::string>( "SortPolicy" ).value_or( "None" ) ;
    auto policyIter = fSortFunctions.find( sortPolicy ) ;
    if( fSortFunctions.end() == policyIter ) {
      policyIter = fSortFunctions.find( "None" ) ;
    }
    std::sort( particles.begin(), particles.end(), policyIter->second ) ;
    
    LCObjectFactory lcFactory( this->GetEventDisplay() ) ;
    EveElementFactory eveFactory( this->GetEventDisplay() ) ;
    
    auto eveParticleList = eveFactory.CreateRecoParticleContainer() ;
    eveParticleList->SetName( name ) ;
    eveParticleList->SetMainColor( kBlue ) ;

    for( auto lcParticle : particles ) {
      auto params = lcFactory.ConvertRecoParticle( lcParticle ) ;
      params.fColor = colorFunctor() ;
      auto eveParticle = eveFactory.CreateRecoParticle( params ) ;
      eveParticleList->AddElement( eveParticle ) ;
    }
    return eveParticleList.release() ;
  }
  
}

using namespace lceve ;
// Declare converter plugin
LCEVE_DECLARE_CONVERTER_NS(lceve, LCRecoParticleConverter)
