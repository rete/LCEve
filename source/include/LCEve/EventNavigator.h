#pragma once

// -- root headers
#include <ROOT/REveElement.hxx>
#include <TClass.h>
#include <Rtypes.h>
namespace REX = ROOT::Experimental ;

// -- lcio headers
#include <MT/LCReader.h>
#include <EVENT/LCEvent.h>
#include <EVENT/LCRunHeader.h>

// -- std headers
#include <memory>
#include <unordered_map>

#include <LCEve/json.h>

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
    using RunHeaderMap = std::map<int, std::unique_ptr<EVENT::LCRunHeader>> ;

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

    /// Set whether to allow for the event navigation on the user interface
    void SetAllowUserNavigation( bool allow ) ;
    /// whether the event navigation on the user interface is allowed
    bool AllowUserNavigation() const ;

  private:
    int WriteCoreJson(nlohmann::json &j, int rnr_offset) override ;

  private:
    EventDisplay                      *_eventDisplay {nullptr} ;
    std::unique_ptr<MT::LCReader>      _lcReader {nullptr} ;
    std::unique_ptr<EVENT::LCEvent>    _currentEvent {nullptr} ;
    RunEventMap                        _runEventIds {} ;
    RunHeaderMap                       _runHeaderMap {} ;
    int                                _currentRunEvent {-1} ;
    bool                               _allowUserNavigation {true} ;

    ClassDef( EventNavigator, 0 ) ;
  };

}
