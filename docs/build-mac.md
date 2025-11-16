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

```bash
invoke build --type release
```

#### Clean

```bash
invoke clean --type release
```

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
cmake -G 'Xcode' -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=x64 ..
popd  
```

Open the project in Xcode:

```bash
pushd ./xcode 
open primo-avblocks-cpp.xcodeproj
popd
```

### Build

In the Xcode menu select `Product | Scheme | ALL_BUILD`

In the Xcode menu select `Product | Build For | Running`

### Clean

In the Xcode menu select `Product | Clean Build Folder` 