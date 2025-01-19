import os
import subprocess
import sys
import argparse
import shutil
import platform
import time

BUILD_DIR = "build"
CONFIGURATION = "Release"
EXE_NAME = "engine"
CMAKE_GENERATOR = "Visual Studio 17 2022"
ARCHITECTURE = "x64"


def get_args():
    parser = argparse.ArgumentParser(
        description="Build the project using CMake."
    )
    parser.add_argument(
        "--configuration",
        help="The build configuration (default: Release)",
        default=CONFIGURATION,
        choices=["Debug", "Release"],
    )
    parser.add_argument(
        "--clean",
        help="Clean the build directory before building",
        action="store_true",
    )
    parser.add_argument(
        "--run",
        help="Run the executable after building",
        action="store_true",
    )
    parser.add_argument(
        "--threads",
        help="Number of threads to use for build (default: number of CPU cores)",
        type=int,
        default=os.cpu_count(),
    )
    return parser.parse_args()

args = get_args()

if args.clean:
    if os.path.exists(BUILD_DIR):
        print(f"Cleaning build directory: {BUILD_DIR}")
        shutil.rmtree(BUILD_DIR)

# Create the build directory if it doesn't exist
if not os.path.exists(BUILD_DIR):
    os.makedirs(BUILD_DIR)
    print(f"Created build directory: {BUILD_DIR}")

# Change to the build directory
os.chdir(BUILD_DIR)

# Run CMake
try:
    print("Running CMake configuration...")
    subprocess.run([
        "cmake", "..", "-G", CMAKE_GENERATOR, "-A", ARCHITECTURE
    ], check=True)
    print("CMake configuration completed.")
except subprocess.CalledProcessError as e:
    print(f"CMake configuration failed: {e}")
    sys.exit(1)

# Build the project
try:
    start_time = time.time()
    print("Building the project...")
    subprocess.run([
        "cmake", "--build", ".", "--config", args.configuration, "--parallel", str(args.threads)
    ], check=True)
    end_time = time.time()
    print(f"Build completed in {end_time - start_time:.2f} seconds.")
except subprocess.CalledProcessError as e:
    print(f"Build failed: {e}")
    sys.exit(1)

# Run the executable if requested
if args.run:
    exe_path = os.path.join(BUILD_DIR, args.configuration, f"{EXE_NAME}.exe")
    os.chdir("..")
    if os.path.exists(exe_path):
        print("Running the executable...")
        subprocess.run([exe_path])
    else:
        print(f"Executable not found: {exe_path}")

os.chdir("..")
