#pragma once

// -- std headers
#include <string>

// -- dd4hep headers
#include <DD4hep/Detector.h>
#include <DD4hep/DD4hepUnits.h>
#include <DDRec/DetectorData.h>
#include <DD4hep/DetType.h>
#include <DD4hep/DetectorSelector.h>

// -- root headers
#include <ROOT/REveTrackPropagator.hxx>

// -- lceve headers
#include <LCEve/ROOTTypes.h>

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
    /// Get the detector name. Valid only after reading the geometry
    const std::string &GetDetectorName() const ;

    /// Create a new track propagator
    ROOT::REveTrackPropagator *CreateTrackPropagator() const ;
    /// Create a new MC particle propagator
    ROOT::REveTrackPropagator *CreateMCParticlePropagator() const ;
    /// Get the global B field instance
    ROOT::REveMagField *GetBField() const ;
    /// Helper function to get the layered calorimeter data for a specific detector
    const dd4hep::rec::LayeredCalorimeterData *GetLayeredCaloData(unsigned int includeFlag, unsigned int excludeFlag = 0) const ;


  private:
    /// Recursive function creating an eve shape from a geo node
    static ROOT::REveElement *CreateEveShape( int level, int maxLevel, ROOT::REveElement *parent,
      TGeoNode *node, const TGeoHMatrix& mat, const std::string& name ) ;

    /// Load the detector element (top level function)
    static ROOT::REveElement *LoadDetElement( dd4hep::DetElement det, int levels, ROOT::REveElement* parent ) ;

    /// Load the DD4hep geometry in Eve 
    void LoadGeometry( dd4hep::Detector &detector ) ;

    /// Extract the detector out of the compact file
    /// Can be either the file name without extension
    /// or from the <info> XML element in the compact file itself
    std::string ExtractDetectorName( const std::string &compactFile ) const ;
    
    /// Cache geometry variables often accessed
    void CacheVariables() ;

  private:
    bool                              fLoaded {false} ;
    EventDisplay                     *fEventDisplay {nullptr} ;
    ROOT::REveMagField               *fBField {nullptr} ;
    std::string                       fDetectorName {"Unknown"} ;
    double                            fTrackMaxR {0.} ;
    double                            fTrackMaxZ {0.} ;
    double                            fMCParticleMaxR {0.} ;
    double                            fMCParticleMaxZ {0.} ;
  };

}
