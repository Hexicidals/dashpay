# Maintenance Guide

## Overview

This document outlines maintenance procedures and best practices for the DashPay C++ CLI.

## Project Health

### Current Status
- **Version**: v0.16.0-alpha
- **Platform**: C++20
- **Build System**: CMake 3.20+
- **Commits**: 166
- **Last Update**: February 2020
- **Maintainer**: Hexicidals <wedeving@proton.me>

## Development Workflow

### 1. Feature Development

1. **Create Feature Branch**
   ```bash
   git checkout -b feature/my-feature-name
   ```

2. **Implement Changes**
   - Add .cpp implementations
   - Update header files
   - Add tests
   - Update documentation

3. **Test Changes**
   ```bash
   cd build
   ctest --test-dir build
   ```

4. **Push Feature Branch**
   ```bash
   git push -u origin feature/my-feature-name
   ```

5. **Create Pull Request**
   - Open PR on GitHub with detailed description
   - Include CI/CD results
   - Reference related issues

### 2. Building

```bash
# Debug build
cmake -B build-debug -S cpp -DCMAKE_BUILD_TYPE=Debug
make -C build-debug -j$(nproc)

# Release build
cmake -B build-release -S cpp -DCMAKE_BUILD_TYPE=Release
make -C build-release -j$(nproc)

# Test coverage
cmake -B build-coverage -S cpp -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
make -C build-coverage -j$(nproc)
```

### 3. Testing

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run specific test
ctest --test-dir build --run-tests "TransactionTests"

# Generate coverage report
gcov build-debug/coverage.info
```

### 4. Platform-Specific Builds

#### macOS
```bash
brew install cmake pkg-config libcurl openssl

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
sudo make install
```

#### Windows (MSVC)
```powershell
cmake -B build -S cpp -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cmake --build . --config Release -- /maxcpucount
```

#### Linux
```bash
sudo apt-get install build-essential cmake pkg-config libcurl4-openssl-dev

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

### 5. Code Style

Follow [CONTRIBUTING.md](CONTRIBUTING.md) for code style guidelines.

**Key Points**:
- Use `std::format` for string formatting
- Use `std::expected<T, E>` for error handling
- Prefer `std::unique_ptr` for ownership semantics
- Use `std::shared_ptr` for shared resources
- Follow RAII principles
- Use `const` and `constexpr` where appropriate

### 6. Commit Message Format

```
<type>: <short description>

<type> options:
- feat: New feature
- fix: Bug fix
- docs: Documentation changes
- style: Code style improvements
- refactor: Code refactoring
- test: Test additions or modifications
- chore: Build process or auxiliary tool changes
- perf: Performance improvements
- ci: CI/CD changes

<scope>: <affected component>

Examples:
- feat(core): Add split payment support
- fix(keystore): Resolve Windows Hello authentication issue
- docs(api): Update API endpoint documentation
- refactor(cli): Improve command execution flow
- test(core): Add transaction validation tests
- chore(build): Update CMakeLists.txt for new library
```

### 7. Version Management

Follow [release.md](release.md) for version bump procedures.

**Current Version**: v0.16.0-alpha

**Next Version**: v0.17.0

**Release Types**:
- Major: Breaking changes
- Minor: New features (backwards compatible)
- Patch: Bug fixes

**Version Bump**:
```bash
# Update CHANGELOG.md
# Update version in package.json
# Create git tag
git tag -a v0.17.0 -m "Release v0.17.0"
git push origin main --tags
```

### 8. Branch Management

```bash
# List all branches
git branch -a

# Delete stale branches
git branch -D feature/abandoned-feature

# Rename current branch
git branch -m new-branch-name

# Checkout branch
git checkout new-branch-name
```

### 9. Troubleshooting

#### Build Failures
```bash
# Clean build directory
rm -rf build
cmake .. -B build -DCMAKE_BUILD_TYPE=Release
```

#### Runtime Errors
- **Wallet not found**: Run `dashpay setup`
- **RPC connection failed**: Check network connectivity
- **Key generation failed**: Check OpenSSL installation

#### Debug Mode

```bash
# Enable verbose logging
export DASHPAY_DEBUG=1
dashpay --verbose your-command

# Generate debug report
dashpay debug --status
```

### 10. Security

See [SECURITY.md](SECURITY.md) for:
- Key storage policies
- Authentication mechanisms
- Input validation
- Network communication security

### 11. Resources

- **Documentation**: [README.md](README.md)
- **API Reference**: [docs/api.md](docs/api.md)
- **Contributing**: [CONTRIBUTING.md](CONTRIBUTING.md)
- **Issues**: [https://github.com/Hexicidals/dashpay/issues](https://github.com/Hexicidals/dashpay/issues)

## Roadmap

### v0.17.0
- Complete C++ implementations for all headers
- Add comprehensive test suite
- Release v0.17.0-beta

### v0.18.0 (Future)
- Multi-hop payment support
- Payment protocol improvements
- Advanced debugging features
- Complete GUI components

---

**Last Updated**: February 2020
**Maintainer**: Hexicidals <wedeving@proton.me>
