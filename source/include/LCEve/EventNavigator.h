#pragma once

// -- root headers
#include <ROOT/REveElement.hxx>
#include <TClass.h>
#include <Rtypes.h>
namespace REX = ROOT::Experimental ;

// -- lcio headers
#include <MT/LCReader.h>
#include <EVENT/LCEvent.h>

// -- std headers
#include <memory>
#include <unordered_map>

namespace lceve {

  class EventDisplay ;

  /**
   *  @brief  EventNavigator class
   *  Wrap a LCReader from LCIO and catch previous/next
   *  event signals from the web frontend
   */
  class EventNavigator : public REX::REveElement {
  public:
    using RunEventMap = std::vector<std::pair<int, int>> ;

  public:
    EventNavigator() = delete ;
    EventNavigator( const EventNavigator & ) = delete ;
    EventNavigator &operator =( const EventNavigator & ) = delete ;

    /// Constructor with event display
    EventNavigator( EventDisplay *lced ) ;
    /// Destructor
    ~EventNavigator() ;

    /// Initialize the event navigator
    void Init() ;
    /// Open a new LCIO file
    void Open( const std::vector<std::string> &fnames ) ;
    /// [Slot] Go to previous event
    void PreviousEvent() ;
    /// [Slot] Go to next event
    void NextEvent() ;

  private:
    EventDisplay                      *_eventDisplay {nullptr} ;
    MT::LCReader                      *_lcReader {nullptr} ;
    std::unique_ptr<EVENT::LCEvent>    _currentEvent {nullptr} ;
    RunEventMap                        _runEventIds {} ;
    int                                _currentRunEvent {-1} ;

    ClassDef( EventNavigator, 0 ) ;
  };

}
