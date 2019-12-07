
// -- lceve headers
#include <LCEve/EventDisplay.h>
#include <LCEve/Geometry.h>
#include <LCEve/BField.h>

// -- root headers
#include <ROOT/REveManager.hxx>
#include <ROOT/REveElement.hxx>
#include <ROOT/REveGeoShape.hxx>
#include <ROOT/REveScene.hxx>
#include <ROOT/REveTrans.hxx>
#include <TGeoVolume.h>
#include <TGeoManager.h>
#include <TGeoShape.h>
#include <TGeoNode.h>
#include <TGeoShapeAssembly.h>

// -- dd4hep headers
#include <DD4hep/Detector.h>
#include <DD4hep/Objects.h>
#include <DD4hep/DetElement.h>
#include <DD4hep/Volumes.h>
#include <XML/DocumentHandler.h>

namespace lceve {

  Geometry::Geometry( EventDisplay *lced ) :
    fEventDisplay(lced) {
    fBField = new BField(lced) ;
  }

  //--------------------------------------------------------------------------

  Geometry::~Geometry() {
    delete fBField ;
  }

  //--------------------------------------------------------------------------

  void Geometry::LoadCompactFile( const std::string &compactFile ) {
    if( GeometryLoaded() ) {
      std::cout << "WARNING: Geometry already loaded! Not loading again..." << std::endl ;
      return ;
    }
    dd4hep::Detector& theDetector = dd4hep::Detector::getInstance() ;
    theDetector.fromCompact( compactFile ) ;
    fDetectorName = ExtractDetectorName( compactFile ) ;
    std::cout << "Detector name: "<< fDetectorName << std::endl ;
    std::cout << "Loading geometry in Eve. Please be patient..." << std::endl ;
    LoadGeometry( theDetector ) ;
    // Cache a few geometry variables
    this->CacheVariables() ;
    std::cout << "Loading geometry: done!" << std::endl ;
    fLoaded = true ;
  }

  //--------------------------------------------------------------------------

  bool Geometry::GeometryLoaded() const {
    return fLoaded ;
  }

  //--------------------------------------------------------------------------

  const dd4hep::Detector &Geometry::GetDetector() const {
    return dd4hep::Detector::getInstance() ;
  }

  //--------------------------------------------------------------------------

  const std::string &Geometry::GetDetectorName() const {
    return fDetectorName ;
  }

  //--------------------------------------------------------------------------

  ROOT::REveTrackPropagator *Geometry::CreateTrackPropagator() const {
    auto prop = new ROOT::REveTrackPropagator() ;
    prop->SetMagFieldObj( fBField, false ) ;
    prop->SetMaxOrbs(5) ;
    prop->SetMaxR( fPropagatorMaxR ) ;
    prop->SetMaxZ( fPropagatorMaxZ ) ;
    return prop ;
  }

  //--------------------------------------------------------------------------

  ROOT::REveMagField *Geometry::GetBField() const {
    return fBField ;
  }

  //--------------------------------------------------------------------------

  const dd4hep::rec::LayeredCalorimeterData *Geometry::GetLayeredCaloData(unsigned int includeFlag, unsigned int excludeFlag ) const {
#pragma message "Move Geometry::GetLayeredCaloData method in a geometry helper class"
    dd4hep::Detector &mainDetector = dd4hep::Detector::getInstance() ;
    const auto& theDetectors = dd4hep::DetectorSelector(mainDetector).detectors( includeFlag, excludeFlag ) ;
    if( theDetectors.size()  != 1 ) {
      std::stringstream es ;
      es << " getExtension: selection is not unique (or empty)  includeFlag: " << dd4hep::DetType( includeFlag ) << " excludeFlag: " << dd4hep::DetType( excludeFlag )
        << " --- found detectors : " ;
      for( unsigned i=0, N= theDetectors.size(); i<N ; ++i ){
        es << theDetectors.at(i).name() << ", " ;
      }
      throw std::runtime_error( es.str() ) ;
    }
    return theDetectors.at(0).extension<dd4hep::rec::LayeredCalorimeterData>() ;
  }

  //--------------------------------------------------------------------------

  ROOT::REveElement *Geometry::LoadDetElement( dd4hep::DetElement de, int levels, ROOT::REveElement* parent ) {
    dd4hep::PlacedVolume pv = de.placement() ;
    if (pv.isValid()) {
      TGeoNode* n = pv.ptr() ;
      TGeoMatrix* matrix = n->GetMatrix() ;
      gGeoManager = nullptr ;
      gGeoManager = new TGeoManager() ;
      ROOT::REveElement* e = CreateEveShape(0, levels, parent, n, *matrix, de.name()) ;
      if ( e )  {
        e->SetName( de.name() ) ;
      }
      return e;
    }
    return nullptr ;
  }

  //--------------------------------------------------------------------------

