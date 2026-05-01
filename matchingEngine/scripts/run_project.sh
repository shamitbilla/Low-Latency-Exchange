#!/bin/bash
# -----------------------------------------------------------------------------
# Run Script for MatchingEngine
# This script executes the built C++ application.
# -----------------------------------------------------------------------------

# IMPORTANT FIXES: In shell scripting, you MUST NOT use spaces around '=' 
# when assigning variables.

BUILD_DIR="./build" # Correct path is relative to the script's execution
APP_EXEC="matchingEngine"

echo "Running $APP_EXEC..."

# Check if the executable exists before attempting to run it
if [ ! -f "$BUILD_DIR/$APP_EXEC" ]; then
    echo "Error: Executable not found at $BUILD_DIR/$APP_EXEC"
    echo "Please ensure you have successfully run ./scripts/build_project.sh"
    exit 1
fi

# Execute the application

# Run matchingEngine in background, suppress logs
$BUILD_DIR/$APP_EXEC > /dev/null 2>&1 &

# Run subscriber in background, suppress logs
./build/subscriber > /dev/null 2>&1 &

# Run socat in foreground and see its logs
socat UDP4-RECVFROM:12345,ip-add-membership=239.255.0.1:0.0.0.0,fork STDOUT
# Capture the exit status
if [ $? -ne 0 ]; then
    echo "Application finished with an error. Exit code: $?"
fi

