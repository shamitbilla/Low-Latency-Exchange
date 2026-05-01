ld Directory ---
BUILD_DIR="build"

# --- 2. Create and Enter Build Directory ---
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# --- 3. Configure CMake with Conan Toolchain and LSP Flag ---
cmake .. \
	    -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
	        -DCMAKE_BUILD_TYPE=Release \
		    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# --- 4. Return to Project Root (Optional but good practice) ---
cd ..

echo "Configuration complete. 'compile_commands.json' is in $BUILD_DIR."
