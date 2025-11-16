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

#### Build

```powershell
invoke build --type release
```

#### Clean

```powershell
invoke clean --type release
```

#### Edit

> Start Visual Studio Code. Make sure you install the recommended workspace extensions when asked.

```powershell
code .
```

## Build with Visual Studio

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
start .\primo-avblocks-cpp.sln
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
start .\primo-avblocks-cpp.sln
popd
```

### Generate Visual Studio 2022 project

```powershell
new-item -Force -ItemType Directory ./vs2022
pushd ./vs2022
cmake cmake -G "Visual Studio 17 2022" -A x64 -DPLATFORM=x64 ..
popd  
```

Open the project in Visual Studio:

```powershell
pushd ./vs2022 
start .\primo-avblocks-cpp.sln
popd
```

### Build

In the Visual Studio menu select `Build| Build Solution`

### Clean

In the Visual Studio menu select `Build| Clean Solution` 