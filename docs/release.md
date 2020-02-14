# Release Process

## Prerequisites

Ensure all requirements are met before release:

1. **All tests passing**: `ctest --test-dir build`
2. **Documentation updated**: All new features documented
3. **CHANGELOG.md updated**: Include all user-facing changes
4. **Version bumped**: Update in `package.json`, `cpp/libs/core/CMakeLists.txt`

## Creating a Release

### 1. Bump Version

```sh
# Update version in package.json
npm version patch  # 0.16.0 -> 0.16.1

# Update version in CMakeLists.txt
# Manually update: version 0.16.1
```

### 2. Create Release Tag

```sh
git tag -a v0.16.1 -m "Release v0.16.1"
```

### 3. Build Release Binaries

```sh
# Clean previous build
rm -rf build

# Build release binaries
cmake -B build -S cpp -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Verify binaries
./build/dashpay --version
```

### 4. Build NPM Package

```sh
npm run build
```

### 5. Publish to NPM

```sh
npm publish
```

### 6. Push to GitHub

```sh
git push origin main --tags
```

## Post-Release

1. **Create GitHub Release**: Go to Releases page and create new release
2. **Update Homebrew**: If applicable, submit PR to homebrew-core
3. **Announce**: Update documentation with new features
