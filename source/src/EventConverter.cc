// -- lceve headers
#include <LCEve/EventConverter.h>
#include <LCEve/ICollectionConverter.h>
#include <LCEve/EventDisplay.h>

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
  
  void EventConverter::Init() {
    std::string pluginName = "LCTrackConverter" ;
    auto converter = dd4hep::PluginService::Create<ICollectionConverter*>( pluginName ) ;
    std::cout << "Created plugin instance " << pluginName << " from DD4hep: " << (void*)converter << std::endl ;
    if( nullptr == converter ) {
      throw std::runtime_error( "DD4hep collection converter plugin '" + pluginName + "' not found !" ) ;
    }
    converter->Initialize( fEventDisplay, {} ) ;
    std::shared_ptr<ICollectionConverter> converterPtr( converter ) ;
    fConverters.insert( {"MarlinTrkTracks", std::move(converterPtr)} ) ;      
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
