# Clone the Emscripten SDK repository
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# Fetch the latest version of the emsdk (not needed the first time you clone)
git pull

# Install and activate the latest SDK tools
./emsdk install latest
./emsdk activate latest
