
// -- lceve headers
#include <LCEve/EventDisplay.h>
#include <LCEve/EventNavigator.h>
#include <LCEve/LCCollectionBuilder.h>
#include <LCEve/Geometry.h>

// -- tclap headers
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

// -- root headers
#include <ROOT/REveScene.hxx>
#include <TEnv.h>

// -- lcio headers
#include <EVENT/LCEvent.h>

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
    /// Create and parse the command line
    TCLAP::CmdLine cmd("LCEve: Linear Collider EVEnt display", ' ', "master") ;

    TCLAP::MultiArg<std::string> lcioFilesArg( "f", "lcio-file",
      "The input LCIO file(s)", false, "vector<string>") ;
    cmd.add( lcioFilesArg ) ;

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

    TCLAP::ValueArg<int> portArg( "p", "port",
      "The http port to use", false, 0, "int") ;
    cmd.add( portArg ) ;

    TCLAP::ValueArg<int> detectorLevelArg( "l", "detector-level",
      "The detector depth level to load", false, 1, "int") ;
    cmd.add( detectorLevelArg ) ;

    cmd.parse( argc, argv ) ;

    /// Fill the application settings with parsed values
    if( readColNamesArg.isSet() ) {
      _settings.SetReadCollectionNames( readColNamesArg.getValue() ) ;
    }
    _settings.SetServerMode( serverModeArg.getValue() ) ;
    _settings.SetDSTMode( dstModeArg.getValue() ) ;
    _settings.SetDetectorLevel( detectorLevelArg.getValue() ) ;
    if( portArg.isSet() ) {
      gEnv->SetValue( "WebGui.HttpPort", portArg.getValue() ) ;
    }

    // Create the ROOT application running the event loop
    _application = new TApplication( "LCEve application", nullptr, nullptr ) ;

    /// Create the Eve manager
    _eveManager = REX::REveManager::Create() ;
    _eveManager->GetWorld()->AddElement( this ) ;
    _eveManager->GetWorld()->AddCommand( "QuitRoot", "sap-icon://log", this, "QuitRoot()" ) ;

    /// Load the DD4hep compact file
    _geometry->LoadCompactFile( compactFileArg.getValue() ) ;
    /// Initialize the LCIO event navigator
    _navigator->Init() ;
    /// Open the LCIO files if any
    if( lcioFilesArg.isSet() ) {
      _navigator->Open( lcioFilesArg.getValue() ) ;
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

  //--------------------------------------------------------------------------

  void EventDisplay::VisualizeEvent( const EVENT::LCEvent *const event ) {
    // Cleanup current event scene
    GetEveManager()->DisableRedraw() ;
    auto scene = GetEveManager()->GetEventScene() ;
    scene->DestroyElements() ;
    // Load new event in event scene
    LCCollectionConverter converter( this ) ;
    converter.VisualizeEvent( event, scene ) ;
    /// Send event to clients
    GetEveManager()->EnableRedraw();
    GetEveManager()->DoRedraw3D();
  }

}
