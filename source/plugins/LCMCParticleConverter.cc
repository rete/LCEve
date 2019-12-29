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
#include <EVENT/MCParticle.h>

// -- root headers
#include <ROOT/REveTrack.hxx>

// -- std headers
#include <map>
#include <functional>

namespace lceve {
  
  class LCMCParticleConverter : public ICollectionConverter {
  public:
    /// Default constructor
    LCMCParticleConverter() ;
    
    ///  Create mc particle out of EVENT::MCParticle objects
    ROOT::REveElement* ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) override ;
    
    /// Create a filter function for mc particles
    std::function<bool(const EVENT::MCParticle*)> GetMCParticleFilter() const ;
    
  private:
    using ColorFunctor_t = std::function<ColorType_t(Int_t)> ;
    /// Get the MC particle color functor
    ColorFunctor_t GetColorFunctor() const ;
    
  private:
    using SortFunctionMap_t = LCIOHelper::SortFunctionMap_t<EVENT::MCParticle> ;    
    SortFunctionMap_t                fSortFunctions {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  LCMCParticleConverter::LCMCParticleConverter() {
    fSortFunctions[ "None" ] = [](const EVENT::MCParticle *, const EVENT::MCParticle *) {
      return false ; 
    } ;
    fSortFunctions[ "Vertex" ] = [](const EVENT::MCParticle *lhs, const EVENT::MCParticle *rhs ) { 
      return ROOT::REveVectorT<double>(lhs->getVertex()).Mag() > ROOT::REveVectorT<double>(rhs->getVertex()).Mag() ;
    } ;
    fSortFunctions[ "Endpoint" ] = [](const EVENT::MCParticle *lhs, const EVENT::MCParticle *rhs ) { 
      return ROOT::REveVectorT<double>(lhs->getEndpoint()).Mag() > ROOT::REveVectorT<double>(rhs->getEndpoint()).Mag() ;
    } ;
    fSortFunctions[ "Momentum" ] = [](const EVENT::MCParticle *lhs, const EVENT::MCParticle *rhs ) { 
      return ROOT::REveVectorT<double>(lhs->getMomentum()).Mag() > ROOT::REveVectorT<double>(rhs->getMomentum()).Mag() ;
    } ;
    fSortFunctions[ "Mass" ] = [](const EVENT::MCParticle *lhs, const EVENT::MCParticle *rhs ) { 
      return lhs->getMass() > rhs->getMass() ;
    } ;
    fSortFunctions[ "Energy" ] = [](const EVENT::MCParticle *lhs, const EVENT::MCParticle *rhs ) { 
      return lhs->getEnergy() > rhs->getEnergy() ;
    } ;
  }
  
  //--------------------------------------------------------------------------
  
  ROOT::REveElement* LCMCParticleConverter::ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) {
    if( collection->getTypeName() != EVENT::LCIO::MCPARTICLE ) {
      std::cout << "ERROR: Expected collection type EVENT::LCIO::MCPARTICLE, got " << collection->getTypeName() << std::endl ;
      return nullptr ;
    }
    auto mcparticles = LCIOHelper::CollectionAsVector<EVENT::MCParticle>( collection ) ;
    
    // MCParticle coloring
    auto colorFunctor = GetColorFunctor() ;
    
    // Filter MC particles
    auto mcpFilter = GetMCParticleFilter() ;
    decltype(mcparticles) mcparticlesFiltered {} ;
    mcparticlesFiltered.reserve( mcparticles.size() ) ;
    if( nullptr != mcpFilter ) {
      std::copy_if( mcparticles.begin(), mcparticles.end(), std::back_inserter(mcparticlesFiltered),  mcpFilter ) ;
    }
    else {
      mcparticlesFiltered = mcparticles ;
    }
    
    // Sort mc particles
    auto sortPolicy = GetParameter<std::string>( "SortPolicy" ).value_or( "None" ) ;
    auto policyIter = fSortFunctions.find( sortPolicy ) ;
    if( fSortFunctions.end() == policyIter ) {
      policyIter = fSortFunctions.find( "None" ) ;
    }
    std::sort( mcparticlesFiltered.begin(), mcparticlesFiltered.end(), policyIter->second ) ;
    
    LCObjectFactory lcFactory( this->GetEventDisplay() ) ;
    EveElementFactory eveFactory( this->GetEventDisplay() ) ;
    
    auto propagator = GetEventDisplay()->GetGeometry()->CreateMCParticlePropagator() ;
    auto eveMCParticleList = eveFactory.CreateMCParticleContainer() ;
    eveMCParticleList->SetName( name ) ;
    eveMCParticleList->SetMainColor( kBlue ) ;

    for( auto lcMCParticle : mcparticlesFiltered ) {
      auto color = colorFunctor( lcMCParticle->getPDG() ) ;
      auto params = lcFactory.ConvertMCParticle( lcMCParticle ) ;
      auto lattr = params.fLineAttributes.value_or( LineAttributes {} ) ;
      auto mattr = params.fMarkerAttributes.value_or( MarkerAttributes {} ) ;
      lattr.fColor = color ;
      mattr.fColor = color ;
      params.fLineAttributes = lattr ;
      params.fMarkerAttributes = mattr ;
      auto eveMCParticle = eveFactory.CreateMCParticle( propagator, params ) ;
      eveMCParticleList->AddElement( eveMCParticle ) ;
    }
    return eveMCParticleList.release() ;
  }
  
