
#include <LCEve/DrawAttributes.h>
#include <LCEve/ParticleHelper.h>

#include <UTIL/BitField64.h>

namespace lceve {
  
  const ColorList_t ColorHelper::fgPredefinedColors = {
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
  
  //--------------------------------------------------------------------------
  
  const ColorNameList_t ColorHelper::fgPredefinedColorNames = {
    // Standard colors
    "red", "green", "blue", "magenta", "cyan", "violet", "pink",
    "orange", "yellow", "spring", "teal", "azure", "gray",
    // Dark colors
    "darkRed", "darkGreen", "darkBlue", "darkMagenta", "darkCyan",
    "darkViolet", "darkPink", "darkOrange", "darkYellow",
    // Bright colors
    "brightGreen", "brightBlue", "brightRed", "brightMagenta", "brightCyan",
    "brightViolet", "brightPink", "brightOrange", "brightYellow"
  };

  //--------------------------------------------------------------------------

  ColorType_t ColorIterator::NextColor() {
    fCurrentColor++ ;
    if( ColorHelper::fgPredefinedColors.end() == fCurrentColor ) {
      fCurrentColor = ColorHelper::fgPredefinedColors.begin() ;
    }
    return CurrentColor() ;
  }

  //--------------------------------------------------------------------------

  ColorType_t ColorIterator::CurrentColor() const {
    return *fCurrentColor ;
  }
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  std::optional<ColorType_t> ColorHelper::GetColor( const std::string &color ) {
    // predefined color ?
    auto predIter = std::find( ColorHelper::fgPredefinedColorNames.begin(), ColorHelper::fgPredefinedColorNames.end(), color ) ;
    if( ColorHelper::fgPredefinedColorNames.end() != predIter ) {
      return ColorHelper::fgPredefinedColors[ std::distance(ColorHelper::fgPredefinedColorNames.begin() , predIter ) ] ;
    }
    // #rrggbb form
    if( color.size() == 7 and color[0] == '#' ) {
      return TColor::GetColor(color.c_str()) ;
    }
    // r,g,b form
    std::string col = color ;
    std::size_t pos = col.find(',') ;
    while( pos != std::string::npos ) {
      col.replace( pos, 1, " " ) ;
      pos = col.find(',') ;
    }
    int r, g, b ;
    std::stringstream ss ; 
    ss >> r >> g >> b ;
    if( not ss.fail() ) {
      return TColor::GetColor( r, g, b ) ;
    }
    return std::nullopt ;
  }
  
  //--------------------------------------------------------------------------
  
  ColorHelper::Function_t ColorHelper::GetColorFunction( const std::string &color ) {
    if( color == "iter" ) {
      ColorIterator coliter ;
      return [=]() mutable {
        return coliter.NextColor() ;
      } ;
    }
    auto col = GetColor( color ) ;
    if( col.has_value() ) {
      auto fixcol = col.value() ;
      return [=](){
        return fixcol ;
      } ;
    }
    return nullptr ;
  }
  
  //--------------------------------------------------------------------------
  
  ColorType_t ColorHelper::GetPDGColor( int pdg ) {
    auto particle = ParticleHelper::GetParticle( pdg ) ;
    if( nullptr == particle ) {
      return kBlack ; // Can happen with weird generator particles ...
    }
    if( ParticleHelper::IsPhoton( particle ) ) {
      return kCyan ;
    }
    if( ParticleHelper::IsMuon( particle ) ) {
      return kOrange ;
    }
    if( ParticleHelper::IsElectron( particle ) ) {
      return kAzure ;
    }
    if( ParticleHelper::IsHadron( particle ) ) {
      return kRed ;
    }
    // random color if not found
    return RandomColor( &pdg ) ;
  }

}
