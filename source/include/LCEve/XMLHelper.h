#pragma once

// -- std headers
#include <vector>
#include <map>

// -- tinyxml headers
#include <tinyxml.h>

namespace lceve {
  
  struct CollectionConfig {
    using ParameterMap_t = std::map<std::string, std::string> ;  
    /// The collection name
    std::string       fName {} ;
    /// The plugin converter name
    std::string       fPluginName {} ;
    /// The collection parameter map
    ParameterMap_t    fParameters {} ;
  };
  using CollectionConfigList_t = std::vector<CollectionConfig> ;
  
  /// XMLHelper class
  /// Helper class to read out configuration from the config XML file 
  class XMLHelper {
  public:
    /// Read the XML section describe the collections config.
    /// 'element' is the parent XML elemnt of the <collection> tags
    static void ReadCollectionsConfig( const TiXmlElement *element, CollectionConfigList_t &config ) ;
    
    /// Get the parameter value. Look for a child element 'parameter' with 
    /// the attribute 'name' and returns the inner element text 
    static std::string GetParameterValue( const TiXmlElement *element, const std::string &name ) ;
  };
  
}