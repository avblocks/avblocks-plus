# Build on Linux

> Scripts are `bash`

The project comes with a pre-configured Visual Studio Code workspace which uses `cmake` and `ninja` as a build system. 

## Setup

Make sure you have done the [setup for Linux](./setup-linux.md) 

## Build with Ninja

> This is the recommended way for building and working with the sample code.

#### Configure

```bash
source ./configure.sh
```

#### Build

```bash
./scripts/build.sh --type debug
```

#### Clean

```bash
./scripts/clean.sh --type debug
```    

#### Edit

> Start Visual Studio Code. Make sure you install the recommended workspace extensions when asked.

```bash
code .
```

## Build with CMake Presets

> CMake Presets require CMake 3.19 or later.

Available presets: `debug-x64`, `release-x64`, `debug-demo-x64`, `release-demo-x64`

#### Configure

```bash
cmake --preset debug-x64
```

#### Build

```bash
cmake --build --preset debug-x64
```

#### Clean

```bash
cmake --build --preset debug-x64 --target clean
```

