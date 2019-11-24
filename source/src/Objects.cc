
#include <LCEve/Objects.h>


#include <ROOT/REveVector.hxx>

namespace lceve {

  ROOT::REveVector ObjectHelper::ComputeMomentum( float bfield, float omega, float phi, float tanLambda ) {
    const double pt(bfield* 2.99792e-4 / std::fabs(omega));
    const double px(pt * std::cos(phi));
    const double py(pt * std::sin(phi));
    const double pz(pt * tanLambda);
    return ROOT::REveVector( px, py, pz ) ;
  }

}
