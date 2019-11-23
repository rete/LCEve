
// -- lceve headers
#include <LCEve/EventNavigator.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/Geometry.h>

// -- root headers
#include <TApplication.h>
#include <TGenericClassInfo.h>
#include <ROOT/REveManager.hxx>
#include <ROOT/REveScene.hxx>

// -- std headers
#include <iostream>
#include <algorithm>
#include <ctime>

ClassImp( lceve::EventNavigator )

namespace lceve {

  EventNavigator::EventNavigator( EventDisplay *lced ) :
    _eventDisplay( lced ) {
    SetName( "EventNavigator" ) ;
  }

  //--------------------------------------------------------------------------

  EventNavigator::~EventNavigator() {
  }

  //--------------------------------------------------------------------------

  void EventNavigator::Init() {
    _eventDisplay->GetEveManager()->GetWorld()->AddElement( this ) ;
  }

  //--------------------------------------------------------------------------

  void EventNavigator::Open( const std::vector<std::string> &fnames ) {
    _lcReader = std::make_unique<MT::LCReader>( MT::LCReader::directAccess ) ;
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
    EVENT::IntVec runIds ;
    _lcReader->getRuns( runIds ) ;
    for( auto runId : runIds ) {
      auto run = _lcReader->readRunHeader( runId ) ;
      if( nullptr != run ) {
        _runHeaderMap[ runId ] = std::move(run) ;
      }
    }
    std::cout << "Loaded " << _runHeaderMap.size() << " run(s) from LCIO file" << std::endl ;
    StampObjProps();
  }

  //--------------------------------------------------------------------------

  void EventNavigator::PreviousEvent() {
    if( (nullptr == _lcReader) or (_runEventIds.empty()) ) {
      std::cout << "No LCIO file opened. No data to display..." << std::endl ;
      StampObjProps();
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
    StampObjProps();
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
    if( (nullptr == _lcReader) or (_runEventIds.empty()) ) {
      std::cout << "No LCIO file opened. No data to display..." << std::endl ;
      StampObjProps();
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
    StampObjProps();
    if( nullptr == _currentEvent ) {
      std::cout << "ERROR: read out nullptr event from lcio file" << std::endl ;
      return ;
    }
    std::cout << "NextEvent(): Loaded event " << _currentEvent->getEventNumber() <<
      ", run " << _currentEvent->getRunNumber() << std::endl ;
    _eventDisplay->VisualizeEvent( _currentEvent.get() ) ;
  }

  //--------------------------------------------------------------------------

  int EventNavigator::WriteCoreJson(nlohmann::json &j, int /*rnr_offset*/) {
    REX::REveElement::WriteCoreJson(j, -1) ;
    if( _currentEvent ) {
      // write event info
      std::stringstream timeStr ;
      if( _currentEvent->getTimeStamp() != 0 ) {
        std::tm tstm ;
        std::time_t ts_t = static_cast<std::time_t>(_currentEvent->getTimeStamp()) ;
        gmtime_r( &ts_t, &tstm ) ;
        timeStr << std::put_time( &tstm,  "%T" ) ;
      }
      else {
        std::tm tstm ;
        std::time_t ts_t = time(0) ;
        gmtime_r( &ts_t, &tstm ) ;
        timeStr << std::put_time( &tstm,  "%T" ) << " (now)" ;
      }
      j["event"] = _currentEvent->getEventNumber() ;
      j["run"] = _currentEvent->getRunNumber() ;
      j["date"] = timeStr.str() ;
      auto iter = _runHeaderMap.find( _currentEvent->getRunNumber() ) ;
      j["detector"] = (_runHeaderMap.end() == iter ) ?
        _eventDisplay->GetGeometry()->GetDetectorName() :
        iter->second->getDetectorName() ;
    }
    else {
      // write event info
      j["event"] = -1 ;
      j["run"] = -1 ;
      j["date"] = "";
      j["detector"] = _eventDisplay->GetGeometry()->GetDetectorName() ;
    }
    j["UT_PostStream"] = "RefreshEventInfo" ;
    j["enableNavigation"] = ((_allowUserNavigation) and (nullptr != _lcReader)) ;
    return 0 ;
  }

  //--------------------------------------------------------------------------

  void EventNavigator::SetAllowUserNavigation( bool allow ) {
    _allowUserNavigation = allow ;
  }

  //--------------------------------------------------------------------------

  bool EventNavigator::AllowUserNavigation() const {
    return _allowUserNavigation ;
  }

}
