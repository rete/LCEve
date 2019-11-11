#include <ROOT/REveManager.hxx>
#include <ROOT/REveElement.hxx>
#include <TApplication.h>
namespace REX = ROOT::Experimental ;

namespace lceve {

  class EventNavigator ;
  class Geometry ;

  /**
   *  @brief  EventDisplay class
   *  Central class in the event display framework.
   *  Provides access to all components of the event display
   */
  class EventDisplay : public REX::REveElement {
  public:
    // no copy
    EventDisplay( const EventDisplay & ) = delete ;
    EventDisplay &operator =( const EventDisplay & ) = delete ;

    /// Constructor
    EventDisplay() ;
    /// Destructor
    ~EventDisplay() ;

    /// Initialize the event display from command line argument
    void Init( int argc, const char **argv ) ;
    /// Run the event display
    void Run() ;
    /// [Slot] Quit the ROOT application
    void QuitRoot() ;

    /// Get the Eve manager instance
    REX::REveManager *GetEveManager() const ;
    /// Get the event navigator
    EventNavigator *GetEventNavigator() const ;
    /// Get the ROOT application
    TApplication *GetApplication() const ;
    /// Get the geometry handler
    Geometry *GetGeometry() const ;

  private:
    TApplication                     *_application {nullptr} ;
    REX::REveManager                 *_eveManager {nullptr} ;
    EventNavigator                   *_navigator {nullptr} ;
    Geometry                         *_geometry {nullptr} ;

    ClassDef( EventDisplay, 0 ) ;
  };

}