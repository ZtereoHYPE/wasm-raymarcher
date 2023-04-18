mkdir -p src/build

clang \
   --target=wasm32 \
   -O3 \
   -flto \
   -nostdlib \
   -Wl,--no-entry \
   -Wl,--export-all \
   -Wl,--lto-O3 \
   -Wl,-z,stack-size=$[8 * 1024 * 1024] \
   -o src/build/main.wasm \
   -msimd128 \
  src/wasm/main.c

node src/webserver.js