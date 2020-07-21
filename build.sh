mkdir build
pushd build

gcc ../src/main.c -I../src -I/usr/include/SDL2 -o simple_slides `sdl2-config --cflags --static-libs` -lSDL2_ttf

popd
