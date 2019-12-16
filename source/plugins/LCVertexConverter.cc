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
#include <EVENT/Vertex.h>

// -- root headers
#include <ROOT/REveEllipsoid.hxx>

// -- std headers
#include <map>
#include <functional>

namespace lceve {
  
  class LCVertexConverter : public ICollectionConverter {
  public:
    /// Default constructor
    LCVertexConverter() ;
    
    ///  Create vertices out of EVENT::Vertex objects
    ROOT::REveElement* ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) override ;
    
  private:
    using SortFunctionMap_t = LCIOHelper::SortFunctionMap_t<EVENT::Vertex> ;    
    SortFunctionMap_t                fSortFunctions {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  LCVertexConverter::LCVertexConverter() {
    fSortFunctions[ "None" ] = [](const EVENT::Vertex *, const EVENT::Vertex *) {
      return false ; 
    } ;
    fSortFunctions[ "Radius" ] = [](const EVENT::Vertex *lhs, const EVENT::Vertex *rhs ) { 
      return ROOT::REveVectorT<float>(lhs->getPosition()).R() > ROOT::REveVectorT<float>(rhs->getPosition()).R() ;
    } ;
    fSortFunctions[ "Distance" ] = [](const EVENT::Vertex *lhs, const EVENT::Vertex *rhs ) { 
      return ROOT::REveVectorT<float>(lhs->getPosition()).Mag() > ROOT::REveVectorT<float>(rhs->getPosition()).Mag() ;
    } ;
    fSortFunctions[ "Chi2" ] = [](const EVENT::Vertex *lhs, const EVENT::Vertex *rhs ) { 
      return lhs->getChi2() > rhs->getChi2() ;
    } ;
    fSortFunctions[ "Probability" ] = [](const EVENT::Vertex *lhs, const EVENT::Vertex *rhs ) { 
      return lhs->getProbability() > rhs->getProbability() ;
    } ;
  }
  
  //--------------------------------------------------------------------------
  
  ROOT::REveElement* LCVertexConverter::ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) {
    if( collection->getTypeName() != EVENT::LCIO::VERTEX ) {
      std::cout << "ERROR: Expected collection type EVENT::LCIO::VERTEX, got " << collection->getTypeName() << std::endl ;
      return nullptr ;
    }
    std::cout << "Converting vertex collection " << name << std::endl ;
    auto vertexs = LCIOHelper::CollectionAsVector<EVENT::Vertex>( collection ) ;
    
    // Vertex coloring
    auto color = GetParameter<std::string>( "Color" ).value_or( "iter" ) ;
    auto colorFunctor = ColorHelper::GetColorFunction( color ) ;
    
    // Sort vertex
    auto sortPolicy = GetParameter<std::string>( "SortPolicy" ).value_or( "None" ) ;
    auto policyIter = fSortFunctions.find( sortPolicy ) ;
    if( fSortFunctions.end() == policyIter ) {
      policyIter = fSortFunctions.find( "None" ) ;
    }
    std::sort( vertexs.begin(), vertexs.end(), policyIter->second ) ;
    
    LCObjectFactory lcFactory( this->GetEventDisplay() ) ;
    EveElementFactory eveFactory( this->GetEventDisplay() ) ;
    
    auto eveVertexList = eveFactory.CreateVertexContainer() ;
    eveVertexList->SetName( name ) ;
    eveVertexList->SetMainColor( kPink ) ;

    for( auto lcVertex : vertexs ) {
      auto params = lcFactory.ConvertVertex( lcVertex ) ;
      auto attr = params.fLineAttributes.value_or( LineAttributes {} ) ;
      attr.fColor = colorFunctor() ;
      params.fLineAttributes = attr ;
      auto eveVertex = eveFactory.CreateVertex( params ) ;
      eveVertexList->AddElement( eveVertex ) ;
    }
    return eveVertexList.release() ;
  }
  
}

using namespace lceve ;
// Declare converter plugin
LCEVE_DECLARE_CONVERTER_NS(lceve, LCVertexConverter)
