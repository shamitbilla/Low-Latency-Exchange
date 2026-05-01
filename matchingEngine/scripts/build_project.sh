#!/bin/bash
# -----------------------------------------------------------------------------
# Build Script for MatchingEngine
# This script performs the fast, incremental build after C++ code changes.
# -----------------------------------------------------------------------------

BUILD_DIR="build"
BUILD_TYPE="Release"

# Ensure the build directory exists and has been configured
if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory '$BUILD_DIR' not found. Run ./configure_project.sh first."
    exit 1
fi

echo "Performing incremental build..."

# Use the fast, wrapper command to build from the configured directory
cmake --build $BUILD_DIR
if [ $? -eq 0 ]; then
    echo "Build success! Run: ./$BUILD_DIR/matching_engine"
fi
