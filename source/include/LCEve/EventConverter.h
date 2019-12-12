#pragma once

// -- std headers
#include <string>
#include <map>
#include <memory>

// -- lceve headers
#include <LCEve/ROOTTypes.h>

namespace EVENT {
  class LCEvent ;
}

namespace lceve {
  
  class EventDisplay ;
  class ICollectionConverter ;
  
  /// EventConverter class
  /// Converts a single LCIO event to eve element using collection converter plugins
  class EventConverter {
  public:
    using ConverterMap_t = std::map<std::string, std::shared_ptr<ICollectionConverter>> ;
    
  public:
    /// Constructor
    EventConverter( EventDisplay *lced ) ;
    
    /// Default destructor
    ~EventConverter() = default ;
    
    /// Initialize the event converter.
    /// Create and configure the collection converter plugins
    void Init() ;
    
    /// Load the event in the Eve event scene
    void VisualizeEvent( const EVENT::LCEvent *const event, ROOT::REveScene *eventScene ) ;
    
  private:
    /// Event display framework
    EventDisplay           *fEventDisplay {nullptr} ;
    /// The map of collection converters (collection name <-> converter)
    ConverterMap_t          fConverters {} ;
  };
  
}