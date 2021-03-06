
// -- lceve headers
#include <LCEve/EventDisplay.h>
#include <LCEve/EventNavigator.h>
#include <LCEve/EventConverter.h>
#include <LCEve/Geometry.h>
#include <LCEve/LCEveConfig.h>

// -- tclap headers
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

// -- root headers
#include <ROOT/REveScene.hxx>
#include <TEnv.h>

// -- lcio headers
#include <EVENT/LCEvent.h>

// -- tinyxml headers
#include <tinyxml.h>

ClassImp( lceve::EventDisplay )

namespace lceve {

  EventDisplay::EventDisplay() {
    SetName( "EventDisplay" ) ;
    fNavigator = new EventNavigator( this ) ;
    fGeometry = new Geometry( this ) ;
    fEventConverter = new EventConverter( this ) ; 
  }

  //--------------------------------------------------------------------------

  EventDisplay::~EventDisplay() {
    if(fApplication) delete fApplication ;
    delete fEventConverter ;
    delete fNavigator ;
    delete fGeometry ;
  }

  //--------------------------------------------------------------------------

  ROOT::REveManager *EventDisplay::GetEveManager() const {
    return fEveManager ;
  }

  //--------------------------------------------------------------------------

  EventNavigator *EventDisplay::GetEventNavigator() const {
    return fNavigator ;
  }

  //--------------------------------------------------------------------------

  TApplication *EventDisplay::GetApplication() const  {
    return fApplication ;
  }

  //--------------------------------------------------------------------------

  Geometry *EventDisplay::GetGeometry() const  {
    return fGeometry ;
  }

  //--------------------------------------------------------------------------

  const Settings &EventDisplay::GetSettings() const {
    return fSettings ;
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
    
    TCLAP::ValueArg<std::string> configArg( "c", "config",
      "The event display config file name", true, "", "string") ;
    cmd.add( configArg ) ;    

    TCLAP::SwitchArg serverModeArg( "s", "server-mode",
      "Whether to run in server mode. Do not show any window on startup but the url only", false) ;
    cmd.add( serverModeArg ) ;
    
    TCLAP::SwitchArg clearCacheArg( "z", "clear-cache",
      "Whether to clear the cache in browser on reload (only for Eve)", false) ;
    cmd.add( clearCacheArg ) ;

    TCLAP::ValueArg<int> portArg( "p", "port",
      "The http port to use", false, 0, "int") ;
    cmd.add( portArg ) ;

    TCLAP::ValueArg<int> detectorLevelArg( "l", "detector-level",
      "The detector depth level to load", false, 1, "int") ;
    cmd.add( detectorLevelArg ) ;

    cmd.parse( argc, argv ) ;

    /// Fill the application settings with parsed values
    fSettings.SetServerMode( serverModeArg.getValue() ) ;
    fSettings.SetDetectorLevel( detectorLevelArg.getValue() ) ;
    if( portArg.isSet() ) {
      gEnv->SetValue( "WebGui.HttpPort", portArg.getValue() ) ;
    }

    // Create the ROOT application running the event loop
    fApplication = new TApplication( "LCEve application", nullptr, nullptr ) ;

    /// Create the Eve manager
    fEveManager = ROOT::REveManager::Create() ;
    fEveManager->GetWorld()->AddElement( this ) ;
    
    TiXmlDocument document ;
    bool loadOkay = document.LoadFile( configArg.getValue() ) ;
    if( !loadOkay ) {
      std::stringstream str ;
      str  << "XMLParser::parse error in file [" << configArg.getValue()
          << ", row: " << document.ErrorRow() << ", col: " << document.ErrorCol() << "] : "
          << document.ErrorDesc() ;
      throw std::runtime_error( str.str() ) ;
    }
    auto root = document.RootElement() ;

    fEventConverter->Init( root ) ;
    /// Load the DD4hep compact file
    fGeometry->LoadCompactFile( compactFileArg.getValue(), root ) ;
    /// Initialize the LCIO event navigator
    fNavigator->Init() ;
    /// Open the LCIO files if any
    if( lcioFilesArg.isSet() ) {
      fNavigator->Open( lcioFilesArg.getValue() ) ;
    }

    // Initialize OpenUI custom scripts
    auto ui5Dir = std::string(LCEVE_DIR) + "/ui5" ;
    GetEveManager()->AddLocation("lceve/", ui5Dir) ;
    GetEveManager()->SetDefaultHtmlPage("file:lceve/lceve.html") ;
    
    if( clearCacheArg.getValue() ) {
      std::cout << "Will clear browser cache on page reload..." << std::endl ;
      gEnv->SetValue("WebGui.HttpMaxAge", 0) ;
    }
    if( serverModeArg.getValue() ) {
      std::cout << "Will start EveManager in server mode..." << std::endl ;
      gEnv->SetValue("WebEve.DisableShow", 1) ;
    }
  }

  //--------------------------------------------------------------------------

  void EventDisplay::Run() { 
    GetEveManager()->Show() ;
    GetApplication()->Run() ;
  }

  //--------------------------------------------------------------------------

  void EventDisplay::QuitRoot() {
    if( GetApplication() ) {
      std::cout << "Exiting application..." << std::endl ;
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
    fEventConverter->VisualizeEvent( event, scene ) ;
    /// Send event to clients
    GetEveManager()->EnableRedraw();
    GetEveManager()->DoRedraw3D();
  }

}
