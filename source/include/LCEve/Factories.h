#pragma once

// -- dd4hep headers
#include <DD4hep/Plugins.h>
#include <DD4hep/Primitives.h>

// forward declarations of plugins
namespace lceve {
  class ICollectionConverter ;
}

namespace {
  /// Factory template to create LC collection converter
  DD4HEP_PLUGIN_FACTORY_ARGS_0(lceve::ICollectionConverter*)
  {    return (lceve::ICollectionConverter*)new P();   }
}

// Plugin declaration macros for collection converter
#define LCEVE_DECLARE_CONVERTER_NS(name_space,name) typedef name_space::name __##name##__; \
  DD4HEP_PLUGINSVC_FACTORY(__##name##__,name,lceve::ICollectionConverter*(),__LINE__)

#define LCEVE_DECLARE_CONVERTER(name) \
  LCEVE_DECLARE_CONVERTER_NS(lceve, name)
