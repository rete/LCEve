
// -- lceve headers
#include <LCEve/BField.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/Geometry.h>

// --  dd4hep headers
#include <DD4hep/Detector.h>
#include <DD4hep/Fields.h>
#include <DD4hep/DD4hepUnits.h>

namespace lceve {

  BField::BField( EventDisplay *lced ) :
    fEventDisplay(lced) {
    /* nop */
  }

  //--------------------------------------------------------------------------

  Bool_t BField::IsConst() const {
    return false ;
  }

  //--------------------------------------------------------------------------

  ROOT::REveVector BField::GetField(Float_t x, Float_t y, Float_t z) const {
    double pos[] = {static_cast<double>(x), static_cast<double>(y), static_cast<double>(z)} ;
    double bfield[3] = {0} ;
    fEventDisplay->GetGeometry()->GetDetector().field().combinedMagnetic(pos, bfield) ;
    return ROOT::REveVector( -bfield[0] / dd4hep::tesla, -bfield[1] / dd4hep::tesla, -bfield[2] / dd4hep::tesla ) ;
  }

  //--------------------------------------------------------------------------

  Float_t BField::GetMaxFieldMag() const {
    return GetField(0, 0, 0).Mag() ;
  }

}
