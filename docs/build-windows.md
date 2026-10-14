# Build on Windows

> Scripts are `PowerShell`

The project comes with a pre-configured Visual Studio Code workspace which uses `cmake` and `ninja` as a build system. 

## Setup

Make sure you have done the [setup for Windows](./setup-windows.md) 

## Build with Ninja

> This is the recommended way for building and working with the sample code.

#### Configure

```powershell
. .\configure.ps1
```

`configure.ps1` sets up the Visual C++ environment for the host architecture (`arm64` on Windows on ARM, `x64` otherwise).

#### Build

The target architecture defaults to the host (`arm64` on ARM64, `x64` on AMD64). Use `-platform` to override it.

```powershell
./scripts/build.ps1 -type release

# explicit platform
./scripts/build.ps1 -type release -platform arm64
./scripts/build.ps1 -type release -platform x64
```

> The `sdk/` directory must contain the AVBlocks Core build for the same architecture (`sdk/lib/arm64` or `sdk/lib/x64`). Output goes to `build/<type>_<platform>`, e.g. `build/release_arm64`, and the executables to `bin/<platform>`.

> When building for a platform different from the host, run `configure.ps1` from a matching Visual Studio Developer PowerShell instead (e.g. `VsDevCmd.bat -arch=arm64 -host_arch=amd64` to cross-compile ARM64 on an x64 machine).

#### Clean

```powershell
./scripts/clean.ps1 -type release
./scripts/clean.ps1 -type release -platform arm64
```

#### CMake presets

Presets are also available for both architectures, e.g. `debug-arm64`, `release-arm64`, `debug-demo-arm64`, `release-demo-arm64` (and the `x64` equivalents).

#### Edit

> Start Visual Studio Code. Make sure you install the recommended workspace extensions when asked.

```powershell
code .
```

## Build with Visual Studio

> The commands below generate `x64` projects. For Windows on ARM64 replace `-A x64 -DPLATFORM=x64` with `-A ARM64 -DPLATFORM=arm64`. ARM64 is supported by Visual Studio 2019 and 2022.

### Generate Visual Studio 2017 project

```powershell
new-item -Force -ItemType Directory ./vs2017
pushd ./vs2017
cmake cmake -G "Visual Studio 15 2017" -A x64 -DPLATFORM=x64 ..
popd  
```

Open the project in Visual Studio:

```powershell
pushd ./vs2017 
start .\avblocks-plus.sln
popd
```

### Generate Visual Studio 2019 project

```powershell
new-item -Force -ItemType Directory ./vs2019
pushd ./vs2019
cmake cmake -G "Visual Studio 16 2019" -A x64 -DPLATFORM=x64 ..
popd  
```

Open the project in Visual Studio:

```powershell
pushd ./vs2019 
start .\avblocks-plus.sln
popd
```

### Generate Visual Studio 2022 project

```powershell
new-item -Force -ItemType Directory ./vs2022
pushd ./vs2022

# x64
cmake -G "Visual Studio 17 2022" -A x64 -DPLATFORM=x64 ..

# or ARM64
# cmake -G "Visual Studio 17 2022" -A ARM64 -DPLATFORM=arm64 ..

popd  
```

Open the project in Visual Studio:

```powershell
pushd ./vs2022 
start .\avblocks-plus.sln
popd
```

### Build

In the Visual Studio menu select `Build| Build Solution`

### Clean

In the Visual Studio menu select `Build| Clean Solution` 