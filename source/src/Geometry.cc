
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
namespace REX = ROOT::Experimental ;

// -- dd4hep headers
#include <DD4hep/Detector.h>
#include <DD4hep/Objects.h>
#include <DD4hep/DetElement.h>
#include <DD4hep/Volumes.h>

namespace lceve {

  Geometry::Geometry( EventDisplay *lced ) :
    _eventDisplay(lced) {
    _bfield = new BField(lced) ;
  }

  //--------------------------------------------------------------------------

  Geometry::~Geometry() {
    delete _bfield ;
  }

  //--------------------------------------------------------------------------

  void Geometry::LoadCompactFile( const std::string &compactFile ) {
    if( GeometryLoaded() ) {
    std::cout << "WARNING: Geometry already loaded! Not loading again..." << std::endl ;
      return ;
    }
    dd4hep::Detector& theDetector = dd4hep::Detector::getInstance() ;
    theDetector.fromCompact( compactFile ) ;
    std::cout << "Loading geometry in Eve. Please be patient..." << std::endl ;
    LoadGeometry( theDetector ) ;
    std::cout << "Loading geometry: done!" << std::endl ;
    _loaded = true ;
  }

  //--------------------------------------------------------------------------

  bool Geometry::GeometryLoaded() const {
    return _loaded ;
  }

  //--------------------------------------------------------------------------

  const dd4hep::Detector &Geometry::GetDetector() const {
    return dd4hep::Detector::getInstance() ;
  }

  //--------------------------------------------------------------------------

  REX::REveTrackPropagator *Geometry::CreateTrackPropagator() const {
    auto prop = new REX::REveTrackPropagator() ;
    prop->SetMagFieldObj( _bfield, false ) ;
    return prop ;
  }

  //--------------------------------------------------------------------------

  REX::REveMagField *Geometry::GetBField() const {
    return _bfield ;
  }

  //--------------------------------------------------------------------------

  const dd4hep::rec::LayeredCalorimeterData *Geometry::GetLayeredCaloData(unsigned int includeFlag, unsigned int excludeFlag ) const {
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

  REX::REveElement *Geometry::LoadDetElement( dd4hep::DetElement de, int levels, REX::REveElement* parent ) {
    dd4hep::PlacedVolume pv = de.placement() ;
    if (pv.isValid()) {
      TGeoNode* n = pv.ptr() ;
      TGeoMatrix* matrix = n->GetMatrix() ;
      gGeoManager = nullptr ;
      gGeoManager = new TGeoManager() ;
      REX::REveElement* e = CreateEveShape(0, levels, parent, n, *matrix, de.name()) ;
      if ( e )  {
        e->SetName( de.name() ) ;
      }
      return e;
    }
    return nullptr ;
  }

  //--------------------------------------------------------------------------

  REX::REveElement *Geometry::CreateEveShape( int level, int max_level, REX::REveElement *p, TGeoNode *n, const TGeoHMatrix& mat, const std::string& nam ) {
    TGeoVolume* vol = n ? n->GetVolume() : 0;
    if ( 0 == vol || level > max_level ) {
      return nullptr ;
    }
    dd4hep::VisAttr vis( dd4hep::Volume(vol).visAttributes() ) ;
    TGeoShape* geoShape = vol->GetShape();
    REX::REveElement* element = 0;
    if ( p )   {
      TGeoNode* pn = (TGeoNode*)p->GetUserData();
      if ( pn == n )  {
        element = p;
      }
      if ( !element )  {
        element = (REX::REveGeoShape*)p->FindChild(n->GetName());
      }
      if ( !element && !nam.empty() )  {
        element = (REX::REveGeoShape*)p->FindChild(nam.c_str());
      }
      if ( element ) goto Daughters;
    }
    if ( geoShape->IsA() == TGeoShapeAssembly::Class() )  {
      REX::REveElement* shape = new REX::REveElement( n->GetName(), n->GetName() ) ;
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
      REX::REveGeoShape* shape = new REX::REveGeoShape(n->GetName());
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
      REX::REveElement* dau_shape = CreateEveShape(level+1, max_level, element, daughter, dau_mat, daughter->GetName());
      if ( dau_shape )  {
        element->AddElement(dau_shape);
      }
    }
    return element;
  }

  //--------------------------------------------------------------------------

  void Geometry::LoadGeometry( dd4hep::Detector &detector ) {
    dd4hep::DetElement world = detector.world();
    int levels = _eventDisplay->GetSettings().GetDetectorLevel() ;
    const dd4hep::DetElement::Children& c = world.children();
    if ( c.size() == 0 )   {
      std::cout << "It looks like there is no Geometry loaded. No event display availible." << std::endl ;
    }
    else if ( levels > 0 ) {
      auto parent = _eventDisplay->GetEveManager()->GetGlobalScene() ;
      for (auto i = c.begin(); i != c.end(); ++i) {
        std::cout << "  Loading detector " << (*i).first << " ..." << std::endl ;
        dd4hep::DetElement de = (*i).second ;
        REX::REveElement* e = LoadDetElement(de, levels, parent);
        if ( e )  {
          parent->AddElement( e ) ;
        }
      }
    }
  }

}
