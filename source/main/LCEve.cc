// -- lceve headers
#include <LCEve/EventDisplay.h>

int main (int argc, const char **argv) {
  lceve::EventDisplay eventDisplay ;
  eventDisplay.Init( argc, argv ) ;
  eventDisplay.Run() ;
  return 0 ;
}
