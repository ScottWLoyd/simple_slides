@echo off

IF NOT EXIST build (
mkdir build
)

pushd build

cl ../src/main.c /I ../../external/SDL2/include /link /LIBPATH:../../external/SDL2/bin/x64/ SDL2.lib SDL2_ttf.lib && main.exe

popd