  ROOT::REveElement *Geometry::CreateEveShape( int level, int max_level, ROOT::REveElement *p, TGeoNode *n, const TGeoHMatrix& mat, const std::string& nam ) {
    TGeoVolume* vol = n ? n->GetVolume() : 0;
    if ( 0 == vol || level > max_level ) {
      return nullptr ;
    }
    dd4hep::VisAttr vis( dd4hep::Volume(vol).visAttributes() ) ;
    TGeoShape* geoShape = vol->GetShape();
    ROOT::REveElement* element = 0;
    if ( p )   {
      TGeoNode* pn = (TGeoNode*)p->GetUserData();
      if ( pn == n )  {
        element = p;
      }
      if ( !element )  {
        element = (ROOT::REveGeoShape*)p->FindChild(n->GetName());
      }
      if ( !element && !nam.empty() )  {
        element = (ROOT::REveGeoShape*)p->FindChild(nam.c_str());
      }
      if ( element ) goto Daughters;
    }
    if ( geoShape->IsA() == TGeoShapeAssembly::Class() )  {
      ROOT::REveElement* shape = new ROOT::REveElement( n->GetName(), n->GetName() ) ;
      shape->SetEditMainTransparency( true ) ;
      shape->SetEditMainColor( true ) ;
      shape->SetUserData(n);
      shape->SetMainTransparency(true);
      shape->SetMainAlpha(0.2);
      shape->SetPickable(true);
      if ( vis.isValid() )  {
        float r,g,b;
        vis.rgb(r,g,b);
        shape->SetMainColorRGB(r,g,b);
      }
      element = shape;
      goto Daughters;
    }
    else if ( 0 == element )  {
      ROOT::REveGeoShape* shape = new ROOT::REveGeoShape(n->GetName());
      if ( vis.isValid() )  {
        float r,g,b;
        vis.rgb(r,g,b);
        shape->SetMainColorRGB(r,g,b);
      }
      shape->SetEditMainTransparency( true ) ;
      shape->SetEditMainColor( true ) ;
      shape->SetMainTransparency(true);
      shape->SetMainAlpha(0.2);
      shape->SetPickable(true);
      shape->RefMainTrans().SetFrom(mat);
      shape->SetShape((TGeoShape*)geoShape->Clone());
      if ( level < max_level ) {
        shape->SetRnrSelfChildren(true,true);
      }
      else if ( level == max_level ) {
        shape->SetRnrSelfChildren(true,false);
      }
      else if ( level > max_level ) {
        shape->SetRnrSelfChildren(false,false);
      }
      shape->SetUserData(n);
      element = shape;
    }
  Daughters:
    for (Int_t idau = 0, ndau = n->GetNdaughters(); idau < ndau; ++idau) {
      TGeoNode* daughter = n->GetDaughter(idau);
      TGeoHMatrix dau_mat(mat);
      TGeoMatrix* matrix = daughter->GetMatrix();
      dau_mat.Multiply(matrix);
      ROOT::REveElement* dau_shape = CreateEveShape(level+1, max_level, element, daughter, dau_mat, daughter->GetName());
      if ( dau_shape )  {
        element->AddElement(dau_shape);
      }
    }
    return element;
  }

  //--------------------------------------------------------------------------

  void Geometry::LoadGeometry( dd4hep::Detector &detector ) {
    dd4hep::DetElement world = detector.world();
    int levels = fEventDisplay->GetSettings().GetDetectorLevel() ;
    const dd4hep::DetElement::Children& c = world.children();
    if ( c.size() == 0 )   {
      std::cout << "It looks like there is no Geometry loaded. No event display availible." << std::endl ;
    }
    else if ( levels > 0 ) {
      auto parent = fEventDisplay->GetEveManager()->GetGlobalScene() ;
      for (auto i = c.begin(); i != c.end(); ++i) {
        std::cout << "  Loading detector " << (*i).first << " ..." << std::endl ;
        dd4hep::DetElement de = (*i).second ;
        ROOT::REveElement* e = LoadDetElement(de, levels, parent);
        if ( e )  {
          parent->AddElement( e ) ;
        }
      }
    }
  }

  //--------------------------------------------------------------------------

  std::string Geometry::ExtractDetectorName( const std::string &compactFile ) const {
    std::string detectorName = compactFile ;
    auto last = detectorName.rfind( "/" ) ;
    if( last != std::string::npos ) {
      detectorName = detectorName.substr( last+1 ) ;
    }
    last = detectorName.rfind( "." ) ;
    if( last != std::string::npos ) {
      detectorName = detectorName.substr( 0, last ) ;
    }
    try {
      dd4hep::xml::DocumentHandler handler;
      auto doc = handler.load( compactFile ).root() ;
      if( doc.hasChild("info") ) {
        auto info = doc.child("info") ;
        if( info.hasAttr("name") ) {
          detectorName = info.attr_value( "name" ) ;
          std::cout << "From compact file root/info attr name: " << detectorName << std::endl ;
        }
      }
    }
    catch(...) {}
    return detectorName ;
  }
  
  //--------------------------------------------------------------------------
  
  void Geometry::CacheVariables() {
    auto barrelData = GetLayeredCaloData(
      ( dd4hep::DetType::CALORIMETER | dd4hep::DetType::ELECTROMAGNETIC | dd4hep::DetType::BARREL),
      ( dd4hep::DetType::AUXILIARY  |  dd4hep::DetType::FORWARD ) ) ;
    auto endcapData = GetLayeredCaloData(
      ( dd4hep::DetType::CALORIMETER | dd4hep::DetType::ELECTROMAGNETIC | dd4hep::DetType::ENDCAP),
      ( dd4hep::DetType::AUXILIARY  |  dd4hep::DetType::FORWARD ) ) ;
    fPropagatorMaxR = barrelData->extent[0] ;
    fPropagatorMaxZ = endcapData->extent[2] ;
  }

}
