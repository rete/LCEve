#include <ROOT/REveGeomViewer.hxx>
#include <TApplication.h>
namespace REX = ROOT::Experimental ;


int main (int argc, const char **argv) {
  
  TCLAP::CmdLine cmd("Linear Collider Geometry Viewer", ' ', "master") ;

  TCLAP::ValueArg<std::string> compactFileArg( "g", "geometry",
    "The DD4hep geometry compact file", true, "", "string") ;
  cmd.add( compactFileArg ) ;
  
  cmd.parse( argc, argv ) ;
  
  // Load DD4hep compact file
  auto &detector = dd4hep::Detector::getInstance() ;
  detector.fromCompact( compactFileArg.getValue() ) ;
  auto &manager = detector.manager() ;
  
  TApplication app( "LCGeometryViewer", nullptr, nullptr ) ;
  auto viewer = std::make_shared<REveGeomViewer>( &manager ) ;
  viewer->Show() ;
  app.Run() ;
  
  return 0 ;
}
