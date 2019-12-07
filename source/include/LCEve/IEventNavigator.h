#pragma once

// -- lceve headers
#include <LCEve/ROOTTypes.h>
#include <LCEve/json.h>

// -- root headers
#include <ROOT/REveElement.hxx>
#include <ROOT/REveScene.hxx>
#include <Rtypes.h>

// -- std headers
#include <ctime>

namespace lceve {

  class EventDisplay ;

  /**
   *  @brief  IEventNavigator class
   *  Interface for navigating in input event stream.
   */
  class IEventNavigator : public ROOT::REveElement {
  public:
    IEventNavigator() = delete ;
    IEventNavigator( const IEventNavigator & ) = delete ;
    IEventNavigator &operator =( const IEventNavigator & ) = delete ;

    /// Constructor with event display
    IEventNavigator( EventDisplay *lced ) ;
    /// Destructor
    virtual ~IEventNavigator() = default ;

    /// Initialize the event navigator
    virtual void Init() = 0 ;
    /// Open a input files
    virtual void Open( const std::vector<std::string> &fnames ) = 0 ;
    /// Whether the input files are opened
    virtual bool IsOpened() const = 0 ;
    /// [Slot] Go to previous event
    virtual void PreviousEvent() = 0 ;
    /// [Slot] Go to next event
    virtual void NextEvent() = 0 ;
    /// Get the current event number
    virtual std::optional<int> GetCurrentEventNumber() const = 0 ;
    /// Get the current run number
    virtual std::optional<int> GetCurrentRunNumber() const = 0 ;
    /// Get the current run number
    virtual std::optional<std::time_t> GetCurrentEventTimeStamp() const = 0 ;
    /// Return the detector as read out from the current run info or event metadata
    /// If invalid, will use Geometry::GetDetectorName  
    virtual std::optional<std::string> GetDetectorName() const { return std::optional<std::string>{} ; }
    
  protected:
    /// Get the current event scene. The event scene is automatically cleared 
    /// by the framework before calling PreviousEvent() or NextEvent().
    /// This method can be used to access the Eve event scene while loading 
    /// the current event in PreviousEvent() or NextEvent()
    ROOT::REveScene *GetEventScene() const ;
    
  private:
    int WriteCoreJson(nlohmann::json &j, int rnr_offset) override ;

  private:
    EventDisplay                      *fEventDisplay {nullptr} ;
    
    ClassDef( IEventNavigator, 0 ) ;
  };

}
