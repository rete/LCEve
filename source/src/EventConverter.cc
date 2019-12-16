// -- lceve headers
#include <LCEve/EventConverter.h>
#include <LCEve/ICollectionConverter.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/XMLHelper.h>

// -- lcio headers
#include <EVENT/LCEvent.h>

// -- dd4hep headers
#include <DD4hep/Plugins.h>

// -- root headers
#include <ROOT/REveScene.hxx>

namespace lceve {
  
  EventConverter::EventConverter( EventDisplay *lced ) :
    fEventDisplay(lced) {
    /* nop */
  }
  
  //--------------------------------------------------------------------------
  
  void EventConverter::Init( const TiXmlElement *element ) {
    
    CollectionConfigList_t colsConfig {} ;
    XMLHelper::ReadCollectionsConfig( element, colsConfig ) ;
    
    for( auto &c : colsConfig ) {
      auto converter = dd4hep::PluginService::Create<ICollectionConverter*>( c.fPluginName ) ;
      std::cout << "Created plugin instance " << c.fPluginName << " from DD4hep: " << (void*)converter << std::endl ;
      if( nullptr == converter ) {
        throw std::runtime_error( "DD4hep collection converter plugin '" + c.fPluginName + "' not found !" ) ;
      }
      converter->Initialize( fEventDisplay, c.fParameters ) ;
      std::shared_ptr<ICollectionConverter> converterPtr( converter ) ;
      fConverters.insert( {c.fName, std::move(converterPtr)} ) ;       
    }
     
  }
  
  //--------------------------------------------------------------------------
  
  void EventConverter::VisualizeEvent( const EVENT::LCEvent *const event, ROOT::REveScene *eventScene ) {
    for( auto &cvt : fConverters ) {
      std::string collectionName = cvt.first ;
      EVENT::LCCollection *collection = nullptr ;
      try {
        collection = event->getCollection( collectionName ) ;
      }
      catch( EVENT::DataNotAvailableException &e ) {
        std::cout << "Caught DataNotAvailableException: " << e.what() << std::endl ;
        continue ;
      }
      auto eveElement = cvt.second->ProcessCollection( collectionName, collection ) ;
      if( nullptr != eveElement ) {
        eventScene->AddElement( eveElement ) ;
      }
    }
  }
  
}
