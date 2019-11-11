
// -- lcio headers
#include <EVENT/Track.h>
#include <EVENT/LCCollection.h>

// -- root headers
#include <ROOT/REveElement.hxx>
#include <ROOT/REveTrack.hxx>
namespace REX = ROOT::Experimental ;

namespace lceve {

  class EventDisplay ;

  class LCCollectionConverter {
  public:
    LCCollectionConverter() = delete ;
    ~LCCollectionConverter() = default ;
    LCCollectionConverter(const LCCollectionConverter &) = delete ;
    LCCollectionConverter &operator =(const LCCollectionConverter &) = delete ;

    /// Constructor
    LCCollectionConverter( EventDisplay *lced ) ;

    /// Convert the lcio collection to a list of Eve elements
    REX::REveElement *CreateEveElementList( EVENT::LCCollection *collection ) ;
    /// Convert LCIO tracks to Eve tracks
    REX::REveElement *CreateEveTracks( const EVENT::LCCollection *const collection,
                                       const std::string &name,
                                       REX::REveElement *parent ) ;

  private:
    EventDisplay             *_eventDisplay {nullptr} ;
  };

}
