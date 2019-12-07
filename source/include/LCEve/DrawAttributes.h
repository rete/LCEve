#pragma once

#include <TColor.h>
#include <vector>

namespace lceve {

  using ColorType = int ;
  using ColorList = std::initializer_list<int> ;

  /// A predefined list of colors to iterate over
  constexpr ColorList colorList = {
    // Standard colors
    kRed, kGreen, kBlue, kMagenta, kCyan, kViolet, kPink,
    kOrange, kYellow, kSpring, kTeal, kAzure, kGray,
    // Dark colors
    kRed+2, kGreen+2, kBlue+2, kMagenta+2, kCyan+2,
    kViolet+2, kPink+2, kOrange+2, kYellow+2,
    // Bright colors
    kGreen-9, kBlue-9, kRed-9, kMagenta-9, kCyan-9,
    kViolet-9, kPink-9, kOrange-9, kYellow-9
  };
  
  /// Get a random color based on the memory address of the object
  template <typename T>
  inline static Int_t RandomColor( const T *const ptr ) {
    static const std::vector<Int_t> colors {colorList} ;
    auto addr = reinterpret_cast<std::size_t>( ptr ) ;
    return colors[addr % colorList.size()] ;
  }

  enum class ColorIterStrategy {
    /// Always return the same color
    FIXED_COLOR,
    /// Iterate over the color table
    AUTO_ITER
  };

  /**
   *  @brief  ColorIterator class
   *  Helper class to iterate over colors
   */
  class ColorIterator {
  public:
    /// Constructor
    ColorIterator() ;

    /// Default destructor
    ~ColorIterator() = default ;

    /// Constructor with strategy and color
    /// If the startegy is "fixed", the second argument is the fixed color
    /// If the startegy is "auto", the color is the start color in the color table
    ColorIterator( ColorIterStrategy strategy, int color = *colorList.begin() ) ;

    /// Increment the color iterator and return the new color
    int NextColor() ;

    /// Get the current color
    int CurrentColor() const ;

    /// Fixed color accessor
    int FixedColor() const ;

  private:
    ColorIterStrategy             _strategy { ColorIterStrategy::AUTO_ITER } ;
    std::vector<int>              _colors = colorList ;
    std::vector<int>::iterator    _colorIter { _colors.begin() } ;
    int                           _fixedColor {1} ;
  };

}
