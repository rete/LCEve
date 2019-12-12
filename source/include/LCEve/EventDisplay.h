#pragma once

#include <ROOT/REveManager.hxx>
#include <ROOT/REveElement.hxx>
#include <TApplication.h>

// -- lceve headers
#include <LCEve/ROOTTypes.h>
#include <LCEve/Settings.h>

namespace EVENT {
  class LCEvent ;
}

namespace lceve {

  class EventNavigator ;
  class Geometry ;
  class EventConverter ;

  /**
   *  @brief  EventDisplay class
   *  Central class in the event display framework.
   *  Provides access to all components of the event display
   */
  class EventDisplay : public ROOT::REveElement {
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
    ROOT::REveManager *GetEveManager() const ;
    /// Get the event navigator
    EventNavigator *GetEventNavigator() const ;
    /// Get the ROOT application
    TApplication *GetApplication() const ;
    /// Get the geometry handler
    Geometry *GetGeometry() const ;
    /// Get the application settings
    const Settings &GetSettings() const ;

    /// Visualize the LCIO event
    void VisualizeEvent( const EVENT::LCEvent *const event ) ;

  private:
    TApplication                     *fApplication {nullptr} ;
    ROOT::REveManager                *fEveManager {nullptr} ;
    EventNavigator                   *fNavigator {nullptr} ;
    Geometry                         *fGeometry {nullptr} ;
    EventConverter                   *fEventConverter {nullptr} ;
    Settings                          fSettings {} ;

    ClassDef( EventDisplay, 0 ) ;
  };

}
