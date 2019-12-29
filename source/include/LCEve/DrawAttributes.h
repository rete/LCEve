#pragma once

// -- root headers
#include <TColor.h>

// -- std headers
#include <vector>
#include <optional>
#include <functional>

namespace lceve {
  
  using ColorType_t = Int_t ;
  using ColorList_t = std::vector<ColorType_t> ;
  using ColorNameList_t = std::vector<std::string> ;

  /// ColorHelper class
  /// A set of helper functions dealing with ROOT colors
  class ColorHelper {
  public:
    /// A predefined list of colors to iterate over
    static const ColorList_t fgPredefinedColors ;
    
    /// Same as above but by corresponding string names 
    static const ColorNameList_t fgPredefinedColorNames ;
    
    /// Color functor iterator
    using Function_t = std::function<ColorType_t()> ;
    
    /// Find the corresponding color number in the ROOT color palette
    /// Allowed parameters:
    ///  - a predefined color name (see above). Example: "red"
    ///  - an hexadecimal color code. Example: "#c0c0c0"
    ///  - a list of 3 integers, comma separated "r,g,b". Example: "125,255,255"
    /// Returns std::nullopt_t if not matching any of the options listed above
    static std::optional<ColorType_t> GetColor( const std::string &color ) ;
    
    /// Get a color generator function. Possible cases:
    ///   - "iter" : the functor will generate a new color from the predefined color list
    ///   - anything else that GetColor() can parse: the functor will always return the same color
    ///   - else: null functor 
    static Function_t GetColorFunction( const std::string &color ) ;
    
    /// Get a random color based on the memory address of the object
    template <typename T>
    inline static Int_t RandomColor( const T *const ptr ) {
      auto addr = reinterpret_cast<std::size_t>( ptr ) ;
      return fgPredefinedColors[addr % fgPredefinedColors.size()] ;
    }
    
    /// Get a color has a function of the PDG value
    static ColorType_t GetPDGColor( int pdg ) ;
  };
  
  /**
   *  @brief  ColorIterator class
   *  Helper class to iterate over colors
   */
  class ColorIterator {
  public:
    /// Constructor
    ColorIterator() = default ;
    ColorIterator( ColorIterator && ) = default ;
    ColorIterator( const ColorIterator & ) = default ;
    /// Default destructor
    ~ColorIterator() = default ;
    
    /// Increment the color iterator and return the new color
    ColorType_t NextColor() ;

    /// Get the current color
    ColorType_t CurrentColor() const ;

  private:
    ColorList_t::const_iterator    fCurrentColor { ColorHelper::fgPredefinedColors.begin() } ;
  };
  
}
