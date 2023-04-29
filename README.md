# wasm-raymarcher
Recreation of jsRayMarcher in C/wasm using SIMD instructions where possible

## Running:
You need to compile the actual wasm. To do so, please run `./start.sh`.

This requires llvm, lld, and node (for the webserver hosting the wasm for CORS) to be installed.