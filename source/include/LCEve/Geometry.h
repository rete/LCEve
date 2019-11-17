
// -- std headers
#include <string>

#include <DD4hep/Detector.h>
#include <DD4hep/DD4hepUnits.h>
#include <DDRec/DetectorData.h>
#include <DD4hep/DetType.h>
#include <DD4hep/DetectorSelector.h>

// -- root headers
#include <ROOT/REveTrackPropagator.hxx>
namespace REX = ROOT::Experimental ;

namespace lceve {

  class EventDisplay ;

  /**
   *  @brief  Geometry class
   *  Main interface to deal with DD4hep geometry
   */
  class Geometry {
  public:
    Geometry() = delete ;
    Geometry(const Geometry &) = delete ;
    Geometry &operator =(const Geometry &) = delete ;
    ~Geometry() ;

    /// Constructor with event display object
    Geometry( EventDisplay *lced ) ;

    /// Load the DD4hep compact file
    void LoadCompactFile( const std::string &compactFile ) ;
    /// Whether the geometry has been loaded
    bool GeometryLoaded() const ;
    /// Get the dd4hep detector instance
    const dd4hep::Detector &GetDetector() const ;
    /// Create a new track propagator
    REX::REveTrackPropagator *CreateTrackPropagator() const ;
    ///
    REX::REveMagField *GetBField() const ;
    ///
    const dd4hep::rec::LayeredCalorimeterData *GetLayeredCaloData(unsigned int includeFlag, unsigned int excludeFlag = 0) const ;


  private:
    static REX::REveElement *CreateEveShape( int level, int maxLevel, REX::REveElement *parent,
      TGeoNode *node, const TGeoHMatrix& mat, const std::string& name ) ;

    static REX::REveElement *LoadDetElement( dd4hep::DetElement det, int levels, REX::REveElement* parent ) ;

    void LoadGeometry( dd4hep::Detector &detector ) ;

  private:
    bool                              _loaded {false} ;
    EventDisplay                     *_eventDisplay {nullptr} ;
    REX::REveMagField                *_bfield {nullptr} ;
  };

}
