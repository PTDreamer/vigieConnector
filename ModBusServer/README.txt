WINDOWS DEBUG BUILD:
qmake VirtualModbusClient.pro -r -spec win32-g++ "CONFIG+=debug"
mingw32-make -f Makefile.Debug

WINDOWS RELEASE BUILD:
qmake VirtualModbusClient.pro -r -spec win32-g++ "CONFIG+=release"
mingw32-make -f Makefile.Release

LINUX DEBUG BUILD:
qmake VirtualModbusClient.pro -r -spec linux-g++-64 "CONFIG+=debug"
make -f Makefile.Debug

LINUX RELEASE BUILD:
qmake VirtualModbusClient.pro -r -spec linux-g++-64 "CONFIG+=release"
make -f Makefile.Release

USAGE:
VirtualModBusClient -v=DEBUG_LEVEL -setting=PATH_TO_SETTINGS_FILE

