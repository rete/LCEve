
// -- lceve headers
#include <LCEve/EventDisplay.h>
#include <LCEve/EventNavigator.h>
#include <LCEve/Geometry.h>

// -- tclap headers
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

// -- root headers
#include <ROOT/REveScene.hxx>
#include <TEnv.h>

ClassImp( lceve::EventDisplay )

namespace lceve {

  EventDisplay::EventDisplay() {
    SetName( "EventDisplay" ) ;
    _navigator = new EventNavigator( this ) ;
    _geometry = new Geometry( this ) ;
  }

  //--------------------------------------------------------------------------

  EventDisplay::~EventDisplay() {
    if(_application) delete _application ;
    delete _navigator ;
    delete _geometry ;
  }

  //--------------------------------------------------------------------------

  REX::REveManager *EventDisplay::GetEveManager() const {
    return _eveManager ;
  }

  //--------------------------------------------------------------------------

  EventNavigator *EventDisplay::GetEventNavigator() const {
    return _navigator ;
  }

  //--------------------------------------------------------------------------

  TApplication *EventDisplay::GetApplication() const  {
    return _application ;
  }

  //--------------------------------------------------------------------------

  Geometry *EventDisplay::GetGeometry() const  {
    return _geometry ;
  }

  //--------------------------------------------------------------------------

  const Settings &EventDisplay::GetSettings() const {
    return _settings ;
  }

  //--------------------------------------------------------------------------

  void EventDisplay::Init( int argc, const char **argv ) {
    _application = new TApplication( "LCEve application", nullptr, nullptr ) ;

    TCLAP::CmdLine cmd("LCEve: Linear Collider EVEnt display", ' ', "master") ;

    TCLAP::ValueArg<std::string> lcioFileArg( "f", "lcio-file",
      "The input LCIO file", false, "", "string") ;
    cmd.add( lcioFileArg ) ;

    TCLAP::ValueArg<std::string> compactFileArg( "g", "geometry",
      "The DD4hep geometry compact file", true, "", "string") ;
    cmd.add( compactFileArg ) ;

    TCLAP::MultiArg<std::string> readColNamesArg( "c", "lcio-collections",
      "The list of LCIO collection to read only", false, "vector<string>") ;
    cmd.add( readColNamesArg ) ;

    TCLAP::SwitchArg serverModeArg( "s", "server-mode",
      "Whether to run in server mode. Do not show any window on startup but the url only", false) ;
    cmd.add( serverModeArg ) ;

    TCLAP::SwitchArg dstModeArg( "d", "dst-mode",
      "Whether to display LCIO event in DST mode. Draw only reconstructed quantities with dedicated display", false) ;
    cmd.add( dstModeArg ) ;

    TCLAP::ValueArg<int> detectorLevelArg( "l", "detector-level",
      "The detector depth level to load", true, 1, "int") ;
    cmd.add( detectorLevelArg ) ;

    cmd.parse( argc, argv ) ;

    if( readColNamesArg.isSet() ) {
      _settings.SetReadCollectionNames( readColNamesArg.getValue() ) ;
    }
    _settings.SetServerMode( serverModeArg.getValue() ) ;
    _settings.SetDSTMode( dstModeArg.getValue() ) ;
    _settings.SetDetectorLevel( detectorLevelArg.getValue() ) ;

    _eveManager = REX::REveManager::Create() ;
    _geometry->LoadCompactFile( compactFileArg.getValue() ) ;
    _eveManager->GetWorld()->AddElement( this ) ;
    _eveManager->GetWorld()->AddCommand( "QuitRoot", "sap-icon://log", this, "QuitRoot()" ) ;

    _navigator->Init() ;

    if( lcioFileArg.isSet() ) {
      _navigator->Open( lcioFileArg.getValue() ) ;
    }
  }

  //--------------------------------------------------------------------------

  void EventDisplay::Run() {
    if( GetSettings().GetServerMode() ) {
      gEnv->SetValue("WebEve.DisableShow", 1) ;
    }
    GetEveManager()->Show() ;
    GetApplication()->Run() ;
  }

  //--------------------------------------------------------------------------

  void EventDisplay::QuitRoot() {
    if( GetApplication() ) {
      GetApplication()->Terminate() ;
    }
  }

}
