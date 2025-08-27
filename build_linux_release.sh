#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Clear previous builds
echo "Cleaning previous release directory..."
rm -rf release
mkdir -p release

# Build 64-bit libraries
echo "Building 64-bit libraries..."
mkdir -p build_64
cd build_64
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../toolchain-x86_64.cmake
cmake --build .

# Copy 64-bit artifacts
if [ -f "libsteam_api.so" ]; then
    cp libsteam_api.so ../release/libsteam_api64.so
    echo "Copied libsteam_api.so (64-bit) to release/libsteam_api64.so"
else
    echo "ERROR: libsteam_api.so (64-bit) not found in build_64!"
    exit 1
fi
if [ -f "lobby_connect" ]; then
    cp lobby_connect ../release/lobby_connect64
    echo "Copied lobby_connect (64-bit) to release/lobby_connect64"
else
    echo "WARNING: lobby_connect (64-bit) not found in build_64. Skipping."
fi
if [ -f "generate_interfaces_file" ]; then
    cp generate_interfaces_file ../release/generate_interfaces_file64
    echo "Copied generate_interfaces_file (64-bit) to release/generate_interfaces_file64"
else
    echo "WARNING: generate_interfaces_file (64-bit) not found in build_64. Skipping."
fi

cd ..
rm -rf build_64
echo "Finished 64-bit build."

# 32-bit build is currently not supported due to complex multi-arch dependencies.
echo "Skipping 32-bit library build: Currently unsupported due to multi-arch dependency complexities."

# Copy additional files
echo "Copying additional files..."
cp Readme_release.txt release/Readme.txt
rsync -a files_example/ release/files_example/
echo "Additional files copied."

echo "Linux build process complete."
