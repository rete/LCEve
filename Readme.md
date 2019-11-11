# LCEve - Linear Collider Event Display

LCEve is an event display based on the experimental ROOT Eve7 component using web browser for the client vizualisation.

**Note that this software is experimental and developed against the ROOT master branch at the time being**

## Requirements

- A decent compiler with c++14 available (gcc >= 5.4)
- CMake >= 3.14
- LCIO: a special branch is needed in this context. Use the HEAD version of the github repo `rete/LCIO`, branch `sio-external`
- ROOT: master branch. Must have been compiled with `root7=ON` and `webgui=ON`
- DD4hep: I use the master branch for the development. You can probably use the latest version. I don't require any specific feature.


## Compilation

```shell
source /path/to/dd4hep/bin/thisdd4hep.sh
mkdir build
cd build
cmake -DLCIO_DIR=/path/to/lcio ..
make install -j42
```

## Running the event display

The standard help switch is your friend:
```shell
# to get the help
./bin/LCEve --help
```

As of today:
```
# Just with the geometry
./bin/LCEve -c /path/to/your/compactfile.xml
# Geometry and LCIO events
./bin/LCEve -c /path/to/your/compactfile.xml -f /path/to/your/lciofile.slcio
```

More options will be added later on. Again, the help switch is your friend.

Have fun ! :-)
