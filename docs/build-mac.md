# Build on macOS

> Scripts are `bash`

The project comes with a pre-configured Visual Studio Code workspace which uses `cmake` and `ninja` as a build system. 

## Setup

Make sure you have done the [setup for macOS](./setup-mac.md) 

## Build with Ninja

> This is the recommended way for building and working with the sample code.

Start `bash` (if your default shell is `zsh` or spmething else):

```bash
bash
```

#### Configure

```bash
source ./configure.sh
```

#### Build

The target architecture defaults to the host (`arm64` on Apple Silicon, `x64` on Intel). Use `--platform` to override it.

```bash
./scripts/build.sh --type release

# explicit platform
./scripts/build.sh --type release --platform arm64
./scripts/build.sh --type release --platform x64
```

> The `sdk/` directory must contain the AVBlocks Core build for the same architecture. Output goes to `build/<type>_<platform>`, e.g. `build/release_arm64`.

#### Clean

```bash
./scripts/clean.sh --type release
./scripts/clean.sh --type release --platform arm64
```

#### CMake presets

Presets are also available for both architectures, e.g. `debug-arm64`, `release-arm64`, `debug-demo-arm64`, `release-demo-arm64` (and the `x64` equivalents).

#### Edit

> Start Visual Studio Code. Make sure you install the recommended workspace extensions when asked.

```bash
code .
```

## Build with Xcode

### Generate Xcode project

```bash
mkdir -p ./xcode
pushd ./xcode
cmake -G 'Xcode' -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=arm64 ..   # or x64 for Intel
popd  
```

Open the project in Xcode:

```bash
pushd ./xcode 
open avblocks-plus.xcodeproj
popd
```

### Build

In the Xcode menu select `Product | Scheme | ALL_BUILD`

In the Xcode menu select `Product | Build For | Running`

### Clean

In the Xcode menu select `Product | Clean Build Folder` 