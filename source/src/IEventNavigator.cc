
// -- lceve headers
#include <LCEve/IEventNavigator.h>
#include <LCEve/Geometry.h>
#include <LCEve/EventDisplay.h>

// -- std headers
#include <ctime>
#include <sstream>

ClassImp(lceve::IEventNavigator)

namespace lceve {
  
  IEventNavigator::IEventNavigator( EventDisplay *lced ) :
    fEventDisplay(lced) {
    
  }
  
  //--------------------------------------------------------------------------
  
  ROOT::REveScene *IEventNavigator::GetEventScene() const {
    return fEventDisplay->GetEveManager()->GetEventScene() ;
  }
  
  //--------------------------------------------------------------------------
  
  int IEventNavigator::WriteCoreJson(nlohmann::json &j, int /*rnr_offset*/) {
    ROOT::REveElement::WriteCoreJson(j, -1) ;
    int eventNumber = this->GetCurrentEventNumber().value_or( -1 ) ;
    int runNumber = this->GetCurrentRunNumber().value_or( -1 ) ;
    auto now = time(0) ;
    auto timeStamp = this->GetCurrentEventTimeStamp().value_or( now ) ;
    auto detectorName = this->GetDetectorName().value_or( fEventDisplay->GetGeometry()->GetDetectorName() ) ; 
    std::stringstream timeStr ;
    std::tm tstm ;
    gmtime_r( &timeStamp, &tstm ) ;
    timeStr << std::put_time( &tstm,  "%T" ) << ((timeStamp == now) ? " (now)" : "") ;
    j["event"] = eventNumber ;
    j["run"] = runNumber ;
    j["date"] = timeStr.str() ;
    j["detector"] = detectorName ;
    j["UT_PostStream"] = "RefreshEventInfo" ;
    j["enableNavigation"] = this->IsOpened() ;
    return 0 ;
  }
  
}