
#include <LCEve/DrawAttributes.h>


namespace lceve {

  ColorIterator::ColorIterator() :
    ColorIterator( ColorIterStrategy::AUTO_ITER ) {

  }

  //--------------------------------------------------------------------------

  ColorIterator::ColorIterator( ColorIterStrategy strategy, int color ) :
    _strategy(strategy),
    _fixedColor(color) {
    _colorIter = std::find( _colors.begin(), _colors.end(), color );
    if( _colors.end() == _colorIter ) {
      _colorIter = _colors.begin() ;
    }
  }

  //--------------------------------------------------------------------------

  int ColorIterator::NextColor() {
    if( _strategy == ColorIterStrategy::AUTO_ITER ) {
      _colorIter++ ;
      if( _colors.end() == _colorIter ) {
        _colorIter = _colors.begin() ;
      }
    }
    return CurrentColor() ;
  }

  //--------------------------------------------------------------------------

  int ColorIterator::CurrentColor() const {
    if( _strategy == ColorIterStrategy::AUTO_ITER ) {
      return *_colorIter ;
    }
    else {
      return FixedColor() ;
    }
  }

  //--------------------------------------------------------------------------

  int ColorIterator::FixedColor() const {
    return _fixedColor ;
  }

}
