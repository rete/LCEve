#pragma once

// -- std headers
#include <optional>
#include <string>
#include <map>

// -- lceve headers
#include <LCEve/EventDisplay.h>
#include <LCEve/ROOTTypes.h>

namespace EVENT {
  class LCCollection ;
}

namespace lceve {
  
  class EventDisplay ;
  
  /// Base class for converting a LCIO LCCollection to eve objects
  class ICollectionConverter {
  public:
    using ParameterMap_t = std::map<std::string, std::string> ;
        
    /// Default constructor
    ICollectionConverter() = default ;
    /// Default destructor
    virtual ~ICollectionConverter() = default ;
    
    /// Callback function to process a LCIO LCCollection
    virtual void ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) = 0 ;
    
    /// Set the event display instance and input parameters
    void Initialize( EventDisplay *lceve, ParameterMap_t parameters ) ;
    
  protected:
    /// Get the eve manager
    ROOT::REveManager *GetEveManager() const ;
    
    /// Get a parameter
    template <typename T>
    std::optional<T> GetParameter( const std::string &key ) const ;
    
  private:
    /// The event display instance
    EventDisplay          *fEventDisplay {nullptr} ;
    /// A list of helper parameters for converting collections
    ParameterMap_t         fParameters {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  inline void ICollectionConverter::Initialize( EventDisplay *lceve, ParameterMap_t parameters ) { 
    fEventDisplay = lceve ;
    fParameters = std::move( parameters ) ;
  }
  
  //--------------------------------------------------------------------------
  
  inline ROOT::REveManager *ICollectionConverter::GetEveManager() const {
    if( nullptr == fEventDisplay ) {
      return nullptr ;
    }
    return fEventDisplay->GetEveManager() ;
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline std::optional<T> ICollectionConverter::GetParameter( const std::string &key ) const {
    auto iter = fParameters.find( key ) ;
    if( fParameters.end() == iter ) {
      return std::nullopt ;
    }
    std::stringstream ss(iter->second) ;
    T value ;
    if( (ss >> value).fail() ) {
      return std::nullopt ;
    }
    return value ;
  }
  
}