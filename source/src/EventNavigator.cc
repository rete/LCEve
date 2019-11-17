
// -- lceve headers
#include <LCEve/EventNavigator.h>
#include <LCEve/EventDisplay.h>

// -- root headers
#include <TApplication.h>
#include <TGenericClassInfo.h>
#include <ROOT/REveManager.hxx>
#include <ROOT/REveScene.hxx>

// -- std headers
#include <iostream>
#include <algorithm>

ClassImp( lceve::EventNavigator )

namespace lceve {

  EventNavigator::EventNavigator( EventDisplay *lced ) :
    _eventDisplay( lced ) {
    SetName( "EventNavigator" ) ;
    _lcReader = new MT::LCReader( MT::LCReader::directAccess ) ; // random access ?
  }

  //--------------------------------------------------------------------------

  EventNavigator::~EventNavigator() {
    delete _lcReader ;
  }

  //--------------------------------------------------------------------------

  void EventNavigator::Init() {
    _eventDisplay->GetEveManager()->GetWorld()->AddElement( this ) ;
    _eventDisplay->GetEveManager()->GetWorld()->AddCommand("PreviousEvent", "sap-icon://media-reverse", this, "PreviousEvent()") ;
    _eventDisplay->GetEveManager()->GetWorld()->AddCommand("NextEvent", "sap-icon://media-play", this, "NextEvent()") ;
  }

  //--------------------------------------------------------------------------

  void EventNavigator::Open( const std::vector<std::string> &fnames ) {
    _lcReader->open( fnames ) ;
    _runEventIds.clear() ;
    EVENT::IntVec eventRunIds ;
    _lcReader->getEvents( eventRunIds ) ;
    for( unsigned int i=0 ; i<eventRunIds.size() ; i+=2 ) {
      _runEventIds.emplace_back( eventRunIds[i], eventRunIds[i+1] ) ;
    }
    _currentRunEvent = -1 ;
    std::sort( _runEventIds.begin(), _runEventIds.end(), []( const auto &lhs, const auto &rhs ){
      if( lhs.first < rhs.first ) {
        return true ;
      }
      if( (lhs.first == rhs.first) and (lhs.second < rhs.second) ) {
        return true ;
      }
      return false ;
    } ) ;
  }

  //--------------------------------------------------------------------------

  void EventNavigator::PreviousEvent() {
    if( _runEventIds.empty() ) {
      std::cout << "No LCIO file opened. No data to display..." << std::endl ;
      return ;
    }
    _currentEvent = nullptr ;
    if( _runEventIds.empty() or _currentRunEvent < 0 ) {
      std::cout << "WARNING: Couldn't load previous event" << std::endl ;
      return ;
    }
    _currentRunEvent--;
    auto iter = std::next( _runEventIds.begin(), _currentRunEvent ) ;
    _currentEvent = _lcReader->readEvent( iter->first, iter->second ) ;
    if( nullptr == _currentEvent ) {
      std::cout << "ERROR: read out nullptr event from lcio file" << std::endl ;
      return ;
    }
    std::cout << "PreviousEvent(): Loaded event " << _currentEvent->getEventNumber() <<
      ", run " << _currentEvent->getRunNumber() << std::endl ;
    _eventDisplay->VisualizeEvent( _currentEvent.get() ) ;
  }

  //--------------------------------------------------------------------------

  void EventNavigator::NextEvent() {
    if( _runEventIds.empty() ) {
      std::cout << "No LCIO file opened. No data to display..." << std::endl ;
      return ;
    }
    _currentEvent = nullptr ;
    auto iter = std::next( _runEventIds.begin(), _currentRunEvent+1 ) ;
    if( _runEventIds.empty() or  iter == _runEventIds.end() ) {
      std::cout << "WARNING: Couldn't load next event, EOF" << std::endl ;
      return ;
    }
    _currentRunEvent++;
    _currentEvent = _lcReader->readEvent( iter->first, iter->second ) ;
    if( nullptr == _currentEvent ) {
      std::cout << "ERROR: read out nullptr event from lcio file" << std::endl ;
      return ;
    }
    std::cout << "NextEvent(): Loaded event " << _currentEvent->getEventNumber() <<
      ", run " << _currentEvent->getRunNumber() << std::endl ;
    _eventDisplay->VisualizeEvent( _currentEvent.get() ) ;
  }

}