  //--------------------------------------------------------------------------
  
  std::function<bool(const EVENT::MCParticle*)> LCMCParticleConverter::GetMCParticleFilter() const {
    std::function<bool(const EVENT::MCParticle*)> filter = [](const EVENT::MCParticle*) { return true ; } ;
    bool hasFilter = false ;
    auto minE = GetParameter<float>( "MinEnergy" ) ;
    if( minE.has_value() ) {
      filter = [filter=std::move(filter),minE=minE](const EVENT::MCParticle *mcp) {
        return (filter(mcp) && mcp->getEnergy() > minE.value() ) ;
      } ;
      hasFilter = true ;
    }
    auto maxE = GetParameter<float>( "MaxEnergy" ) ;
    if( maxE.has_value() ) {
      filter = [filter=std::move(filter),maxE=maxE](const EVENT::MCParticle *mcp) {
        return (filter(mcp) && mcp->getEnergy() < maxE.value() ) ;
      } ;
      hasFilter = true ;
    }
    auto pdgOnly = GetParameters<int>( "PDGOnly" ) ;
    if( pdgOnly.has_value() ) {
      auto pdgs = pdgOnly.value() ;
      filter = [filter=std::move(filter),pdgs=pdgs](const EVENT::MCParticle *mcp) {
        return (filter(mcp) && std::find( pdgs.begin(), pdgs.end(), mcp->getPDG() ) != pdgs.end() ) ;
      } ;
      hasFilter = true ;
    }
    auto genOnly = GetParameters<int>( "GeneratorStatusOnly" ) ;
    if( genOnly.has_value() ) {
      auto gen = genOnly.value() ;
      filter = [filter=std::move(filter),gen=gen](const EVENT::MCParticle *mcp) {
        return (filter(mcp) && std::find( gen.begin(), gen.end(), mcp->getGeneratorStatus() ) != gen.end() ) ;
      } ;
      hasFilter = true ;
    }
    return hasFilter ? filter : nullptr ;
  }
  
  //--------------------------------------------------------------------------
  
  LCMCParticleConverter::ColorFunctor_t LCMCParticleConverter::GetColorFunctor() const {
    auto colorStr = GetParameter<std::string>( "Color" ).value_or( "iter" ) ;
    auto colorFunctor = ColorHelper::GetColorFunction( colorStr ) ;
    std::transform(colorStr.begin(), colorStr.end(), colorStr.begin(), [](unsigned char c){ return std::tolower(c); } ) ;
    const bool usePdg = ("pdg" == colorStr ) ;
    return [=]( Int_t pdg ) {
      return usePdg ? ColorHelper::GetPDGColor( pdg ) : colorFunctor() ;
    };
  }
  
}

using namespace lceve ;
// Declare converter plugin
LCEVE_DECLARE_CONVERTER_NS(lceve, LCMCParticleConverter)
