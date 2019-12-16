// -- lceve headers
#include <LCEve/XMLHelper.h>

// -- std headers
#include <stdexcept>

namespace lceve {
  
  void XMLHelper::ReadCollectionsConfig( const TiXmlElement *element, CollectionConfigList_t &config ) {
    for( auto col = element->FirstChildElement( "collection" ) ; nullptr != col ; col = col->NextSiblingElement( "collection" ) ) {
      CollectionConfig collectionConfig {} ;
      // read collection name
      const char* name = col->Attribute( "name" );
      if( nullptr == name ) {
        throw std::runtime_error( "XML <collection> tag has no attribute 'name'" ) ;
      }
      collectionConfig.fName = name ;
      // read plugin name
      const char* plugin = col->Attribute( "plugin" );
      if( nullptr == plugin ) {
        throw std::runtime_error( "XML <collection> tag has no attribute 'plugin'" ) ;
      }
      collectionConfig.fPluginName = plugin ;
      for( auto p = col->FirstChildElement( "parameter" ) ; nullptr != p ; p = p->NextSiblingElement( "parameter" ) ) {
        // read parameter name
        const char* key = p->Attribute( "name" );
        if( nullptr == key ) {
          throw std::runtime_error( "XML <parameter> tag has no attribute 'name'" ) ;
        }
        if( p->FirstChild() ) {
          auto value = p->FirstChild()->ValueStr() ;
          std::cout << "Collection " << name << ", adding parameter " << key << "=" << value << std::endl ;
          collectionConfig.fParameters[ key ] = value ;
        }
        else {
          throw std::runtime_error( "Couldn't extract parameter value from XML <parameter> tag" ) ;
        }
      }
      config.push_back( collectionConfig ) ;
    }
  }
  
}