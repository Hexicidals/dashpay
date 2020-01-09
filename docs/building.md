# Building DashPay

## Prerequisites

### Ubuntu/Debian

```sh
sudo apt-get update
sudo apt-get install -y build-essential cmake libssl-dev ninja-build
```

### macOS

```sh
brew install cmake openssl ninja
```

### Windows

```sh
# Install Visual Studio 2022 or later
# Install CMake and Ninja via winget
winget install -e --id cmake.ninja.ninja
```

## Build Instructions

```sh
# Clone the repository
git clone https://github.com/dashpay-org/dashpay.git
cd dashpay

# Create build directory
mkdir build && cd build

# Configure CMake
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build . --config Release

# Install (optional)
sudo cmake --install .
```

## Build Options

| Option | Default | Description |
|---------|----------|-------------|
| `-DCMAKE_BUILD_TYPE` | `Debug` | Build type (Debug, Release) |
| `-DWITH_SERVER` | `ON` | Build server components |
| `-DWITH_TESTS` | `ON` | Build test suite |
| `-DWITH_MCP` | `ON` | Build MCP server |

## Building with Tests

```sh
cmake -B build-test -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..
cmake --build build-test
ctest --test-dir build-test
```

## Cross-Compilation

### Linux to macOS

```sh
cmake -DCMAKE_SYSTEM_NAME=Darwin \
      -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ ..
```

### Linux to Windows

Use MinGW or Docker container:

```sh
docker run --rm -v $(pwd):/src -w /src \
    dashpayorg/dashpay-builder-windows:latest \
    cmake -B /src/build -G "MinGW Makefiles" ..
```
