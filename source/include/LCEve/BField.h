#pragma once

// -- root headers
#include <LCEve/ROOTTypes.h>
#include <ROOT/REveTrackPropagator.hxx>

namespace lceve {

  class EventDisplay ;

  /**
   *  @brief  BField class
   *  Wrap the DD4hep detector Bfield into Eve
   */
  class BField : public ROOT::REveMagField {
  public:
    BField() = delete ;
    BField(const BField &) = delete ;
    BField &operator =(const BField &) = delete ;

    /// Constructor
    BField( EventDisplay *lced ) ;

    /// Whether the B field is constant
    Bool_t IsConst() const override ;
    /// Get the B field at the given position
    ROOT::REveVector GetField(Float_t, Float_t, Float_t) const override ;
    /// Get the B field magnitude at the given position
    Float_t GetMaxFieldMag() const override ;

  private:
    EventDisplay               *fEventDisplay {nullptr} ;
  };

}
