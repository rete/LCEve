#pragma once

// -- std headers
#include <vector>
#include <string>

namespace lceve {

  /**
   *  @brief  Settings class
   *  Holds the application settings of the event display.
   *  Most of these values can be steered by command line
   */
  class Settings {
  public:
    /// LCIO read collection names
    inline void SetReadCollectionNames( const std::vector<std::string> &cols ) { fReadCollectionNames = cols ; }
    inline const std::vector<std::string> &GetReadCollectionNames() const      { return fReadCollectionNames ; }

    /// The detector depth level to draw
    inline void SetDetectorLevel( int level ) { fDetectorLevel = level ; }
    inline int GetDetectorLevel() const       { return fDetectorLevel ; }

    /// Server mode. Whether to show a window on startup or not (server mode = no window at startup)
    inline void SetServerMode( bool server ) { fServerMode = server ; }
    inline bool GetServerMode() const        { return fServerMode ; }

    /// DST mode. Load only reconstructed quantities from LCIO files.
    /// Adjust the display according to this mode
    inline void SetDSTMode( bool dst ) { fDstMode = dst ; }
    inline bool GetDSTMode() const { return fDstMode ; }

  private:
    std::vector<std::string>           fReadCollectionNames {} ;
    int                                fDetectorLevel {1} ;
    bool                               fServerMode {false} ;
    bool                               fDstMode {false} ;
  };

}
