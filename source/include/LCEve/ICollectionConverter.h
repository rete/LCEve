#pragma once

// -- std headers
#include <optional>
#include <string>
#include <map>

// -- lceve headers
#include <LCEve/EventDisplay.h>
#include <LCEve/ROOTTypes.h>

// -- lcio headers
#include <UTIL/BitField64.h>

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
    virtual ROOT::REveElement* ProcessCollection( const std::string &name, const EVENT::LCCollection *const collection ) = 0 ;
    
    /// Set the event display instance and input parameters
    void Initialize( EventDisplay *lceve, ParameterMap_t parameters ) ;
    
  protected:
    /// Get the event display
    EventDisplay *GetEventDisplay() const ;
    
    /// Get a parameter
    template <typename T>
    std::optional<T> GetParameter( const std::string &key ) const ;
    
    /// Get a parameter as vector
    template <typename T>
    std::optional<std::vector<T>> GetParameters( const std::string &key ) const ;
    
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
  
  inline EventDisplay *ICollectionConverter::GetEventDisplay() const {
    return fEventDisplay ;
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
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline std::optional<std::vector<T>> ICollectionConverter::GetParameters( const std::string &key ) const {
    auto iter = fParameters.find( key ) ;
    if( fParameters.end() == iter ) {
      return std::nullopt ;
    }
    std::vector<std::string> tokensStr ;
    UTIL::LCTokenizer tokenizer( tokensStr, ' ' ) ;
    std::for_each( iter->second.begin(), iter->second.end(), tokenizer ) ;
    std::vector<T> values {} ;
    values.reserve( tokensStr.size() ) ;
    for( auto &tok : tokensStr ) {
      std::stringstream ss(tok) ;
      T value ;
      if( (ss >> value).fail() ) {
        return std::nullopt ;
      }
      values.push_back( value ) ;
    }
    return values ;
  }
  
}