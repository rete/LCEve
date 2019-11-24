#pragma once

#include <Rtypes.h>

/// Nice header to deal with ROOT class definitions.
/// We might remove the Experimental namespace one day...

namespace ROOT {
  namespace Experimental {
    class REveManager ;
    class REveElement ;
    class REveTrackPropagator ;
    class REveMagField ;
    template <class TT> class REveVectorT ;
    class REveTrack ;
    class REveScene ;
    class REveGeomViewer ;
    class REveGeoShape ;
    class REveJetCone ;
    template <typename TT>
    class REveRecTrackT ;
    template <typename TT>
    class REvePathMarkT ;
    class REvePointSet ;
    class REveBoxSet ;
    class REveEllipsoid ;
  }
  // Import Experimental namespace in ROOT namespace,
  // not in global namespace
  using namespace Experimental ;
}
