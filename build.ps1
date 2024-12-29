$BUILD_DIR = "build"
$CONFIGURATION = "Release"
$EXE_NAME = "engine"

# Create the build directory if it doesn't exist
if (-Not (Test-Path $BUILD_DIR)) {
    New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
    Write-Host "Created build directory: $BUILD_DIR"
}

Push-Location $BUILD_DIR

# Run CMake
try {
    Write-Host "Running CMake configuration..."
    cmake .. -G "Visual Studio 17 2022" -A x64
    Write-Host "CMake configuration completed."
} catch {
    Write-Host "CMake configuration failed: $_"
    Pop-Location
    exit 1
}

# Build the project
try {
    Write-Host "Building the project..."
    cmake --build . --config $CONFIGURATION
    Write-Host "Build completed."
} catch {
    Write-Host "Build failed: $_"
    Pop-Location
    exit 1
}

# Run the executable
$exePath = ".\$CONFIGURATION\$EXE_NAME.exe"
if (Test-Path $exePath) {
    Write-Host "Running the executable..."
    & $exePath
} else {
    Write-Host "Executable not found: $exePath"
}

Pop-Location