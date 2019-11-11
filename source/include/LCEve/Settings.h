
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
    inline void SetReadCollectionNames( const std::vector<std::string> &cols ) { _readCollectionNames = cols ; }
    inline const std::vector<std::string> &GetReadCollectionNames() const      { return _readCollectionNames ; }

    /// The detector depth level to draw
    inline void SetDetectorLevel( int level ) { _detectorLevel = level ; }
    inline int GetDetectorLevel() const       { return _detectorLevel ; }

    /// Server mode. Whether to show a window on startup or not (server mode = no window at startup)
    inline void SetServerMode( bool server ) { _serverMode = server ; }
    inline bool GetServerMode() const        { return _serverMode ; }

    /// DST mode. Load only reconstructed quantities from LCIO files.
    /// Adjust the display according to this mode
    inline void SetDSTMode( bool dst ) { _dstMode = dst ; }
    inline bool GetDSTMode() const { return _dstMode ; }

  private:
    std::vector<std::string>           _readCollectionNames {} ;
    int                                _detectorLevel {1} ;
    bool                               _serverMode {false} ;
    bool                               _dstMode {false} ;
  };

}
