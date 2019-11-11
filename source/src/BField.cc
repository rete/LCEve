
// -- lceve headers
#include <LCEve/BField.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/Geometry.h>

// --  dd4hep headers
#include <DD4hep/Detector.h>
#include <DD4hep/Fields.h>

namespace lceve {

  BField::BField( EventDisplay *lced ) :
    _eventDisplay(lced) {
    /* nop */
  }

  //--------------------------------------------------------------------------

  Bool_t BField::IsConst() const {
    return false ;
  }

  //--------------------------------------------------------------------------

  REX::REveVector BField::GetField(Float_t x, Float_t y, Float_t z) const {
    double pos[] = {static_cast<double>(x), static_cast<double>(y), static_cast<double>(z)} ;
    double bfield[3] = {0} ;
    _eventDisplay->GetGeometry()->GetDetector().field().magneticField(pos, bfield) ;
    return REX::REveVector( bfield ) ;
  }

  //--------------------------------------------------------------------------

  Float_t BField::GetMaxFieldMag() const {
    return GetField(0, 0, 0).Mag() ;
  }

}
