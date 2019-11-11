
// -- lcio headers
#include <EVENT/LCEvent.h>
#include <EVENT/Track.h>
#include <EVENT/LCCollection.h>

// -- root headers
#include <ROOT/REveElement.hxx>
#include <ROOT/REveTrack.hxx>
#include <ROOT/REveScene.hxx>
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

    /// Load the LCIO event in the Eve scene
    void VisualizeEvent( const EVENT::LCEvent *event, REX::REveScene *scene ) ;

    /// Convert LCIO tracks to Eve tracks
    REX::REveElement *CreateEveTracks( const EVENT::LCCollection *const collection,
                                       const std::string &name ) ;

  private:
    EventDisplay             *_eventDisplay {nullptr} ;
  };

}
